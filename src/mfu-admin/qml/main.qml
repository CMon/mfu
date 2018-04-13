import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0

import CMon.MFUAdmin 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    width: 1024
    height: 768
    title: qsTr("MFU Administration")

    Dialog {
        id: editDialog
        property variant index: null

        title: qsTr("Edit Entry")
        standardButtons: Dialog.Ok | Dialog.Cancel

        GridLayout {
            id: grid

            anchors.fill: parent
            columns: 2

            Text { text: qsTr("MAC Address:") }
            Text { id: edMac; }

            Text { text: qsTr("Name:") }
            TextField { id: edName; }

            Text { text: qsTr("Location:") }
            TextField { id: edLocation; }

            Text { text: qsTr("Stream Url:") }
            TextField { id: edStreamUrl; }

            Text { text: qsTr("Stream Port:") }
            SpinBox { id: edStreamPort; stepSize: 1; from: 0; to: 65535 }

            Text { text: qsTr("Registered:") }
            CheckBox { id: edRegistered }
        }

        onVisibleChanged: {
            if (!index) return
            var base = Qt.UserRole

            edMac.text           = clientsModel.data(index, base + 1);
            edName.text          = clientsModel.data(index, base + 2);
            edLocation.text      = clientsModel.data(index, base + 3);
            edStreamUrl.text     = clientsModel.data(index, base + 4);
            edStreamPort.value   = clientsModel.data(index, base + 5);
            edRegistered.checked = clientsModel.data(index, base + 6);
        }
        onAccepted: {
            clientsModel.updateClient(edMac.text, edName.text, edLocation.text, edStreamUrl.text, edStreamPort.text, edRegistered.checked)
        }
    }

    header: MenuBar {
        Menu {
            title: qsTr("File")
            Action { text: qsTr("Quit"); onTriggered: appWindow.close() }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10
        spacing: 10

        Login {
            Layout.fillWidth: true
            id: loginBox
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TableView {
                anchors.fill: parent
                TableViewColumn { id:tvcMac; role: "macAddress"; title: qsTr("MAC Address") }
                TableViewColumn { role: "name";       title: qsTr("Name") }
                TableViewColumn { role: "streamUrl";  title: qsTr("Stream Url") }
                TableViewColumn { role: "streamPort"; width: 100; title: qsTr("Stream Port") }
                TableViewColumn { role: "registered"; width: 100; title: qsTr("Registered") }
                TableViewColumn { role: "loggedIn";   width: 100; title: qsTr("Logged In") }
                TableViewColumn { role: "action";     width: 100; title: qsTr("Action"); delegate: Button {
                        text: qsTr("Edit")
                        onClicked: {
                            editDialog.index = styleData.value
                            editDialog.visible = true
                        }
                    }}

                model: clientsModel
            }
        }
    }

    footer: Text {
        text: MFUAdmin.status
    }
}
