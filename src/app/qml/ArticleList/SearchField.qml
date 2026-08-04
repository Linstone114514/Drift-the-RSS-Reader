import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

TextField {
    id: root

    signal searchTextChanged(string text)

    placeholderText: qsTr("Search articles...")
    font.pixelSize: 13
    leftPadding: 30
    rightPadding: clearBtn.visible ? 30 : 8

    // Search icon
    Label {
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        text: "🔍"
        font.pixelSize: 14
    }

    // Clear button
    ToolButton {
        id: clearBtn
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        text: "✕"
        font.pixelSize: 12
        visible: root.text !== ""
        width: 24
        height: 24
        onClicked: {
            root.text = ""
            root.searchTextChanged("")
        }
    }

    onTextChanged: searchTextChanged(root.text)

    background: Rectangle {
        radius: 8
        color: Qt.lighter(Material.background, 1.08)
        border.color: root.activeFocus ? Material.accent : Material.dividerColor
        border.width: root.activeFocus ? 2 : 1
    }
}
