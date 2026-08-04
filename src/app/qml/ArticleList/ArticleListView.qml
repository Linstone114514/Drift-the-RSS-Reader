import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

ListView {
    id: root
    required property var controller

    clip: true
    spacing: 0
    model: controller.articleListModel

    delegate: ArticleDelegate {
        width: root.width
    }

    // Empty state
    EmptyState {
        anchors.centerIn: parent
        visible: root.count === 0
        message: controller.articleListModel.searchQuery !== ""
                 ? qsTr("No articles match your search.")
                 : qsTr("No articles to display.\nAdd a feed to get started!")
        icon: controller.articleListModel.searchQuery !== "" ? "🔍" : "📰"
    }

    // Scroll to top when view changes
    Connections {
        target: controller.articleListModel
        function onCountChanged() {
            root.positionViewAtBeginning()
        }
    }

    ScrollBar.vertical: ScrollBar {}
}
