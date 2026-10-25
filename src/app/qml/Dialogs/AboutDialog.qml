import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

WinDialog {
    id: root
    dialogTitle: qsTr("About Drift")
    width: 380

    ColumnLayout {
        spacing: 10
        Layout.fillWidth: true

        Label {
            text: "Drift"
            font.pixelSize: 24
            font.weight: Font.Bold
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: qsTr("A modern RSS reader built with Qt 6")
            color: Material.secondaryTextColor
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        Label {
            text: qsTr("Version 0.1.0")
            font.pixelSize: 12
            color: Material.secondaryTextColor
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Material.dividerColor
        }

        Label {
            text: qsTr("Built with Qt %1\nC++20 · QML · Material Design")
                .arg("6.11")
            font.pixelSize: 11
            color: Material.secondaryTextColor
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
    }

    buttons: [
        WinDialogButton {
            text: qsTr("OK")
            primary: true
            onClicked: root.close()
        }
    ]
}
