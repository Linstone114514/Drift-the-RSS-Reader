import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

ToolBar {
    id: root
    required property var controller

    height: 48

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // Search field
        SearchField {
            id: searchField
            Layout.fillWidth: true
            onSearchTextChanged: function(text) {
                controller.articleListModel.setSearchQuery(text)
            }
        }

        // Unread-only toggle
        ToolButton {
            text: "📋"
            checkable: true
            ToolTip.text: qsTr("Show unread only")
            ToolTip.visible: hovered
            onCheckedChanged: controller.articleListModel.setShowUnreadOnly(checked)
        }

        // Mark all read
        ToolButton {
            text: "✓✓"
            ToolTip.text: qsTr("Mark all read")
            ToolTip.visible: hovered
            onClicked: controller.markAllRead()
        }

        // Refresh button
        ToolButton {
            text: "↻"
            ToolTip.text: qsTr("Refresh all feeds")
            ToolTip.visible: hovered

            RotationAnimation on rotation {
                from: 0
                to: 360
                duration: 1000
                running: controller.isRefreshing
                loops: Animation.Infinite
            }

            onClicked: controller.refreshAll()
        }

        // Refresh progress
        Label {
            text: controller.isRefreshing
                  ? controller.refreshProgressDone + "/" + controller.refreshProgressTotal
                  : controller.articleListModel.count + " articles"
            font.pixelSize: 11
            color: Material.secondaryTextColor
            Layout.minimumWidth: 80
            horizontalAlignment: Text.AlignRight
        }
    }
}
