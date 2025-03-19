import QtQml 2.0
import org.kazoe.helios 1.0
import org.kazoe.kaza 1.0

QtObject {
    id: root
    property string name: "helios"

    property Helios helios: Helios {}
    property alias hostname: root.helios.hostname
    property alias refreshRate: root.helios.refreshRate

    property KaZaObject setOperatingMode: KaZaObject {
        name: root.name + ".setOperatingMode"
        onValueChanged: {
            helios.setOperatingMode(value)
        }
    }

    property KaZaObject setForceMode: KaZaObject {
        name: root.name + ".setForceMode"
        onValueChanged: {
            helios.setForceMode(value)
        }
    }

    property KaZaObject setFanStage: KaZaObject {
        name: root.name + ".setFanStage"
        onValueChanged: {
            helios.setFanStage(value)
        }
    }

    property KaZaObject setForceModeSpeed: KaZaObject {
        name: root.name + ".setForceModeSpeed"
        onValueChanged: {
            helios.setForceModeSpeed(value)
        }
    }

    property KaZaObject setForceTime: KaZaObject {
        name: root.name + ".setForceTime"
        onValueChanged: {
            helios.setForceTime(value)
        }
    }

    property KaZaObject forceMode: KaZaObject {
        name: root.name + ".forceMode"
        value: helios.forceMode
    }

    property KaZaObject forceModeSpeed: KaZaObject {
        name: root.name + ".forceModeSpeed"
        value: helios.forceModeSpeed
    }

    property KaZaObject forceModeRemaning: KaZaObject {
        name: root.name + ".forceModeRemaning"
        value: helios.forceModeRemaning
    }

    property KaZaObject operatingMode: KaZaObject {
        name: root.name + ".operatingMode"
        value: helios.operatingMode
    }

    property KaZaObject fanStage: KaZaObject {
        name: root.name + ".fanStage"
        value: helios.fanStage
    }

    property KaZaObject fanPercentage: KaZaObject {
        name: root.name + ".fanPercentage"
        value: helios.fanPercentage
    }

    property KaZaObject rpmSupply: KaZaObject {
        name: root.name + ".rpmSupply"
        value: helios.rpmSupply
    }

    property KaZaObject rpmExtract: KaZaObject {
        name: root.name + ".rpmExtract"
        value: helios.rpmExtract
    }

    property KaZaObject byPass: KaZaObject {
        name: root.name + ".byPass"
        value: helios.byPass
    }

    property KaZaObject humitity: KaZaObject {
        name: root.name + ".humidity"
        value: helios.humidity
        unit: "%"
    }

    property KaZaObject heliosInOutsideTemperature: KaZaObject {
        name: root.name + ".inOutsideTemperature"
        value: helios.inOutsideTemperature
        unit: "°C"
    }

    property KaZaObject heliosOutOutsideTemperature: KaZaObject {
        name: root.name + ".outOutsideTemperature"
        value: helios.outOutsideTemperature
        unit: "°C"
    }

    property KaZaObject heliosInInsideTemperature: KaZaObject {
        name: root.name + ".inInsideTemperature"
        value: helios.inInsideTemperature
        unit: "°C"
    }

    property KaZaObject heliosoutInsideTemperature: KaZaObject {
        name: root.name + ".outInsideTemperature"
        value: helios.outInsideTemperature
        unit: "°C"
    }
}
