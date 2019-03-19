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
    m_port = Q_NULLPTR;
    m_dataLen = -1;
    m_frequency = 0;
    m_amplitude = 0;
    m_enabled = false;

    // Registrar tipos de datos
    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();

    // Comenzar busqueda de dispositivos
    QTimer::singleShot(1000, this, &Serial::refreshSerialDevices);
}

/**
 * Cierra la conexión con el dispositivo serial antes de que la ejecución
 * del programa termine.
 */
Serial::~Serial() {
    disconnectDevice();
}

/**
 * Regresa la amplitud target actual
 */
qreal Serial::amplitude() const {
    return m_amplitude;
}

/**
 * Regresa la frequencia target actual
 */
qreal Serial::frequency() const {
    return m_frequency;
}

/**
 * Regresa la frequencia minima posible
 */
qreal Serial::minFrequency() const {
    return 0;
}

/**
 * Regresa la frequencia maxima posible
 */
qreal Serial::maxFrequency() const {
    return 100;
}

/**
 * Regresa la amplitud minima posible
 */
qreal Serial::minAmplitude() const {
    return 0;
}

/**
 * Regresa la amplitud maxima posible
 */
qreal Serial::maxAmplitude() const {
    return 5;
}

/**
 * Regresa el numero de lecturas que tomamos antes de
 * actualizar la gráfica
 */
int Serial::samples() const {
    return 2 * static_cast<int>(maxFrequency());
}

/**
 * Regresa @a true si el GMAS esta habilitado
 */
bool Serial::enabled() const {
    return m_enabled;
}

/**
 * Regresa @a true si estamos conectados a algun dispositivo
 * serial
 */
bool Serial::isConnectedToDevice() const {
    if (m_port != Q_NULLPTR)
        return m_port->isOpen();

    return false;
}

/**
 * Regresa una lista con los dispositivos serial disponibles
 */
QStringList Serial::devices() const {
    return m_devices;
}

/**
 * Intenta establecer una conexión a 9600 baudios con el
 * @a device seleccionado.
 *
 * @return @a true si la conexión se establecio con éxito,
 *         @a false si hubo algún error
 */
bool Serial::startComm(const int device) {
    // Disconectar el dispositivo actual
    disconnectDevice();

    // Obtener lista de puertos serial
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    // Checar que el dispositivo sea valido
    assert(device < ports.count());

    // Configurar nuevo dispositivo serial
    m_port = new QSerialPort(ports.at(device));
    m_port->setBaudRate(9600);

    // Conectar señales para poder leer datos del dispositivo
    connect(m_port, SIGNAL(readyRead()),
            this,     SLOT(onDataReceived()));
    connect(m_port, SIGNAL(aboutToClose()),
            this,     SLOT(disconnectDevice()));

    // Intentar abrir una conexion con el dispositivo
    if (m_port->open(QIODevice::ReadWrite)) {
        emit connectedChanged();
        return true;
    }

    // Hubo un error al abrir la conexión
    else {
        disconnectDevice();
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
void Serial::setEnabled(const bool enabled) {
    m_enabled = enabled && isConnectedToDevice() && devices().count() > 0;
    emit enabledChanged();
}

/**
 * Cambia la frecuencia target a la que se movera el GMAS
 */
void Serial::setFrequency(const qreal frequency) {
    assert(frequency >= minFrequency());
    assert(frequency <= maxFrequency());

    m_frequency = frequency;
    emit frequencyChanged();
}

/**
 * Cambia la amplitud target a la que se movera el GMAS
 */
void Serial::setAmplitude(const qreal amplitude) {
    assert(amplitude >= minAmplitude());
    assert(amplitude <= maxAmplitude());

    m_amplitude = amplitude;
    emit amplitudeChanged();
}

/**
 * Actualiza los datos de la gráfica @a series con los valores
 * que le corresponden a la @a signal establecida:
 *   - 0: lecturas de posicion en el eje x
 *   - 1: lecturas de posicion en el eje y
 *   - 2: lecturas de posicion en el eje z
 */
void Serial::updateSeries(QAbstractSeries* series, const int signal) {
    // Verificaciones
    assert(signal >= 0);
    assert(signal <= 3);
    assert(series != Q_NULLPTR);

    // Obtener puntos para la señal especificada
    QVector<QPointF> data;
    switch (signal) {
    case 0:
        data = m_xReadings;
        break;
    case 1:
        data = m_yReadings;
        break;
    case 2:
        data = m_zReadings;
        break;
    }

    // Convertir la gráfica a XY y remplazar puntos
    static_cast<QXYSeries*>(series)->replace(data);
}

/**
 * Llamado cuando recibimos cualquier número de bytes del dispositivo
 * serial, genera y separa paquetes de datos para su posterior interpretacion
 */
void Serial::onDataReceived() {

}

/**
 * Termina la conexión con el dispositivo serial actual (si hay alguno conectado)
 */
void Serial::disconnectDevice() {
    // Reset contador de bytes
    m_dataLen = -1;

    // Verificar si el puerto serial es valido
    if (m_port != Q_NULLPTR) {
        // Disconectar señales del puerto serial
        m_port->disconnect(this, SLOT(onDataReceived()));
        m_port->disconnect(this, SLOT(disconnectDevice()));

        // Cerrar y eliminar conexion
        m_port->close();
        m_port->deleteLater();

        // Reset apuntador
        m_port = Q_NULLPTR;

    }

    // Actualizar UI
    emit connectedChanged();
}

/**
 * Obtiene una lista de dispositivos serial del sistema y alerta al
 * resto de la aplicacion si hubo algún cambio
 */
void Serial::refreshSerialDevices() {
    // Buscar dispositivos serial
    QStringList devices;
    foreach(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        if (port.isValid() && !port.description().isEmpty())
            devices.append(port.portName());
    }

    // Si los dispositivos cambiaron, actualizar la UI
    if (devices != m_devices) {
        m_devices = devices;
        emit devicesChanged();
    }

    // Llamar esta funcion dentro de un segundo
    QTimer::singleShot(1000, this, &Serial::refreshSerialDevices);
}

/**
 * Decodifica las lecturas del acelerometro y giroscopio contenidas en
 * el paquete de @a datos.
 *
 * Posteriormente, se realiza una doble integración con el promedio de
 * los datos anteriores para obtener la posición actual en [x,y,z] del
 * acelerometro.
 */
void Serial::readPacket(const QByteArray& data) {

}
