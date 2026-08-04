import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

ListView {
    id: root
    required property var controller

    // Signals for actions that the parent (MainWindow) handles
    signal addFeedRequested()
    signal importOpmlRequested()
    signal exportOpmlRequested()
    signal aboutRequested()

    clip: true
    spacing: 0

    model: controller.feedTreeModel

    // Section headers and delegates
    section.property: "nodeType"
    section.criteria: ViewSection.FullString
    section.delegate: sectionDelegate

    delegate: FeedNodeDelegate {
        width: root.width
    }

    // ── Context menu on right-click ────────────────────────────────
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: function(mouse) {
            const idx = root.indexAt(mouse.x, mouse.y)
            if (idx < 0) return

            const nodeType = model.data(model.index(idx, 0), controller.feedTreeModel.NodeTypeRole)
            if (nodeType !== 2) return // FeedNode only

            feedContextMenu.feedId = model.data(model.index(idx, 0), controller.feedTreeModel.FeedIdRole)
            feedContextMenu.feedTitle = model.data(model.index(idx, 0), controller.feedTreeModel.TitleRole)
            feedContextMenu.popup()
        }
    }

    Menu {
        id: feedContextMenu
        property string feedId
        property string feedTitle

        MenuItem {
            text: qsTr("Refresh")
            onTriggered: controller.refreshFeed(feedContextMenu.feedId)
        }
        MenuItem {
            text: qsTr("Mark All Read")
            onTriggered: controller.markFeedRead(feedContextMenu.feedId)
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Remove Feed")
            onTriggered: controller.removeFeed(feedContextMenu.feedId)
        }
    }

    // ── Section delegate (categories) ──────────────────────────────
    Component {
        id: sectionDelegate

        ItemDelegate {
            width: root.width
            height: 36

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 6

                Label {
                    text: section === "1" ? "" : section  // "1" = CategoryNode
                    font.weight: Font.Bold
                    font.pixelSize: 12
                    color: Material.secondaryTextColor
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                // Unread badge removed from section delegate
                // (section model doesn't have unreadCount role)
            }
        }
    }

    // ── Footer: add feed and OPML buttons ─────────────────────────
    footer: ColumnLayout {
        width: root.width
        spacing: 0

        ItemDelegate {
            Layout.fillWidth: true
            text: qsTr("＋ Add Feed")
            onClicked: root.addFeedRequested()
        }

        ItemDelegate {
            Layout.fillWidth: true
            text: qsTr("↥ Import OPML")
            onClicked: root.importOpmlRequested()
        }

        ItemDelegate {
            Layout.fillWidth: true
            text: qsTr("↧ Export OPML")
            onClicked: root.exportOpmlRequested()
        }

        ItemDelegate {
            Layout.fillWidth: true
            text: qsTr("ⓘ About Drift")
            onClicked: root.aboutRequested()
        }
    }
}
