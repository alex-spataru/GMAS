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

import QtQuick 2.0
import QtCharts 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

ChartView {
    //
    // Opciones de visualizacion
    //
    antialiasing: true
    backgroundRoundness: 0
    theme: ChartView.ChartThemeDark

    //
    // Definir limites del eje del tiempo
    //
    ValueAxis {
        id: timeAxis
        min: 0
        max: CSerial.samples
    }

    //
    // Definir limites para ejes de posicion
    //
    ValueAxis {
        id: positionAxis
        max: (CSerial.maxAmplitude + 0.5)
        min: (CSerial.maxAmplitude + 0.5) * -1
    }

    //
    // Señal para el eje X
    //
    SplineSeries {
        id: xSeries
        useOpenGL: true
        axisX: timeAxis
        axisY: positionAxis
        name: qsTr("Posición Relativa en X")
    }

    //
    // Señal para el eje Y
    //
    SplineSeries {
        id: ySeries
        useOpenGL: true
        axisX: timeAxis
        axisY: positionAxis
        name: qsTr("Posición Relativa en Y")
    }

    //
    // Señal para el eje Z
    //
    SplineSeries {
        id: zSeries
        useOpenGL: true
        axisX: timeAxis
        axisY: positionAxis
        name: qsTr("Posición Relativa en Z")
    }

    //
    // Actualizar la gráfica a una freq. de 20 Hz
    //
    Timer {
        id: timer
        repeat: true
        running: true
        interval: (1000 / 20)
        onTriggered:{
            CSerial.updateSeries(xSeries, 0)
            CSerial.updateSeries(ySeries, 1)
            CSerial.updateSeries(zSeries, 2)
        }
    }
}
