import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

ItemDelegate {
    id: root

    // === 第一次修改：添加这四行 ===
    property string articleId: model.articleId || ""
    property string articleTitle: model.title || ""
    property string articleLink: model.link || ""
    property bool articleStarred: model.isStarred || false
    // ==================

    width: ListView.view ? ListView.view.width : 300
    height: 80

    // Selected article (shown in reader)
    property bool selected: false

    Rectangle {
        anchors.fill: parent
        color: root.selected ? Qt.lighter(Material.accent, 1.8) : "transparent"
        visible: root.selected
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // Article info
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 2

            // Feed name + date
            RowLayout {
                spacing: 6

                Label {
                    text: model.feedName || ""
                    font.pixelSize: 10
                    color: Material.accent
                    elide: Text.ElideRight
                    Layout.maximumWidth: 120
                }

                Label {
                    text: "·"
                    font.pixelSize: 10
                    color: Material.secondaryTextColor
                    visible: model.feedName !== ""
                }

                Label {
                    text: model.publishedRelative || ""
                    font.pixelSize: 10
                    color: Material.secondaryTextColor
                }

                // Unread dot
                Rectangle {
                    width: 6
                    height: 6
                    radius: 3
                    color: Material.accent
                    visible: !model.isRead
                }

                Item { Layout.fillWidth: true }
            }

            // Title
            Label {
                text: model.title || qsTr("(Untitled)")
                font.pixelSize: 14
                font.weight: model.isRead ? Font.Normal : Font.Bold
                elide: Text.ElideRight
                maximumLineCount: 2
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            // Summary
            Label {
                text: model.summary || ""
                font.pixelSize: 11
                color: Material.secondaryTextColor
                elide: Text.ElideRight
                maximumLineCount: 1
                Layout.fillWidth: true
                visible: text !== ""
            }
        }

        // Star button
        StarButton {
            starred: model.isStarred
            Layout.alignment: Qt.AlignTop
            onStarToggled: controller.toggleStar(model.articleId)
        }
    }

    onClicked: {
        // Mark as read and show in reader
        if (!model.isRead) {
            controller.markRead([model.articleId], true)
        }
        root.ListView.view.currentIndex = index
        // The reader will be updated via the binding
    }
}
