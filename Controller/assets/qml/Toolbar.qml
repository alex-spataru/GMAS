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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

Container {
    id: main

    background: Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 1
                color: "#121218"
            }

            GradientStop {
                position: 0
                color: "#2e303a"
            }
        }
    }

    contentItem: ColumnLayout {
        spacing: app.spacing
        anchors.fill: parent
        anchors.margins: app.spacing

        Button {
            icon.width: 44
            icon.height: 44
            text: qsTr("Salir")
            onClicked: Qt.quit()
            Layout.fillWidth: true
            display: Button.TextUnderIcon
            font.pixelSize: fontSizeExtraSmall
            icon.source: "qrc:/icons/exit.svg"
        }

        Button {
            icon.width: 44
            icon.height: 44
            Layout.fillWidth: true
            display: Button.TextUnderIcon
            font.pixelSize: fontSizeExtraSmall
            icon.source: app.visibility === Window.FullScreen ?
                             "qrc:/icons/exit-fullscreen.svg" :
                             "qrc:/icons/fullscreen.svg"
            text: app.visibility !== Window.FullScreen ?
                      qsTr("Pantalla Completa") :
                      qsTr("Pantalla Normal")
            onClicked: {
                if (app.visibility === Window.FullScreen)
                    app.showNormal()

                else
                    app.showFullScreen()
            }
        }

        Item {
            height: 2 * app.spacing
        }

        GlowingLabel {
            font.bold: true
            text: qsTr("Créditos:")
            font.pixelSize: app.fontSizeMedium
        }

        GlowingLabel {
            text: "<ul>" +
                  "<li><b>Electrónica</b>: Emilianio Ríos</li>" +
                  "<li><b>Estructuras:</b> Antonio Martínez</li>" +
                  "<li><b>Documentación:</b> Mitzi Pérez</li>" +
                  "<li><b>Programación:</b> Alex Spataru</li>" +
                  "</ul>"
        }

        Item {
            height: 50
        }

        GlowingLabel {
            font.pixelSize: app.fontSizeMedium
            text: qsTr("<b>Grupo:</b> IECSA 05-A")
        }

        GlowingLabel {
            font.pixelSize: app.fontSizeMedium
            text: qsTr("<b>Curso:</b> Física para Ingeniería")
        }

        GlowingLabel {
            font.pixelSize: app.fontSizeMedium
            text: qsTr("<b>Profesor:</b> Javier Uribe Luna")
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            flat: true
            enabled: false
            Layout.fillWidth: true
            icon.width: main.implicitWidth - 6 * app.spacing
            icon.height: 43/103 * icon.width
            icon.source: "qrc:/icons/unaq.svg"
        }
    }
}
