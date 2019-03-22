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
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

Item {
    id: main

    Rectangle {
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

    ColumnLayout {
        anchors.fill: parent
        spacing: app.spacing
        anchors.margins: app.spacing

        Item {
            height: app.spacing
        }

        //
        // Controles de amplitud
        //
        GlowingLabel {
            color: "white"
            text: qsTr("Amplitud")
            font.pixelSize: fontSizeMedium
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
        } Dial {
            to: CSerial.amplitudMax
            from: CSerial.amplitudMin
            value: CSerial.amplitudMax / 2
            implicitHeight: implicitWidth
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: main.implicitWidth
            onValueChanged: CSerial.amplitud = value

            Label {
                color: "white"
                anchors.centerIn: parent
                text: parent.value.toFixed(2) + " cm"
                font.pixelSize: Qt.application.font.pixelSize * 2
            }
        }

        //
        // Espaciador
        //
        Item {
            height: app.spacing
        }

        //
        // Controles de frequencia
        //
        GlowingLabel {
            color: "white"
            text: qsTr("Frecuencia")
            font.pixelSize: fontSizeMedium
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
        } Dial {
            to: CSerial.frecuenciaMax
            from: CSerial.frecuenciaMin
            implicitHeight: implicitWidth
            value: CSerial.frecuenciaMax / 10
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: main.implicitWidth
            onValueChanged: CSerial.frecuencia = value.toFixed(0)

            Label {
                color: "white"
                anchors.centerIn: parent
                text: parent.value.toFixed(0) + " Hz"
                font.pixelSize: Qt.application.font.pixelSize * 2
            }
        }

        //
        // Espaciador
        //
        Item {
            Layout.fillHeight: true
        }

        //
        // Seleccionador de puertos
        //
        GlowingLabel {
            color: "white"
            text: qsTr("Puertos Serial")
            font.pixelSize: fontSizeMedium
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
        } Item {
            height: app.spacing
        } Frame {
            Layout.fillWidth: true
            Layout.fillHeight: false

            ColumnLayout {
                spacing: app.spacing
                anchors.fill: parent

                Label {
                    font.bold: true
                    font.pixelSize: 14
                    Layout.maximumWidth: parent.width
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    visible: CSerial.dispositivosSerial.length === 0
                    text: qsTr("No hay ningún dispositivo conectado")
                }

                Repeater {
                    model: CSerial.dispositivosSerial
                    delegate: RadioButton {
                        text: modelData
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        width: parent.width - 2 * app.spacing
                        onCheckedChanged: {
                            if (checked) {
                                if (!CSerial.conectarADispositivo(index))
                                    checked = false
                            }
                        }
                    }
                }
            }
        }

        //
        // Espaciador
        //
        Item {
            Layout.fillHeight: true
        }

        //
        // Boton de habilitado
        //
        Button {
            icon.width: 36
            icon.height: 36
            checkable: true
            font.bold: true
            Layout.fillWidth: true
            text: qsTr("Habilitar")
            display: Button.TextBesideIcon
            Layout.alignment: Qt.AlignHCenter
            icon.source: "qrc:/icons/enable.svg"
            enabled: CSerial.conexionConDispositivo
            onCheckedChanged: CSerial.gmasHabilitado = checked
            opacity: enabled ? 1 : 0.2
            visible: CSerial.dispositivosSerial.length > 0
        }
    }
}
