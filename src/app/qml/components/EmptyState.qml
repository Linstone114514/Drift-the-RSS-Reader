import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

ColumnLayout {
    id: root
    property string icon: "📰"
    property string message: ""

    spacing: 12
    visible: true

    Label {
        text: root.icon
        font.pixelSize: 48
        Layout.alignment: Qt.AlignHCenter
    }

    Label {
        text: root.message
        font.pixelSize: 14
        color: Material.secondaryTextColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        Layout.maximumWidth: 280
        Layout.alignment: Qt.AlignHCenter
    }
}
