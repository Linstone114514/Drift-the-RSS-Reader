import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Drift

Item {
    id: root
    required property var controller

    // Public API
    function openImport() { fileDialog.openMode = "import"; fileDialog.open() }
    function openExport() { fileDialog.openMode = "export"; fileDialog.open() }

    // ── File Dialog ──────────────────────────────────────────────
    property string _openMode: "import"

    FileDialog {
        id: fileDialog
        property string openMode: "import"
        title: openMode === "import" ? qsTr("Import OPML File") : qsTr("Export OPML File")
        nameFilters: ["OPML files (*.opml *.xml)", "All files (*)"]
        fileMode: openMode === "import" ? FileDialog.OpenFile : FileDialog.SaveFile
        defaultSuffix: "opml"

        onAccepted: {
            const path = selectedFile.toString()
            if (openMode === "import") {
                resultDialog.resultText = controller.importOpml(path)
            } else {
                const error = controller.exportOpml(path)
                resultDialog.resultText = error
                    ? error
                    : qsTr("Feeds exported successfully to:\n") + path
            }
            resultDialog.open()
        }
    }

    // ── Result Dialog ────────────────────────────────────────────
    Dialog {
        id: resultDialog
        property string resultText: ""
        title: qsTr("OPML Result")
        standardButtons: Dialog.Ok
        modal: true
        width: 400

        Label {
            text: resultDialog.resultText
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
