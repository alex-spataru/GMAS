/*
 * Copyright (c) 2019 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Serial.h"

#include <QtMath>
#include <QTimer>
#include <QDebug>
#include <QXYSeries>
#include <QMetaType>
#include <QSerialPort>
#include <QMessageBox>
#include <QSerialPortInfo>

//
// Magia
//
QT_CHARTS_USE_NAMESPACE
Q_DECLARE_METATYPE(QAbstractSeries *)
Q_DECLARE_METATYPE(QAbstractAxis *)

/**
 * Inicializa los miembros de la clase y comienza a buscar
 * dispositivos serial
 */
Serial::Serial() {
    // Inicializar valores
    m_puerto = Q_NULLPTR;
    m_lenDatos = -1;
    m_frecuencia = 0;
    m_amplitud = 0;
    m_gmasHabilitado = false;

    // Registrar tipos de datos
    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();

    // Comenzar busqueda de dispositivos
    QTimer::singleShot(1000, this, &Serial::actualizarDispositivosSerial);
}

/**
 * Cierra la conexión con el dispositivo serial antes de que la ejecución
 * del programa termine.
 */
Serial::~Serial() {
    desconectarDispositivo();
}

/**
 * Regresa la amplitud target actual
 */
qreal Serial::amplitud() const {
    return m_amplitud;
}

/**
 * Regresa la frequencia target actual
 */
qreal Serial::frecuencia() const {
    return m_frecuencia;
}

/**
 * Regresa la frequencia minima posible
 */
qreal Serial::frecuenciaMin() const {
    return 0;
}

/**
 * Regresa la frequencia maxima posible
 */
qreal Serial::frecuenciaMax() const {
    return 100;
}

/**
 * Regresa la amplitud minima posible
 */
qreal Serial::amplitudMin() const {
    return 0;
}

/**
 * Regresa la amplitud maxima posible
 */
qreal Serial::amplitudMax() const {
    return 5;
}

/**
 * Regresa el numero de lecturas que tomamos antes de
 * actualizar la gráfica
 */
int Serial::numLecturas() const {
    return 2 * static_cast<int>(frecuenciaMax());
}

/**
 * Regresa @a true si el GMAS esta habilitado
 */
bool Serial::gmasHabilitado() const {
    return m_gmasHabilitado;
}

/**
 * Regresa @a true si estamos conectados a algun dispositivo
 * serial
 */
bool Serial::conexionConDispositivo() const {
    if (m_puerto != Q_NULLPTR)
        return m_puerto->isOpen();

    return false;
}

/**
 * Regresa una lista con los dispositivos serial disponibles
 */
QStringList Serial::dispositivosSerial() const {
    return m_dispositivosSerial;
}

/**
 * Intenta establecer una conexión a 9600 baudios con el
 * @a device seleccionado.
 *
 * @return @a true si la conexión se establecio con éxito,
 *         @a false si hubo algún error
 */
bool Serial::conectarADispositivo(const int device) {
    // Disconectar el dispositivo actual
    desconectarDispositivo();

    // Obtener lista de puertos serial
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    // Checar que el dispositivo sea valido
    assert(device < ports.count());

    // Configurar nuevo dispositivo serial
    m_puerto = new QSerialPort(ports.at(device));
    m_puerto->setBaudRate(9600);

    // Conectar señales para poder leer datos del dispositivo
    connect(m_puerto, SIGNAL(readyRead()),
            this,     SLOT(onDatosRecibidos()));
    connect(m_puerto, SIGNAL(aboutToClose()),
            this,     SLOT(desconectarDispositivo()));

    // Intentar abrir una conexion con el dispositivo
    if (m_puerto->open(QIODevice::ReadWrite)) {
        emit conexionCambiada();
        return true;
    }

    // Hubo un error al abrir la conexión
    else {
        desconectarDispositivo();
        QMessageBox::warning(Q_NULLPTR,
                             tr("Error de comunicación"),
                             tr("Error al intentar establecer una conexión con %1")
                             .arg(ports.at(device).portName()));
    }

    // Hubo algun error
    return false;
}

/**
 * Habilita o des-habilita el GMAS
 */
void Serial::habilitarGmas(const bool enabled) {
    m_gmasHabilitado = enabled && conexionConDispositivo() && dispositivosSerial().count() > 0;
    emit gmasEstadoCambiado();
}

/**
 * Cambia la frecuencia target a la que se movera el GMAS
 */
void Serial::cambiarFrecuencia(const qreal frequency) {
    assert(frequency >= frecuenciaMin());
    assert(frequency <= frecuenciaMax());

    m_frecuencia = frequency;
    emit frecuenciaCambiada();
}

/**
 * Cambia la amplitud target a la que se movera el GMAS
 */
void Serial::cambiarAmplitud(const qreal amplitude) {
    assert(amplitude >= amplitudMin());
    assert(amplitude <= amplitudMax());

    m_amplitud = amplitude;
    emit amplitudCambiada();
}

/**
 * Actualiza los datos de la gráfica @a series con los valores
 * que le corresponden a la @a signal establecida:
 *   - 0: lecturas de posicion en el eje x
 *   - 1: lecturas de posicion en el eje y
 *   - 2: lecturas de posicion en el eje z
 */
void Serial::actualizarGrafica(QAbstractSeries* series, const int signal) {
    // Verificaciones
    assert(signal >= 0);
    assert(signal <= 3);
    assert(series != Q_NULLPTR);

    // Obtener puntos para la señal especificada
    QVector<QPointF> data;
    switch (signal) {
    case 0:
        data = m_lecturasX;
        break;
    case 1:
        data = m_lecturasY;
        break;
    case 2:
        data = m_lecturasZ;
        break;
    }

    // Convertir la gráfica a XY y remplazar puntos
    static_cast<QXYSeries*>(series)->replace(data);
}

/**
 * Llamado cuando recibimos cualquier número de bytes del dispositivo
 * serial, genera y separa paquetes de datos para su posterior interpretacion
 */
void Serial::onDatosRecibidos() {
    // Verificar apuntador del puerto serial
    if (!m_puerto)
        return;

    // Leer datos del puerto serial
    m_lenDatos += m_puerto->bytesAvailable();
    m_buffer.append(m_puerto->readAll());

    // El buffer contiene la secuencia de terminacion, lo cual representa un
    // paquete
    if (m_buffer.contains(';')) {
        // Pudimos haber recibido parte del siguiente paquete, o mas de un
        // paquete, para evitar errores, separamos los paquetes individualmente
        QList<QByteArray> paquetes = m_buffer.split(';');

        // Verificar si el ultimo paquete esta completo o no
        if (m_buffer.endsWith(";"))
            m_buffer.clear();
        else {
            m_buffer = paquetes.last();
            paquetes.removeLast();
        }

        // Leer cada paquete de manera individual
        foreach (QByteArray paquete, paquetes)
            interpretarPaquete(paquete);
    }

    // Asegurarnos que el tamaño del buffer no excede los limites del programa
    if (m_buffer.length() >= 1024)
        m_buffer.clear();
}

/**
 * Realiza una doble integración con el promedio de los datos anteriores para obtener
 * la posición actual en [x,y,z] del sensor del GMAS.
 */
void Serial::actualizarPosicion() {
    // Actualizar graficas desde la UI
    emit posicionCalculada();
}

/**
 * Termina la conexión con el dispositivo serial actual (si hay alguno conectado)
 */
void Serial::desconectarDispositivo() {
    // Reset contador de bytes
    m_lenDatos = -1;

    // Verificar si el puerto serial es valido
    if (m_puerto != Q_NULLPTR) {
        // Disconectar señales del puerto serial
        m_puerto->disconnect(this, SLOT(onDatosRecibidos()));
        m_puerto->disconnect(this, SLOT(desconectarDispositivo()));

        // Cerrar y eliminar conexion
        m_puerto->close();
        m_puerto->deleteLater();

        // Reset apuntador
        m_puerto = Q_NULLPTR;
    }

    // Actualizar UI
    emit conexionCambiada();
}

/**
 * Obtiene una lista de dispositivos serial del sistema y alerta al
 * resto de la aplicacion si hubo algún cambio
 */
void Serial::actualizarDispositivosSerial() {
    // Buscar dispositivos serial
    QStringList dispositivos;
    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        if (port.isValid() && !port.description().isEmpty())
            dispositivos.append(port.portName());
    }

    // Si los dispositivos cambiaron, actualizar la UI
    if (dispositivos != m_dispositivosSerial) {
        m_dispositivosSerial = dispositivos;
        emit dispositivosCambiados();
    }

    // Llamar esta funcion dentro de un segundo
    QTimer::singleShot(1000, this, &Serial::actualizarDispositivosSerial);
}

/**
 * Decodifica las lecturas del acelerometro y giroscopio contenidas en
 * el paquete de @a datos, el cual tiene el sig. formato:
 *
 *          {ACCEL_X,ACCEL_Y,ACCEL_Z,GYRO_X,GYRO_Y,GYRO_Z};
 *
 * Posteriormente, se registran las lecturas del acelerometro y giroscopio
 * en una lista, la cual es usada para calcular la posicion relativa del
 * GMAS.
 */
void Serial::interpretarPaquete(const QByteArray& datos) {
    // El paquete esta vacio
    if (datos.length() <= 0)
        return;

    // Checar si el paquete es invalido
    // (el ';' ya fue removido por el proceso de seleccion de paquetes).
    if (!datos.endsWith("}") || !datos.startsWith("{"))
        return;

    // Quitar primer caracter y ultimos dos caracteres
    QByteArray copia = datos;
    copia.remove(0, 1);
    copia.chop(2);

    // Generar una lista a partir de los datos del paquete
    QList<QByteArray> lecturas = copia.split(',');

    // Checar si la lista contiene un numero invalido de elementos
    if (lecturas.count() != 6)
        return;

    // Obtener lecturas individuales
    QString acclX(lecturas.at(0));
    QString acclY(lecturas.at(1));
    QString acclZ(lecturas.at(2));
    QString gyroX(lecturas.at(3));
    QString gyroY(lecturas.at(4));
    QString gyroZ(lecturas.at(5));

    // Generar vector de acelerometro
    QVector3D vectorAccl;
    vectorAccl.setX(acclX.toFloat());
    vectorAccl.setY(acclY.toFloat());
    vectorAccl.setZ(acclZ.toFloat());

    // Generar vector de giroscopio
    QVector3D vectorGyro;
    vectorGyro.setX(gyroX.toFloat());
    vectorGyro.setY(gyroY.toFloat());
    vectorGyro.setZ(gyroZ.toFloat());

    // Actualizar registro de lecturas
    m_lecturasAccl.append(vectorAccl);
    m_lecturasGyro.append(vectorGyro);

    // Eliminar lecturas viejas
    while (m_lecturasAccl.count() > numLecturas())
        m_lecturasAccl.removeFirst();
    while (m_lecturasGyro.count() > numLecturas())
        m_lecturasGyro.removeFirst();

    // Calcular posicion y actualizar datos para las graficas
    actualizarPosicion();
}
