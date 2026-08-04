import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

ToolButton {
    id: root
    property bool starred: false

    signal starToggled()

    text: starred ? "⭐" : "☆"
    font.pixelSize: 16

    ToolTip.text: starred ? qsTr("Remove star") : qsTr("Add star")
    ToolTip.visible: hovered

    onClicked: starToggled()
}
