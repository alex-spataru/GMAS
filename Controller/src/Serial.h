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

    Q_PROPERTY(qreal amplitud
               READ amplitud
               WRITE cambiarAmplitud
               NOTIFY amplitudCambiada)
    Q_PROPERTY(bool gmasHabilitado
               READ gmasHabilitado
               WRITE habilitarGmas
               NOTIFY gmasEstadoCambiado)
    Q_PROPERTY(qreal frecuencia
               READ frecuencia
               WRITE cambiarFrecuencia
               NOTIFY frecuenciaCambiada)
    Q_PROPERTY(qreal frecuenciaMin
               READ frecuenciaMin
               CONSTANT)
    Q_PROPERTY(qreal frecuenciaMax
               READ frecuenciaMax
               CONSTANT)
    Q_PROPERTY(qreal amplitudMin
               READ amplitudMin
               CONSTANT)
    Q_PROPERTY(qreal amplitudMax
               READ amplitudMax
               CONSTANT)
    Q_PROPERTY(int numLecturas
               READ numLecturas
               CONSTANT)
    Q_PROPERTY(QStringList dispositivosSerial
               READ dispositivosSerial
               NOTIFY dispositivosCambiados)
    Q_PROPERTY(bool conexionConDispositivo
               READ conexionConDispositivo
               NOTIFY conexionCambiada)

signals:
    void datosRecibidos();
    void conexionCambiada();
    void amplitudCambiada();
    void posicionCalculada();
    void frecuenciaCambiada();
    void gmasEstadoCambiado();
    void dispositivosCambiados();

public:
    Serial();
    ~Serial();

    qreal amplitud() const;
    qreal frecuencia() const;
    qreal frecuenciaMin() const;
    qreal frecuenciaMax() const;
    qreal amplitudMin() const;
    qreal amplitudMax() const;

    int numLecturas() const;
    bool gmasHabilitado() const;
    bool conexionConDispositivo() const;
    QStringList dispositivosSerial() const;
    Q_INVOKABLE bool conectarADispositivo(const int device);

public slots:
    void habilitarGmas(const bool gmasHabilitado);
    void cambiarFrecuencia(const qreal frecuencia);
    void cambiarAmplitud(const qreal amplitud);
    void actualizarGrafica(QAbstractSeries* series, const int signal);

private slots:
    void onDatosRecibidos();
    void actualizarPosicion();
    void desconectarDispositivo();
    void actualizarDispositivosSerial();
    void interpretarPaquete(const QByteArray& datos);

private:
    bool m_gmasHabilitado;
    qreal m_amplitud;
    qreal m_frecuencia;
    QList<QString> m_dispositivosSerial;
    QVector<QPointF> m_lecturasX;
    QVector<QPointF> m_lecturasY;
    QVector<QPointF> m_lecturasZ;
    QList<QVector3D> m_lecturasGyro;
    QList<QVector3D> m_lecturasAccl;

    qint64 m_lenDatos;
    QByteArray m_buffer;
    QSerialPort* m_puerto;
};

#endif
