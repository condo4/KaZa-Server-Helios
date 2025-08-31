import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import test.helios 1.0

Item {
    id: root
    property alias hostname: helios.hostname
    property alias refreshRate: helios.refreshRate
    property bool showModeForce: false

    Helios {
        id: helios
    }

    component VMCProperty : Rectangle {
        property string label: ""
        property string value: ""

        Layout.fillWidth: true
        height: 40
        color: "#e0e0e0"
        radius: 5

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            Text {
                text: label
                font.pixelSize: 16
                Layout.fillWidth: true
            }

            Text {
                text: value
                font.pixelSize: 16
                font.bold: true
            }
        }
    }

    Rectangle {
        id: rec
        anchors.fill: root
        border.color: helios.connected ? "green" : "red"
        border.width: 1
        color: "#f0f0f0"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10

            Text {
                text: "VMC Helios"
                font.pixelSize: 24
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Button {
                    text: "Manuel"
                    visible: (helios.forceMode === 0)
                    checked: (helios.operatingMode === 1)
                    onClicked: {
                        helios.setOperatingMode(1)
                    }
                }

                Button {
                    text: "Auto"
                    visible: (helios.forceMode === 0)
                    checked: (helios.operatingMode === 0)
                    onClicked: {
                        helios.setOperatingMode(0)
                    }
                }

                Button {
                    text: (helios.forceMode === 0) ? "Forcer" : ("Annuler mode forcé (restant: " + helios.forceModeRemaning + " min)")
                    onClicked: {
                        if(helios.forceMode === 1)
                        {
                            helios.setForceMode(0)
                        }
                        else
                        {
                            showModeForce = true
                        }
                    }
                }
            }

            RowLayout {

                Button {
                    text: "1"
                    enabled: ((helios.forceMode === 0) && (helios.operatingMode === 1))
                    checked: (helios.forceMode === 0) ? (helios.fanStage === 1) : (helios.forceModeSpeed === 1)
                    onClicked: {
                        helios.setFanStage(1)
                    }
                }
                Button {
                    text: "2"
                    enabled: ((helios.forceMode === 0) && (helios.operatingMode === 1))
                    checked: (helios.forceMode === 0) ? (helios.fanStage === 2) : (helios.forceModeSpeed === 2)
                    onClicked: {
                        helios.setFanStage(2)
                    }
                }
                Button {
                    text: "3"
                    enabled: ((helios.forceMode === 0) && (helios.operatingMode === 1))
                    checked: (helios.forceMode === 0) ? (helios.fanStage === 3) : (helios.forceModeSpeed === 3)
                    onClicked: {
                        helios.setFanStage(3)
                    }
                }
                Button {
                    text: "4"
                    enabled: ((helios.forceMode === 0) && (helios.operatingMode === 1))
                    checked: (helios.forceMode === 0) ? (helios.fanStage === 4) : (helios.forceModeSpeed === 4)
                    onClicked: {
                        helios.setFanStage(4)
                    }
                }
            }

            RowLayout {
                visible: (showModeForce && (helios.forceMode === 0))

                Button {
                    text: "1"
                    width: 150
                    checked: (helios.forceModeSpeed === 1)
                    onClicked: {
                        helios.setForceModeSpeed(1)
                    }
                }
                Button {
                    text: "2"
                    width: height
                    checked: (helios.forceModeSpeed === 2)
                    onClicked: {
                        helios.setForceModeSpeed(2)
                    }
                }
                Button {
                    text: "3"
                    width: height
                    checked: (helios.forceModeSpeed === 3)
                    onClicked: {
                        helios.setForceModeSpeed(3)
                    }
                }
                Button {
                    text: "4"
                    width: height
                    checked: (helios.forceModeSpeed === 4)
                    onClicked: {
                        helios.setForceModeSpeed(4)
                    }
                }

            }

            RowLayout {
                visible: (showModeForce && (helios.forceMode === 0))

                Button {
                    text: "15 min"
                    checked: (helios.forceModeTime === 15)
                    onClicked: {
                        helios.setForceTime(15)
                    }
                }
                Button {
                    text: "30 min"
                    checked: (helios.forceModeTime === 30)
                    onClicked: {
                        helios.setForceTime(30)
                    }
                }
                Button {
                    text: "1h"
                    checked: (helios.forceModeTime === 60)
                    onClicked: {
                        helios.setForceTime(60)
                    }
                }
                Button {
                    text: "2h"
                    checked: (helios.forceModeTime === 120)
                    onClicked: {
                        helios.setForceTime(120)
                    }
                }
                Button {
                    text: "3h"
                    checked: (helios.forceModeTime === 180)
                    onClicked: {
                        helios.setForceTime(180)
                    }
                }
            }

            RowLayout {
                visible: (showModeForce && (helios.forceMode === 0))

                Button {
                    text: "Annuler"
                    onClicked: {
                        showModeForce = false
                    }
                }

                Button {
                    text: "Confirmer"
                    onClicked: {
                        helios.setForceMode(1)
                        showModeForce = false
                    }
                }
            }


            GridLayout {
                columns: 2
                Layout.fillWidth: true
                columnSpacing: 20
                rowSpacing: 10

                VMCProperty { label: "RPM alimentation"; value: helios.rpmSupply }
                VMCProperty { label: "RPM extraction"; value: helios.rpmExtract }
                VMCProperty { label: "Bypass"; value: {
                    if(helios.byPass === 0) return "Fermé"
                    return "Ouvert"
                    }
                }
                VMCProperty { label: "Humidité"; value: helios.humidity.toFixed(0) + "%" }
                VMCProperty { label: "T° air extérieur"; value: helios.inOutsideTemperature.toFixed(1) + "°C" }
                VMCProperty { label: "T° air rejeté"; value: helios.outOutsideTemperature.toFixed(1) + "°C" }
                VMCProperty { label: "T° air soufflé"; value: helios.inInsideTemperature.toFixed(1) + "°C" }
                VMCProperty { label: "T° air repris"; value: helios.outInsideTemperature.toFixed(1) + "°C" }
            }
        }
    }
}
