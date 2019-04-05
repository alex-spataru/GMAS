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
    property alias xAxisEnabled: xSeries.visible
    property alias yAxisEnabled: ySeries.visible
    property alias zAxisEnabled: zSeries.visible
    property alias pAxisEnabled: pSeries.visible

    //
    // Opciones de visualizacion
    //
    antialiasing: false
    backgroundRoundness: 0
    theme: ChartView.ChartThemeDark

    //
    // Definir limites del eje del tiempo
    //
    ValueAxis {
        id: timeAxis
        min: 1
        max: 1
    }

    //
    // Definir limites para ejes de posicion
    //
    ValueAxis {
        id: positionAxis
        max: 60
        min: -40
    }

    //
    // Señal para el eje promedio
    //
    LineSeries {
        id: pSeries
        useOpenGL: true
        axisX: timeAxis
        axisY: positionAxis
        name: qsTr("Aceleración Promedio")
    }

    //
    // Señal para el eje Z
    //
    LineSeries {
        id: zSeries
        visible: false
        useOpenGL: true
        axisX: timeAxis
        axisY: positionAxis
        name: qsTr("Aceleración en Z")
    }

    //
    // Señal para el eje X
    //
    LineSeries {
        id: xSeries
        visible: false
        useOpenGL: true
        axisX: timeAxis
        axisY: positionAxis
        name: qsTr("Aceleración en X")
    }

    //
    // Señal para el eje Y
    //
    LineSeries {
        id: ySeries
        visible: false
        useOpenGL: true
        axisX: timeAxis
        axisY: positionAxis
        name: qsTr("Aceleración en Y")
    }

    //
    // Actualizar la gráfica cuando la ultima posicion
    // del GMAS es calculada
    //
    Timer {
        interval: 1000 / 60
        repeat: true
        running: true
        onTriggered: {
            timeAxis.max = Math.max(CSerial.numLecturas, CSerial.escala)
            timeAxis.min = Math.max(0, CSerial.numLecturas - CSerial.escala)

            CSerial.actualizarGrafica(xSeries, 0)
            CSerial.actualizarGrafica(ySeries, 1)
            CSerial.actualizarGrafica(zSeries, 2)
            CSerial.actualizarGrafica(pSeries, 3)
        }
    }
}
