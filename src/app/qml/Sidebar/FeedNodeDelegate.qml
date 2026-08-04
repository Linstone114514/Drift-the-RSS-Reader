import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

ItemDelegate {
    id: root
    width: ListView.view ? ListView.view.width : 200
    height: 32

    property bool isFeedNode: model.nodeType === 2

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: isFeedNode ? 24 : 8
        anchors.rightMargin: 8
        spacing: 6

        // Favicon or icon indicator
        Rectangle {
            width: 18
            height: 18
            radius: 3
            color: isFeedNode ? Material.accent : "transparent"
            visible: isFeedNode

            Image {
                anchors.fill: parent
                anchors.margins: 2
                source: model.iconPath || ""
                visible: model.iconPath !== ""
                fillMode: Image.PreserveAspectFit
            }
        }

        Label {
            text: model.title || ""
            font.weight: isFeedNode ? Font.Normal : Font.Bold
            font.pixelSize: isFeedNode ? 12 : 13
            elide: Text.ElideRight
            color: model.unreadCount > 0 && isFeedNode
                   ? Material.foreground
                   : Material.secondaryTextColor
            font.bold: model.unreadCount > 0 && isFeedNode
            Layout.fillWidth: true
        }

        UnreadBadge {
            count: model.unreadCount || 0
            visible: count > 0 && isFeedNode
        }
    }

    onClicked: {
        if (model.nodeType === 2) {
            // Feed node: show feed articles
            controller.setView(4, model.feedId) // FeedView = 4
        } else if (model.nodeType === 1) {
            // Category node: toggle expand
            // (Simplified: just keep expanded state visual)
        }
    }
}
