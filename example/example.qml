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
        hostname: "192.168.1.222"
        refreshRate: 1
        anchors.fill: parent
    }
}
