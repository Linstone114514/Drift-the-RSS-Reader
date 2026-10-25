import QtQuick
import QtQuick.Controls

// Windows style push button for WinDialog button rows.
// `primary: true` renders the accent-filled default button.
Button {
    id: control

    property bool primary: false

    readonly property bool dark: control.Material.theme === Material.Dark

    hoverEnabled: true
    font.pixelSize: 13
    implicitHeight: 32
    implicitWidth: 100

    contentItem: Text {
        text: control.text
        font: control.font
        color: control.primary ? "#FFFFFF" : control.palette.buttonText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: 4
        color: control.primary
             ? (control.down ? Qt.darker(Material.accentColor, 1.15)
                : control.hovered ? Qt.lighter(Material.accentColor, 1.08)
                : Material.accentColor)
             : (control.down ? (control.dark ? "#262626" : "#E0E0E0")
                : control.hovered ? (control.dark ? "#4A4A4A" : "#EAEAEA")
                : (control.dark ? "#3A3A3A" : "#FBFBFB"))
        border.width: control.primary ? 0 : 1
        border.color: control.dark ? "#5A5A5A" : "#ADADAD"
    }
}
