import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Item {
    id: root
    required property var controller

    // The currently selected article ID (set from ArticleListView)
    property string articleId: ""
    property string articleTitle: ""
    property string articleLink: ""
    property bool articleStarred: false

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Reader toolbar
        ToolBar {
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                // Back button (for narrow layout)
                ToolButton {
                    text: "←"
                    visible: false // shown only in narrow/mobile mode
                    onClicked: {}  // navigate back
                }

                Label {
                    text: root.articleTitle || qsTr("Select an article")
                    font.pixelSize: 16
                    font.weight: Font.Bold
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                StarButton {
                    starred: root.articleStarred
                    onStarToggled: {
                        if (root.articleId !== "")
                            controller.toggleStar(root.articleId)
                    }
                }

                ToolButton {
                    text: "🌐"
                    ToolTip.text: qsTr("Open original in browser")
                    ToolTip.visible: hovered
                    onClicked: {
                        if (root.articleLink !== "")
                            controller.openOriginal(root.articleLink)
                    }
                }
            }
        }

        // Reader content
        ReaderMarkdown {
            id: readerMarkdown
            Layout.fillWidth: true
            Layout.fillHeight: true
            content: root.articleId !== ""
                     ? controller.readerContent(root.articleId)
                     : ""
            fontSize: controller.settings ? controller.settings.readerFontSize : 16
        }

        // Offline banner
        Rectangle {
            Layout.fillWidth: true
            height: 28
            color: Material.accent
            visible: controller.isOffline
            opacity: 0.9

            Label {
                anchors.centerIn: parent
                text: qsTr("📡 You are offline — showing cached content")
                font.pixelSize: 11
                color: "white"
            }
        }
    }
}
