import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Drift

Dialog {
    id: root
    required property var controller

    property string feedId: ""
    property string feedTitle: ""

    title: qsTr("Feed Settings: ") + feedTitle
    standardButtons: Dialog.Ok
    modal: true
    width: 350

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: qsTr("Feed settings will be available in a future update.")
            wrapMode: Text.WordWrap
            color: Material.secondaryTextColor
            Layout.fillWidth: true
        }
    }
}
