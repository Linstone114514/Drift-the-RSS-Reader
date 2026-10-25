import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

WinDialog {
    id: root
    required property var controller

    dialogTitle: qsTr("Add Feed")
    width: 420

    function tryAccept() {
        const url = urlField.text.trim()
        if (url === "") {
            errorLabel.text = qsTr("Please enter a feed URL.")
            errorLabel.visible = true
            return
        }
        if (!url.startsWith("http://") && !url.startsWith("https://")) {
            errorLabel.text = qsTr("URL must start with http:// or https://")
            errorLabel.visible = true
            return
        }
        errorLabel.visible = false
        root.accept()
    }

    ColumnLayout {
        spacing: 10
        Layout.fillWidth: true

        Label {
            text: qsTr("Enter the URL of an RSS or Atom feed:")
            wrapMode: Text.WordWrap
            font.pixelSize: 13
            Layout.fillWidth: true
        }

        WinDialogTextField {
            id: urlField
            placeholderText: "https://example.com/feed.xml"
            Layout.fillWidth: true
            onAccepted: root.tryAccept()
        }

        Label {
            text: qsTr("Title (optional — will be auto-detected from the feed):")
            wrapMode: Text.WordWrap
            font.pixelSize: 12
            color: Material.secondaryTextColor
            Layout.fillWidth: true
        }

        WinDialogTextField {
            id: titleField
            placeholderText: qsTr("My Feed")
            Layout.fillWidth: true
            onAccepted: root.tryAccept()
        }

        Label {
            id: errorLabel
            font.pixelSize: 12
            color: Material.color(Material.Red)
            visible: false
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }

    buttons: [
        WinDialogButton {
            text: qsTr("Cancel")
            onClicked: root.reject()
        },
        WinDialogButton {
            text: qsTr("Add")
            primary: true
            onClicked: root.tryAccept()
        }
    ]

    onAccepted: {
        const url = urlField.text.trim()
        const title = titleField.text.trim()
        controller.addFeed(url, title)
        urlField.text = ""
        titleField.text = ""
    }

    onOpened: {
        urlField.text = ""
        titleField.text = ""
        errorLabel.visible = false
        urlField.forceActiveFocus()
    }
}
