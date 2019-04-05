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

#include <QFile>
#include <QObject>
#include <QVector3D>
#include <QStringList>
#include <QElapsedTimer>
#include <QAbstractSeries>

QT_CHARTS_USE_NAMESPACE

class QSerialPort;
class Serial : public QObject {
    Q_OBJECT

    Q_PROPERTY(qreal velocidad
               READ velocidad
               WRITE cambiarVelocidad
               NOTIFY velocidadCambiada)
    Q_PROPERTY(bool gmasHabilitado
               READ gmasHabilitado
               WRITE habilitarGmas
               NOTIFY gmasEstadoCambiado)
    Q_PROPERTY(qreal velocidad
               READ velocidad
               WRITE cambiarVelocidad
               NOTIFY velocidadCambiada)
    Q_PROPERTY(int escalaMin
               READ escalaMin
               CONSTANT)
    Q_PROPERTY(int escalaMax
               READ escalaMax
               CONSTANT)
    Q_PROPERTY(qreal velocidadMin
               READ velocidadMin
               CONSTANT)
    Q_PROPERTY(qreal velocidadMax
               READ velocidadMax
               CONSTANT)
    Q_PROPERTY(int numLecturas
               READ numLecturas
               CONSTANT)
    Q_PROPERTY(int escala
               READ escala
               WRITE cambiarEscala
               NOTIFY escalaCambiada)
    Q_PROPERTY(QStringList dispositivosSerial
               READ dispositivosSerial
               NOTIFY dispositivosCambiados)
    Q_PROPERTY(bool conexionConDispositivo
               READ conexionConDispositivo
               NOTIFY conexionCambiada)

signals:
    void escalaCambiada();
    void datosRecibidos();
    void conexionCambiada();
    void velocidadCambiada();
    void posicionCalculada();
    void gmasEstadoCambiado();
    void dispositivosCambiados();

public:
    Serial();
    ~Serial();

    int escala() const;
    qreal velocidad() const;

    int escalaMin() const;
    int escalaMax() const;
    qreal velocidadMin() const;
    qreal velocidadMax() const;

    quint64 numLecturas() const;
    bool gmasHabilitado() const;
    bool conexionConDispositivo() const;
    QStringList dispositivosSerial() const;
    Q_INVOKABLE bool conectarADispositivo(const int device);

public slots:
    void cambiarEscala (const int escala);
    void habilitarGmas(const bool gmasHabilitado);
    void cambiarVelocidad(const qreal velocidad);
    void actualizarGrafica(QAbstractSeries* series, const int signal);

private slots:
    void mandarDatos();
    void onDatosRecibidos();
    void actualizarPosicion();
    void desconectarDispositivo();
    void actualizarDispositivosSerial();
    void interpretarPaquete(const QByteArray& datos);

private:
    int m_escala;
    qreal m_velocidad;
    quint64 m_numLecturas;
    bool m_gmasHabilitado;

    QFile m_archivoLecturas;

    QVector<QPointF> m_lecturasX;
    QVector<QPointF> m_lecturasY;
    QVector<QPointF> m_lecturasZ;
    QVector<QPointF> m_lecturasP;

    QVector<QVector3D> m_lecturasGyro;
    QVector<QVector3D> m_lecturasAccl;

    QByteArray m_buffer;
    QSerialPort* m_puerto;
    QList<QString> m_dispositivosSerial;
};

#endif
