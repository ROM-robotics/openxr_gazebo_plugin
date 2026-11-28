import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Rectangle {
    anchors.fill: parent
    color: "#202631"

    implicitWidth: 360
    implicitHeight: 420
    Layout.minimumWidth: 280
    Layout.minimumHeight: 360

    ScrollView {
        id: scrollView
        objectName: "statusScroll"
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        contentHeight: content.implicitHeight + 32
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        ColumnLayout {
            id: content
            x: 16
            y: 16
            width: Math.max(0, scrollView.availableWidth - 32)
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
                implicitHeight: 1
                color: "#475569"
            }

            Label {
                objectName: "runtimeStatus"
                text: GazeboOpenXR.status
                color: "#4ade80"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Button {
                objectName: "runtimeButton"
                text: "Check OpenXR Runtime"
                Layout.fillWidth: true

                onClicked: GazeboOpenXR.CheckRuntime()
            }

            Button {
                objectName: "startButton"
                text: "Start VR Session"
                Layout.fillWidth: true

                onClicked: GazeboOpenXR.StartVR()
            }

            Button {
                objectName: "stopButton"
                text: "Stop VR Session"
                Layout.fillWidth: true

                onClicked: GazeboOpenXR.StopVR()
            }

            Button {
                objectName: "renderDiagnosticButton"
                text: "Test Stereo Rendering"
                Layout.fillWidth: true

                onClicked: GazeboOpenXR.TestStereoRendering()
            }

            Button {
                objectName: "startContinuousButton"
                text: "Start Continuous Test"
                Layout.fillWidth: true

                onClicked: GazeboOpenXR.StartContinuousTest()
            }

            Button {
                objectName: "stopContinuousButton"
                text: "Stop Continuous Test"
                Layout.fillWidth: true

                onClicked: GazeboOpenXR.StopContinuousTest()
            }
        }
    }
}
