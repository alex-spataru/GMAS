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

import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

ApplicationWindow {
    id: app

    //
    // Definir geometria de la ventana
    //
    minimumWidth: 1136
    minimumHeight: 640
    width: minimumWidth
    height: minimumHeight

    //
    // Centrar ventana al principio
    //
    Component.onCompleted: {
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2
        visible = true
    }

    //
    // Definir espaciado default entre controles
    //
    readonly property int spacing: 8

    //
    // Definir colores principales de la applicacion
    //
    readonly property color colorGlow: "#1d6d64"
    readonly property color colorMain: "#6affcd"
    readonly property color colorBright: "#ffffff"
    readonly property color colorDarkGrey: "#333333"
    readonly property color colorLightGrey: "#888888"

    //
    // Definir tamaño de fuentes
    //
    readonly property int fontSizeLarge: Qt.application.font.pixelSize * 2
    readonly property int fontSizeMedium: Qt.application.font.pixelSize * 1.5
    readonly property int fontSizeExtraLarge: Qt.application.font.pixelSize * 5
    readonly property int fontSizeExtraSmall: Qt.application.font.pixelSize * 0.8

    //
    // Salir de la app al precionar Ctrl+Q
    //
    Shortcut {
        sequence: "Ctrl+Q"
        onActivated: Qt.quit()
    }

    //
    // Definir fondo
    //
    background: Rectangle {
        color: "#12121a"
    }

    //
    // Ordenar controles horizontalmente
    //
    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: app.spacing * 2
        spacing: app.spacing * 2

        //
        // Controles de frequencia, amplitud, puerto y habilitado
        //
        Frame {
            Layout.fillHeight: true
            Layout.fillWidth: false

            Controls {
                implicitWidth: 144
                anchors.fill: parent
                anchors.margins: -9
            }
        }

        //
        // Grafica
        //
        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Graph {
                anchors.fill: parent
                anchors.margins: -18
            }
        }

        //
        // Controles de aplicacion (salido y pantalla completa)
        //
        Frame {
            Layout.fillHeight: true
            Layout.fillWidth: false

            Toolbar {
                anchors.fill: parent
                anchors.margins: -9
            }
        }
    }
}
