import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Rectangle {
    id: root
    property int count: 0

    width: Math.max(20, countLabel.implicitWidth + 8)
    height: 18
    radius: 9
    color: Material.accent
    visible: count > 0

    Label {
        id: countLabel
        anchors.centerIn: parent
        text: root.count > 999 ? "999+" : root.count.toString()
        font.pixelSize: 10
        font.weight: Font.Bold
        color: "white"
    }
}
