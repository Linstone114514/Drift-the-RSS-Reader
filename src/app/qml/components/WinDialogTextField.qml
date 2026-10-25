import QtQuick
import QtQuick.Controls

// Windows style text field for WinDialogs: full border, accent highlight on
// focus. The built-in `accepted` signal fires on Return/Enter.
TextField {
    id: control

    readonly property bool dark: control.Material.theme === Material.Dark

    selectByMouse: true
    font.pixelSize: 13
    leftPadding: 10
    rightPadding: 10
    topPadding: 7
    bottomPadding: 7

    background: Rectangle {
        radius: 4
        color: control.dark ? "#333333" : "#FFFFFF"
        border.width: control.activeFocus ? 2 : 1
        border.color: control.activeFocus ? Material.accentColor
                     : control.dark ? "#5A5A5A" : "#8A8A8A"
    }
}
