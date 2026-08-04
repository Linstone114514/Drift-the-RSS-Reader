import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

// This file is only compiled when QtWebView is available.
// When DRIFT_HAS_WEBVIEW is false, it is excluded from the build
// and controller.openOriginal() uses QDesktopServices::openUrl instead.

Item {
    id: root
    property string url: ""

    // Placeholder: when QtWebView module is installed, this would use:
    // import QtWebView
    // WebView { anchors.fill: parent; url: root.url }

    Label {
        anchors.centerIn: parent
        text: "WebView not available. Open the link in your browser."
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        color: Material.secondaryTextColor
    }
}
