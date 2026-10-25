import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Dialog {
    id: root
    required property var controller

    property string feedId: ""
    property string feedTitle: ""
    property string feedUrl: ""
    property string feedSiteUrl: ""
    property string feedLastFetched: ""
    property string feedLastError: ""
    property bool feedPaused: false

    title: qsTr("Feed Settings")
    standardButtons: Dialog.Ok | Dialog.Cancel
    modal: true
    width: 420

    // Load feed info when opened
    function loadFeedInfo() {
        if (feedId === "" || !controller) return
        const info = controller.feedInfo(feedId)
        if (info && Object.keys(info).length > 0) {
            feedTitle = info.title || ""
            feedUrl = info.url || ""
            feedSiteUrl = info.siteUrl || ""
            feedLastFetched = info.lastFetched || ""
            feedLastError = info.lastError || ""
            feedPaused = info.paused || false
        }
    }

    onOpened: loadFeedInfo()

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // ── Title ────────────────────────────────────────────────────
        Label {
            text: qsTr("Title")
            font.pixelSize: 11
            color: Material.secondaryTextColor
        }
        TextField {
            id: titleField
            text: root.feedTitle
            placeholderText: qsTr("Feed title")
            Layout.fillWidth: true
            selectByMouse: true
        }

        // ── URL ──────────────────────────────────────────────────────
        Label {
            text: qsTr("Feed URL")
            font.pixelSize: 11
            color: Material.secondaryTextColor
        }
        TextField {
            id: urlField
            text: root.feedUrl
            placeholderText: "https://example.com/feed.xml"
            Layout.fillWidth: true
            selectByMouse: true
            readOnly: true
            color: readOnly ? Material.secondaryTextColor : Material.foreground
        }

        // ── Paused ───────────────────────────────────────────────────
        CheckBox {
            id: pausedCheck
            text: qsTr("Pause updates for this feed")
            checked: root.feedPaused
        }

        // ── Info ─────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Material.dividerColor
        }

        GridLayout {
            columns: 2
            Layout.fillWidth: true
            columnSpacing: 12
            rowSpacing: 4

            Label {
                text: qsTr("Last fetched:")
                font.pixelSize: 11
                color: Material.secondaryTextColor
            }
            Label {
                text: root.feedLastFetched || qsTr("Never")
                font.pixelSize: 11
            }

            Label {
                text: qsTr("Site URL:")
                font.pixelSize: 11
                color: Material.secondaryTextColor
            }
            Label {
                text: root.feedSiteUrl || qsTr("Unknown")
                font.pixelSize: 11
                elide: Text.ElideMiddle
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Last error:")
                font.pixelSize: 11
                color: Material.secondaryTextColor
                visible: root.feedLastError !== ""
            }
            Label {
                text: root.feedLastError
                font.pixelSize: 11
                color: Material.color(Material.Red)
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                visible: root.feedLastError !== ""
            }
        }

        // ── Open site ────────────────────────────────────────────────
        Button {
            text: qsTr("Open Website")
            enabled: root.feedSiteUrl !== ""
            flat: true
            onClicked: Qt.openUrlExternally(root.feedSiteUrl)
        }

        Item { Layout.fillHeight: true }

        // ── Delete ───────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Material.dividerColor
        }

        Button {
            text: qsTr("Delete Feed")
            flat: true
            Layout.fillWidth: true

            contentItem: Label {
                text: parent.text
                color: Material.color(Material.Red)
                horizontalAlignment: Text.AlignHCenter
            }

            onClicked: deleteConfirmDialog.open()
        }
    }

    onAccepted: {
        if (!root.controller || root.feedId === "") return
        root.controller.updateFeed(
            root.feedId,
            titleField.text.trim(),
            urlField.text.trim(),
            pausedCheck.checked
        )
    }

    // ── Delete confirmation dialog ───────────────────────────────────
    Dialog {
        id: deleteConfirmDialog
        title: qsTr("Delete Feed")
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        width: 350

        Label {
            anchors.fill: parent
            text: qsTr("Are you sure you want to delete \"%1\"?\n\nThis will remove the feed and all its cached content. This action cannot be undone.").arg(root.feedTitle)
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            if (root.controller && root.feedId !== "") {
                root.controller.removeFeed(root.feedId)
            }
            root.reject() // Close the settings dialog too
        }
    }
}
