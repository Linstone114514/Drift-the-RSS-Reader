import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Windows 11 style dialog frame: rounded card, custom title bar with close
// button, centered in the window. Declared children become the dialog body;
// assign WinDialogButtons to `buttons` for the bottom-right button row.
Dialog {
    id: root

    default property alias content: body.data
    property alias buttons: buttonRow.data
    property string dialogTitle: ""

    modal: true
    padding: 0
    closePolicy: Popup.CloseOnEscape
    anchors.centerIn: Overlay.overlay

    readonly property bool dark: root.Material.theme === Material.Dark

    background: Rectangle {
        radius: 8
        color: root.dark ? "#2C2C2C" : "#FFFFFF"
        border.width: 1
        border.color: root.dark ? "#454545" : "#C6C6C6"
    }

    contentItem: ColumnLayout {
        spacing: 0

        // ── Title bar row ─────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 24
            Layout.rightMargin: 10
            Layout.topMargin: 18
            spacing: 8

            Label {
                text: root.dialogTitle
                font.pixelSize: 20
                font.weight: Font.DemiBold
                Layout.fillWidth: true
            }

            AbstractButton {
                id: closeButton
                Layout.preferredWidth: 36
                Layout.preferredHeight: 32
                hoverEnabled: true
                onClicked: root.reject()

                background: Rectangle {
                    radius: 4
                    color: closeButton.hovered
                         ? (root.dark ? "#3D3D3D" : "#E9E9E9")
                         : "transparent"
                }

                contentItem: Text {
                    text: "\u2715"
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: Material.secondaryTextColor
                }
            }
        }

        // ── Body (declared children land here) ───────────
        ColumnLayout {
            id: body
            Layout.fillWidth: true
            Layout.margins: 24
            Layout.topMargin: 16
            spacing: 10
        }

        // ── Divider + button row ─────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.topMargin: 8
            height: 1
            visible: buttonRow.visible
            color: Material.dividerColor
        }

        RowLayout {
            id: buttonRow
            Layout.fillWidth: true
            Layout.margins: 24
            Layout.topMargin: 16
            spacing: 8
            visible: buttonRow.implicitWidth > 0

            Item { Layout.fillWidth: true }
        }
    }
}
