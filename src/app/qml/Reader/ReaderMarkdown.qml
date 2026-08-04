import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Flickable {
    id: root
    property string content: ""
    property int fontSize: 16

    clip: true
    contentWidth: width
    contentHeight: textContent.implicitHeight + 40
    flickableDirection: Flickable.VerticalFlick

    ScrollBar.vertical: ScrollBar {}

    // Empty state when no content
    EmptyState {
        anchors.centerIn: parent
        visible: root.content === ""
        message: qsTr("Select an article to start reading")
        icon: "📖"
    }

    Text {
        id: textContent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 16
        topPadding: 16
        bottomPadding: 16

        text: root.content
        textFormat: Text.MarkdownText
        font.pixelSize: root.fontSize
        color: Material.foreground
        wrapMode: Text.WordWrap
        selectable: true
        lineHeight: 1.6

        onLinkActivated: function(link) {
            Qt.openUrlExternally(link)
        }
    }
}
