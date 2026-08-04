import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Dialog {
    id: root
    required property var controller

    title: qsTr("Add Feed")
    standardButtons: Dialog.Ok | Dialog.Cancel
    modal: true
    width: 400

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: qsTr("Enter the URL of an RSS or Atom feed:")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        TextField {
            id: urlField
            placeholderText: "https://example.com/feed.xml"
            Layout.fillWidth: true
            selectByMouse: true
        }

        Label {
            id: errorLabel
            color: Material.color(Material.Red)
            visible: false
            Layout.fillWidth: true
        }
    }

    onAccepted: {
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
        controller.addFeed(url)
        urlField.text = ""
    }

    onOpened: {
        urlField.text = ""
        urlField.forceActiveFocus()
    }
}
