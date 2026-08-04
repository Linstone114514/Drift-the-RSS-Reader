import QtQuick
import QtQuick.Controls.Material
import Drift

ApplicationWindow {
    id: root

    // Controller is set via initialProperties from main.cpp
    required property var controller

    width: controller.settings ? controller.settings.windowWidth : 1280
    height: controller.settings ? controller.settings.windowHeight : 800
    minimumWidth: 500
    minimumHeight: 400
    title: "Drift"

    Material.theme: {
        if (controller.settings && controller.settings.theme === "dark") return Material.Dark
        if (controller.settings && controller.settings.theme === "light") return Material.Light
        return Material.System
    }
    Material.accent: Material.Teal

    onClosing: {
        if (controller) {
            controller.saveWindowGeometry(width, height)
        }
    }

    // Show the main two-pane layout
    MainWindow {
        anchors.fill: parent
        controller: root.controller
    }
visible: true
}
