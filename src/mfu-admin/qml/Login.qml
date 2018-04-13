import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import CMon.MFUAdmin 1.0

Rectangle {
    id: loginRoot

    color: "#FF0000"
    property bool loggedIn: false

    height: host.height + tfLogin.height + loginButton.height + ((grid.children.length/grid.columns) * grid.rowSpacing)

    GridLayout {
        id: grid
        columns: 4
        anchors.fill: parent

        Text { text: qsTr("Host:") }
        TextField {
            id: host
            text: "localhost"
        }
        Text { text: qsTr("Port:") }
        SpinBox {
            id: port
            stepSize: 1
            from: 0
            to: 65535
            value: 4711
        }

        Text { text: qsTr("Login:") }
        TextField {
            id: tfLogin
            focus: true
            onAccepted: loginButton.clicked()
        }
        Text { text: qsTr("Password:") }
        TextField {
            id: password
            echoMode: TextInput.Password
            onAccepted: loginButton.clicked()
        }

        Text {id: spacer1}
        Text {id: spacer2}
        Text {id: spacer3}
        Button {
            id: loginButton
            text: qsTr("&Login")
            onClicked: {
                MFUAdmin.connectToServer(host.text, port.value, tfLogin.text, password.text)
            }
        }
    }
}
