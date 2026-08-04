import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Item {
    id: root
    required property var controller

    height: 48
    implicitHeight: 48

    // Horizontal scrollable row of filter chips
    ListView {
        anchors.fill: parent
        anchors.margins: 8
        orientation: ListView.Horizontal
        spacing: 6
        model: ListModel {
            ListElement { label: "All"; view: 0 }
            ListElement { label: "Unread"; view: 1 }
            ListElement { label: "Starred"; view: 2 }
            ListElement { label: "Today"; view: 3 }
        }

        delegate: ItemDelegate {
            width: Math.max(60, labelText.implicitWidth + 20)
            height: 32

            readonly property bool active: controller.articleListModel
                && controller.articleListModel.count > 0

            Rectangle {
                anchors.fill: parent
                radius: 16
                color: ListView.isCurrentItem
                       ? Material.accent
                       : Qt.lighter(Material.background, 1.1)
                border.color: ListView.isCurrentItem
                              ? Material.accent
                              : Material.dividerColor
                border.width: 1
            }

            Label {
                id: labelText
                anchors.centerIn: parent
                text: model.label
                font.pixelSize: 12
                font.weight: Font.Medium
                color: ListView.isCurrentItem ? "white" : Material.foreground
            }

            onClicked: {
                ListView.view.currentIndex = index
                controller.setView(model.view)
            }
        }

        Component.onCompleted: {
            currentIndex = 0
            controller.setView(0)
        }
    }
}
