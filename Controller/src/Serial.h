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

#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QVector3D>
#include <QStringList>
#include <QAbstractSeries>

QT_CHARTS_USE_NAMESPACE

class QSerialPort;
class Serial : public QObject {
    Q_OBJECT

    Q_PROPERTY(qreal amplitude
               READ amplitude
               WRITE setAmplitude
               NOTIFY amplitudeChanged)
    Q_PROPERTY(bool enabled
               READ enabled
               WRITE setEnabled
               NOTIFY enabledChanged)
    Q_PROPERTY(qreal frequency
               READ frequency
               WRITE setFrequency
               NOTIFY frequencyChanged)
    Q_PROPERTY(qreal minFrequency
               READ minFrequency
               CONSTANT)
    Q_PROPERTY(qreal maxFrequency
               READ maxFrequency
               CONSTANT)
    Q_PROPERTY(qreal minAmplitude
               READ minAmplitude
               CONSTANT)
    Q_PROPERTY(qreal maxAmplitude
               READ maxAmplitude
               CONSTANT)
    Q_PROPERTY(int samples
               READ samples
               CONSTANT)
    Q_PROPERTY(QStringList devices
               READ devices
               NOTIFY devicesChanged)
    Q_PROPERTY(bool connectedToDevice
               READ isConnectedToDevice
               NOTIFY connectedChanged)

signals:
    void packetRead();
    void dataReceived();
    void devicesChanged();
    void enabledChanged();
    void connectedChanged();
    void amplitudeChanged();
    void frequencyChanged();

public:
    Serial();
    ~Serial();

    qreal amplitude() const;
    qreal frequency() const;
    qreal minFrequency() const;
    qreal maxFrequency() const;
    qreal minAmplitude() const;
    qreal maxAmplitude() const;

    int samples() const;
    bool enabled() const;
    QStringList devices() const;
    bool isConnectedToDevice() const;

    Q_INVOKABLE bool startComm(const int device);

public slots:
    void setEnabled(const bool enabled);
    void setFrequency(const qreal frequency);
    void setAmplitude(const qreal amplitude);
    void updateSeries(QAbstractSeries* series, const int signal);

private slots:
    void onDataReceived();
    void disconnectDevice();
    void refreshSerialDevices();
    void readPacket(const QByteArray& data);

private:
    bool m_enabled;
    qreal m_amplitude;
    qreal m_frequency;
    QList<QString> m_devices;
    QVector<QPointF> m_xReadings;
    QVector<QPointF> m_yReadings;
    QVector<QPointF> m_zReadings;
    QList<QVector3D> m_gyroReadings;
    QList<QVector3D> m_acclReadings;

    qint64 m_dataLen;
    QSerialPort* m_port;
    QByteArray m_buffer;
};

#endif
