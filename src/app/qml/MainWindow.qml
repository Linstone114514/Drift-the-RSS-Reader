import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Item {
    id: root
    required property var controller

    // Responsive layout: StackView for narrow, SplitView for wide
    readonly property bool narrow: width < 600

    Item {
        anchors.fill: parent

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // ── Left sidebar ──────────────────────────────────────
            Rectangle {
                Layout.preferredWidth: Math.min(300, root.width * 0.35)
                Layout.minimumWidth: 180
                Layout.fillHeight: true
                color: Material.background

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // View chips (All / Unread / Starred / Today)
                    ViewChips {
                        controller: root.controller
                        Layout.fillWidth: true
                    }

                    // Feed tree
                    FeedTreeView {
                        id: feedTree
                        controller: root.controller
                        feedSettingsDialog: feedSettingsDialog
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        onAddFeedRequested: addFeedDialog.open()
                        onImportOpmlRequested: opmlDialog.openImport()
                        onExportOpmlRequested: opmlDialog.openExport()
                        onAboutRequested: aboutDialog.open()
                    }
                }
            }

            // ── Divider ──────────────────────────────────────────
            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: Material.dividerColor
            }

            // ── Middle + right panels (article list | reader) ────
            SplitView {
                id: contentSplit
                Layout.fillWidth: true
                Layout.fillHeight: true
                orientation: Qt.Horizontal

                // Thin divider-style handle, matching the sidebar divider
                handle: Rectangle {
                    implicitWidth: 5
                    color: "transparent"

                    Rectangle {
                        anchors.centerIn: parent
                        width: 1
                        height: parent.height
                        color: Material.dividerColor
                    }
                }

                // ── Middle panel (articles of selected feed) ──────
                ColumnLayout {
                    SplitView.minimumWidth: 240
                    SplitView.preferredWidth: 380
                    spacing: 0

                    ListHeader {
                        id: listHeader
                        controller: root.controller
                        Layout.fillWidth: true
                    }

                    ArticleListView {
                        id: articleList
                        controller: root.controller
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }

                // ── Right panel (selected article) ────────────────
                ReaderView {
                    id: readerView
                    controller: root.controller
                    SplitView.minimumWidth: 300
                    SplitView.fillWidth: true
                    articleId: articleList.currentItem ? articleList.currentItem.articleId : ""
                    articleTitle: articleList.currentItem ? articleList.currentItem.articleTitle : ""
                    articleLink: articleList.currentItem ? articleList.currentItem.articleLink : ""
                    articleStarred: articleList.currentItem ? articleList.currentItem.articleStarred : false
                }
            }
        }
    }

    // ── Dialogs ─────────────────────────────────────────────────────
    AddFeedDialog {
        id: addFeedDialog
        controller: root.controller
    }

    FeedSettingsDialog {
        id: feedSettingsDialog
        controller: root.controller
    }

    OpmlDialog {
        id: opmlDialog
        controller: root.controller
    }

    AboutDialog {
        id: aboutDialog
    }
}
