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

#include <QDir>
#include <QtMath>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QXYSeries>
#include <QMetaType>
#include <QSerialPort>
#include <QMessageBox>
#include <QApplication>
#include <QSerialPortInfo>

//
// Magia
//
QT_CHARTS_USE_NAMESPACE
Q_DECLARE_METATYPE(QAbstractSeries *)
Q_DECLARE_METATYPE(QAbstractAxis *)

/**
 * Implementacion de una funcion que elimina los elementos mas viejos de una lista
 * mientras que el numero de lecturas de la lista es mayor al numero maximo de lecturas.
 */
template <typename T>
static void EliminarLecturasViejas(QVector<T> *vector, const int numMaximoElementos) {
    while (vector->count() > numMaximoElementos)
        vector->removeFirst();
}

/**
 * Inicializa los miembros de la clase y comienza a buscar
 * dispositivos serial
 */
Serial::Serial() {
    // Inicializar valores
    m_velocidad = 0;
    m_numLecturas = 0;
    m_puerto = Q_NULLPTR;
    m_gmasHabilitado = false;
    m_escala = escalaMax() / 2;

    // Registrar tipos de datos
    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();

    // Comenzar busqueda de dispositivos
    QTimer::singleShot(1000, this, &Serial::actualizarDispositivosSerial);

    // Comenzar a mandar datos de manera periodica
    QTimer::singleShot(1000, this, &Serial::mandarDatos);
}

/**
 * Cierra la conexión con el dispositivo serial antes de que la ejecución
 * del programa termine.
 */
Serial::~Serial() {
    desconectarDispositivo();
}

/**
 * Regresa el numero de lecturas maximas antes de que sea necesario limpiar
 * el buffer de datos temporales
 */
int Serial::escala() const {
    return m_escala;
}

/**
 * Regresa la amplitud target actual
 */
qreal Serial::velocidad() const {
    return m_velocidad;
}

/**
 * Regrasa la escala minima posible
 */
int Serial::escalaMin() const {
    return 100;
}

/**
 * Regresa la escala maxima
 */
int Serial::escalaMax() const {
    return 2000;
}

/**
 * Regresa la amplitud minima posible
 */
qreal Serial::velocidadMin() const {
    return 0;
}

/**
 * Regresa la amplitud maxima posible
 */
qreal Serial::velocidadMax() const {
    return 97;
}

/**
 * Regresa el numero de lecturas que tomamos antes de
 * actualizar la gráfica
 */
quint64 Serial::numLecturas() const {
    return m_numLecturas;
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
 * Intenta establecer una conexión a 115200 baudios con el
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
    m_puerto->setBaudRate(1000000);

    // Conectar señales para poder leer datos del dispositivo
    connect(m_puerto, SIGNAL(readyRead()),
            this,       SLOT(onDatosRecibidos()));
    connect(m_puerto, SIGNAL(aboutToClose()),
            this,       SLOT(desconectarDispositivo()));

    // Intentar abrir una conexion con el dispositivo
    if (m_puerto->open(QIODevice::ReadWrite)) {
        // Actualizar UI
        emit conexionCambiada();

        // Obtener tiempo actual
        QDateTime tiempo = QDateTime::currentDateTime();

        // Crear carpeta para guardar archivo de lecturas
        QDir dir = QDir::homePath() + "/" + qApp->applicationName() + "/";
        if (!dir.exists())
            dir.mkpath(".");

        // Obtener nombre para archivo de lecturas
        QString filename = QString("Lecturas-%1-%2.csv")
                .arg(m_puerto->portName())
                .arg(tiempo.toString("hh_mm_ss - dd_MMM_yyyy"));

        // Intentar abrir archivo de lecturas
        m_archivoLecturas.setFileName(dir.filePath(filename));
        if (!m_archivoLecturas.open(QFile::WriteOnly))
            qWarning() << "No se puede generar el archivo de lecturas";

        // Escribir titulos al archivo de salidas
        else {
            m_archivoLecturas.write("Num. Lectura, "
                                    "Aceleracion en X,"
                                    "Aceleracion en Y,"
                                    "Aceleracion en Z,"
                                    "Aceleracion Promedio,"
                                    "Fuerza Resultante\n");
        }

        // Regresar verdadero para notificar resultado
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
 * Actualiza la escala de las graficas
 */
void Serial::cambiarEscala(const int escala) {
    if (escala < escalaMin())
        m_escala = escalaMin();
    else if (escala > escalaMax())
        m_escala = escalaMax();
    else
        m_escala = escala;

    emit escalaCambiada();
}

/**
 * Habilita o des-habilita el GMAS
 */
void Serial::habilitarGmas(const bool enabled) {
    m_gmasHabilitado = enabled && conexionConDispositivo() && dispositivosSerial().count() > 0;
    emit gmasEstadoCambiado();
}

/**
 * Cambia la velocidad target a la que se movera el GMAS
 */
void Serial::cambiarVelocidad(const qreal velocidad) {
    assert(velocidad >= velocidadMin());
    assert(velocidad <= velocidadMax());

    m_velocidad = velocidad;
    emit velocidadCambiada();
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

    // No hacer nada si la grafica no es visible
    if (!series->isVisible())
        return;

    // Obtener puntos para la señal especificada
    QVector<QPointF>* data = Q_NULLPTR;
    switch (signal) {
    case 0:
        data = &m_lecturasX;
        break;
    case 1:
        data = &m_lecturasY;
        break;
    case 2:
        data = &m_lecturasZ;
        break;
    case 3:
        data = &m_lecturasP;
        break;
    }

    // Convertir la gráfica a XY y remplazar puntos
    if (data != Q_NULLPTR)
        static_cast<QXYSeries*>(series)->replace(*data);
}

/**
 * Manda los datos de control al GMAS
 */
void Serial::mandarDatos() {
    // Verificar si el puerto esta disponible
    if (conexionConDispositivo()) {
        QString datos = tr("%1;").arg(gmasHabilitado() ? velocidad() : 0);
        m_puerto->write(datos.toUtf8());
    }

    // Llamar esta funcion de nuevo en 100 ms
    QTimer::singleShot(100, this, &Serial::mandarDatos);
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
    // Obtener lecturas en X,Y,Z
    qreal lecX = static_cast<qreal>(m_lecturasAccl.last().x());
    qreal lecY = static_cast<qreal>(m_lecturasAccl.last().y());
    qreal lecZ = static_cast<qreal>(m_lecturasAccl.last().z());

    // Obtener distancia entre el punto de origen y el punto
    // tri-dimensional reportado por el MCU
    qreal posP = sqrt(pow(lecX, 2) + pow(lecY, 2) + pow(lecZ, 2));

    // Incrementar contador
    ++m_numLecturas;

    // Eliminar elementos viejos
    EliminarLecturasViejas<QPointF>(&m_lecturasX, escala());
    EliminarLecturasViejas<QPointF>(&m_lecturasY, escala());
    EliminarLecturasViejas<QPointF>(&m_lecturasZ, escala());
    EliminarLecturasViejas<QPointF>(&m_lecturasP, escala());
    EliminarLecturasViejas<QVector3D>(&m_lecturasAccl, escala());
    EliminarLecturasViejas<QVector3D>(&m_lecturasGyro, escala());

    // Generar puntos de posicion/tiempo
    QPointF puntoX(m_numLecturas, lecX);
    QPointF puntoY(m_numLecturas, lecY);
    QPointF puntoZ(m_numLecturas, lecZ);
    QPointF puntoP(m_numLecturas, posP);

    // Actualizar lista de posiciones
    m_lecturasX.append(puntoX);
    m_lecturasY.append(puntoY);
    m_lecturasZ.append(puntoZ);
    m_lecturasP.append(puntoP);

    // Guardar en archivo de lecturas
    if (m_archivoLecturas.isOpen()) {
        QString data = tr("%1,%2,%3,%4,%5,%6\n")
                .arg(m_numLecturas)
                .arg(lecX)
                .arg(lecY)
                .arg(lecZ)
                .arg(posP)
                .arg(posP * 0.1275);

        m_archivoLecturas.write(data.toUtf8());
    }
}

/**
 * Termina la conexión con el dispositivo serial actual (si hay alguno conectado)
 */
void Serial::desconectarDispositivo() {
    // Verificar si el puerto serial es valido
    if (m_puerto != Q_NULLPTR) {
        // Apagar motor
        if (conexionConDispositivo()) {
            for (int i = 0; i < 255; ++i)
                m_puerto->write("0;");
        }

        // Cerrar archivo de salida
        if (m_archivoLecturas.isOpen())
            m_archivoLecturas.close();

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
        if (!port.description().isEmpty())
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

    // Calcular posicion y actualizar datos para las graficas
    QTimer::singleShot(0, this, &Serial::actualizarPosicion);
}
