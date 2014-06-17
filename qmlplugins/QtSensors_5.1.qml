import QtSensors 5.0

// This file describes the plugin-supplied types contained in the library.
// It is used for QML tooling purposes only.
//
// This file was auto-generated with the command 'qmlplugindump -notrelocatable QtSensors 5.1'.

Module {
    Component {
        name: "QmlAccelerometer"
        prototype: "QmlSensor"
        exports: [
            "QtSensors/Accelerometer 5.0",
            "QtSensors/Accelerometer 5.1"
        ]
        exportMetaObjectRevisions: [0, 1]
        Enum {
            name: "AccelerationMode"
            values: {
                "Combined": 0,
                "Gravity": 1,
                "User": 2
            }
        }
        Property { name: "accelerationMode"; revision: 1; type: "AccelerationMode" }
        Signal {
            name: "accelerationModeChanged"
            revision: 1
            Parameter { name: "accelerationMode"; type: "AccelerationMode" }
        }
    }
    Component {
        name: "QmlAccelerometerReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/AccelerometerReading 5.0",
            "QtSensors/AccelerometerReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "x"; type: "double"; isReadonly: true }
        Property { name: "y"; type: "double"; isReadonly: true }
        Property { name: "z"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlAltimeter"
        prototype: "QmlSensor"
        exports: ["QtSensors/Altimeter 5.1"]
        exportMetaObjectRevisions: [0]
    }
    Component {
        name: "QmlAltimeterReading"
        prototype: "QmlSensorReading"
        exports: ["QtSensors/AltimeterReading 5.1"]
        exportMetaObjectRevisions: [0]
        Property { name: "altitude"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlAmbientLightSensor"
        prototype: "QmlSensor"
        exports: [
            "QtSensors/AmbientLightSensor 5.0",
            "QtSensors/AmbientLightSensor 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
    }
    Component {
        name: "QmlAmbientLightSensorReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/AmbientLightReading 5.0",
            "QtSensors/AmbientLightReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "lightLevel"; type: "QAmbientLightReading::LightLevel"; isReadonly: true }
    }
    Component {
        name: "QmlAmbientTemperatureReading"
        prototype: "QmlSensorReading"
        exports: ["QtSensors/AmbientTemperatureReading 5.1"]
        exportMetaObjectRevisions: [0]
        Property { name: "temperature"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlAmbientTemperatureSensor"
        prototype: "QmlSensor"
        exports: ["QtSensors/AmbientTemperatureSensor 5.1"]
        exportMetaObjectRevisions: [0]
    }
    Component {
        name: "QmlCompass"
        prototype: "QmlSensor"
        exports: ["QtSensors/Compass 5.0", "QtSensors/Compass 5.1"]
        exportMetaObjectRevisions: [0, 0]
    }
    Component {
        name: "QmlCompassReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/CompassReading 5.0",
            "QtSensors/CompassReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "azimuth"; type: "double"; isReadonly: true }
        Property { name: "calibrationLevel"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlGyroscope"
        prototype: "QmlSensor"
        exports: ["QtSensors/Gyroscope 5.0", "QtSensors/Gyroscope 5.1"]
        exportMetaObjectRevisions: [0, 0]
    }
    Component {
        name: "QmlGyroscopeReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/GyroscopeReading 5.0",
            "QtSensors/GyroscopeReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "x"; type: "double"; isReadonly: true }
        Property { name: "y"; type: "double"; isReadonly: true }
        Property { name: "z"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlHolsterReading"
        prototype: "QmlSensorReading"
        exports: ["QtSensors/HolsterReading 5.1"]
        exportMetaObjectRevisions: [0]
        Property { name: "holstered"; type: "bool"; isReadonly: true }
    }
    Component {
        name: "QmlHolsterSensor"
        prototype: "QmlSensor"
        exports: ["QtSensors/HolsterSensor 5.1"]
        exportMetaObjectRevisions: [0]
    }
    Component {
        name: "QmlIRProximitySensor"
        prototype: "QmlSensor"
        exports: [
            "QtSensors/IRProximitySensor 5.0",
            "QtSensors/IRProximitySensor 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
    }
    Component {
        name: "QmlIRProximitySensorReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/IRProximityReading 5.0",
            "QtSensors/IRProximityReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "reflectance"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlLightSensor"
        prototype: "QmlSensor"
        exports: ["QtSensors/LightSensor 5.0", "QtSensors/LightSensor 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "fieldOfView"; type: "double"; isReadonly: true }
        Signal {
            name: "fieldOfViewChanged"
            Parameter { name: "fieldOfView"; type: "double" }
        }
    }
    Component {
        name: "QmlLightSensorReading"
        prototype: "QmlSensorReading"
        exports: ["QtSensors/LightReading 5.0", "QtSensors/LightReading 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "illuminance"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlMagnetometer"
        prototype: "QmlSensor"
        exports: ["QtSensors/Magnetometer 5.0", "QtSensors/Magnetometer 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "returnGeoValues"; type: "bool" }
        Signal {
            name: "returnGeoValuesChanged"
            Parameter { name: "returnGeoValues"; type: "bool" }
        }
    }
    Component {
        name: "QmlMagnetometerReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/MagnetometerReading 5.0",
            "QtSensors/MagnetometerReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "x"; type: "double"; isReadonly: true }
        Property { name: "y"; type: "double"; isReadonly: true }
        Property { name: "z"; type: "double"; isReadonly: true }
        Property { name: "calibrationLevel"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlOrientationSensor"
        prototype: "QmlSensor"
        exports: [
            "QtSensors/OrientationSensor 5.0",
            "QtSensors/OrientationSensor 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
    }
    Component {
        name: "QmlOrientationSensorReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/OrientationReading 5.0",
            "QtSensors/OrientationReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "orientation"; type: "QOrientationReading::Orientation"; isReadonly: true }
    }
    Component {
        name: "QmlPressureReading"
        prototype: "QmlSensorReading"
        exports: ["QtSensors/PressureReading 5.1"]
        exportMetaObjectRevisions: [0]
        Property { name: "pressure"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlPressureSensor"
        prototype: "QmlSensor"
        exports: ["QtSensors/PressureSensor 5.1"]
        exportMetaObjectRevisions: [0]
    }
    Component {
        name: "QmlProximitySensor"
        prototype: "QmlSensor"
        exports: [
            "QtSensors/ProximitySensor 5.0",
            "QtSensors/ProximitySensor 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
    }
    Component {
        name: "QmlProximitySensorReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/ProximityReading 5.0",
            "QtSensors/ProximityReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "near"; type: "bool"; isReadonly: true }
    }
    Component {
        name: "QmlRotationSensor"
        prototype: "QmlSensor"
        exports: [
            "QtSensors/RotationSensor 5.0",
            "QtSensors/RotationSensor 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "hasZ"; type: "bool"; isReadonly: true }
        Signal {
            name: "hasZChanged"
            Parameter { name: "hasZ"; type: "bool" }
        }
    }
    Component {
        name: "QmlRotationSensorReading"
        prototype: "QmlSensorReading"
        exports: [
            "QtSensors/RotationReading 5.0",
            "QtSensors/RotationReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "x"; type: "double"; isReadonly: true }
        Property { name: "y"; type: "double"; isReadonly: true }
        Property { name: "z"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlSensor"
        prototype: "QObject"
        exports: ["QtSensors/Sensor 5.0", "QtSensors/Sensor 5.1"]
        exportMetaObjectRevisions: [0, 1]
        Enum {
            name: "AxesOrientationMode"
            values: {
                "FixedOrientation": 0,
                "AutomaticOrientation": 1,
                "UserOrientation": 2
            }
        }
        Property { name: "identifier"; type: "string" }
        Property { name: "type"; type: "string"; isReadonly: true }
        Property { name: "connectedToBackend"; type: "bool"; isReadonly: true }
        Property { name: "availableDataRates"; type: "QmlSensorRange"; isList: true; isReadonly: true }
        Property { name: "dataRate"; type: "int" }
        Property { name: "reading"; type: "QmlSensorReading"; isReadonly: true; isPointer: true }
        Property { name: "busy"; type: "bool"; isReadonly: true }
        Property { name: "active"; type: "bool" }
        Property { name: "outputRanges"; type: "QmlSensorOutputRange"; isList: true; isReadonly: true }
        Property { name: "outputRange"; type: "int" }
        Property { name: "description"; type: "string"; isReadonly: true }
        Property { name: "error"; type: "int"; isReadonly: true }
        Property { name: "alwaysOn"; type: "bool" }
        Property { name: "skipDuplicates"; revision: 1; type: "bool" }
        Property { name: "axesOrientationMode"; revision: 1; type: "AxesOrientationMode" }
        Property { name: "currentOrientation"; revision: 1; type: "int"; isReadonly: true }
        Property { name: "userOrientation"; revision: 1; type: "int" }
        Property { name: "maxBufferSize"; revision: 1; type: "int"; isReadonly: true }
        Property { name: "efficientBufferSize"; revision: 1; type: "int"; isReadonly: true }
        Property { name: "bufferSize"; revision: 1; type: "int" }
        Signal {
            name: "skipDuplicatesChanged"
            revision: 1
            Parameter { name: "skipDuplicates"; type: "bool" }
        }
        Signal {
            name: "axesOrientationModeChanged"
            revision: 1
            Parameter { name: "axesOrientationMode"; type: "AxesOrientationMode" }
        }
        Signal {
            name: "currentOrientationChanged"
            revision: 1
            Parameter { name: "currentOrientation"; type: "int" }
        }
        Signal {
            name: "userOrientationChanged"
            revision: 1
            Parameter { name: "userOrientation"; type: "int" }
        }
        Signal {
            name: "maxBufferSizeChanged"
            revision: 1
            Parameter { name: "maxBufferSize"; type: "int" }
        }
        Signal {
            name: "efficientBufferSizeChanged"
            revision: 1
            Parameter { name: "efficientBufferSize"; type: "int" }
        }
        Signal {
            name: "bufferSizeChanged"
            revision: 1
            Parameter { name: "bufferSize"; type: "int" }
        }
        Method { name: "start"; type: "bool" }
        Method { name: "stop" }
    }
    Component {
        name: "QmlSensorGesture"
        prototype: "QObject"
        exports: [
            "QtSensors/SensorGesture 5.0",
            "QtSensors/SensorGesture 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "availableGestures"; type: "QStringList"; isReadonly: true }
        Property { name: "gestures"; type: "QStringList" }
        Property { name: "validGestures"; type: "QStringList"; isReadonly: true }
        Property { name: "invalidGestures"; type: "QStringList"; isReadonly: true }
        Property { name: "enabled"; type: "bool" }
        Signal {
            name: "detected"
            Parameter { name: "gesture"; type: "string" }
        }
    }
    Component {
        name: "QmlSensorGlobal"
        prototype: "QObject"
        exports: ["QtSensors/QmlSensors 5.0", "QtSensors/QmlSensors 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Signal { name: "availableSensorsChanged" }
        Method { name: "sensorTypes"; type: "QStringList" }
        Method {
            name: "sensorsForType"
            type: "QStringList"
            Parameter { name: "type"; type: "string" }
        }
        Method {
            name: "defaultSensorForType"
            type: "string"
            Parameter { name: "type"; type: "string" }
        }
    }
    Component {
        name: "QmlSensorOutputRange"
        prototype: "QObject"
        exports: ["QtSensors/OutputRange 5.0", "QtSensors/OutputRange 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "minimum"; type: "double"; isReadonly: true }
        Property { name: "maximum"; type: "double"; isReadonly: true }
        Property { name: "accuracy"; type: "double"; isReadonly: true }
    }
    Component {
        name: "QmlSensorRange"
        prototype: "QObject"
        exports: ["QtSensors/Range 5.0", "QtSensors/Range 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "minimum"; type: "int"; isReadonly: true }
        Property { name: "maximum"; type: "int"; isReadonly: true }
    }
    Component {
        name: "QmlSensorReading"
        prototype: "QObject"
        exports: [
            "QtSensors/SensorReading 5.0",
            "QtSensors/SensorReading 5.1"
        ]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "timestamp"; type: "qulonglong"; isReadonly: true }
    }
    Component {
        name: "QmlTapSensor"
        prototype: "QmlSensor"
        exports: ["QtSensors/TapSensor 5.0", "QtSensors/TapSensor 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "returnDoubleTapEvents"; type: "bool" }
        Signal {
            name: "returnDoubleTapEventsChanged"
            Parameter { name: "returnDoubleTapEvents"; type: "bool" }
        }
    }
    Component {
        name: "QmlTapSensorReading"
        prototype: "QmlSensorReading"
        exports: ["QtSensors/TapReading 5.0", "QtSensors/TapReading 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "tapDirection"; type: "QTapReading::TapDirection"; isReadonly: true }
        Property { name: "doubleTap"; type: "bool"; isReadonly: true }
        Signal { name: "isDoubleTapChanged" }
    }
    Component {
        name: "QmlTiltSensor"
        prototype: "QmlSensor"
        exports: ["QtSensors/TiltSensor 5.0", "QtSensors/TiltSensor 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Method { name: "calibrate" }
    }
    Component {
        name: "QmlTiltSensorReading"
        prototype: "QmlSensorReading"
        exports: ["QtSensors/TiltReading 5.0", "QtSensors/TiltReading 5.1"]
        exportMetaObjectRevisions: [0, 0]
        Property { name: "yRotation"; type: "double"; isReadonly: true }
        Property { name: "xRotation"; type: "double"; isReadonly: true }
    }
}
