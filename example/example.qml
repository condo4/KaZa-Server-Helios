import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import org.kazoe.helios 1.0

Window {
    width: 640
    height: 400
    visible: true
    title: qsTr("Helios")

    HeliosControls {
        id: controls
        hostname: "helios"
        refreshRate: 5
        anchors.fill: parent
    }
}
