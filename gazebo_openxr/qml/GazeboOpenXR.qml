import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Rectangle {
    anchors.fill: parent
    color: "#202631"

    implicitWidth: 380
    implicitHeight: 420

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: "Gazebo OpenXR"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        Label {
            text: "Meta Quest | WiVRn Runtime"
            color: "#a0aec0"
            font.pixelSize: 13
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#475569"
        }

        Label {
            text: GazeboOpenXR.status
            color: "#4ade80"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            text: "Check OpenXR Runtime"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.CheckRuntime()
        }

        Button {
            text: "Start VR Session"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.StartVR()
        }

        Button {
            text: "Stop VR Session"
            Layout.fillWidth: true

            onClicked: GazeboOpenXR.StopVR()
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
