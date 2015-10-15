import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1


Rectangle {
    width: parent.width

    TableView {

        width: parent.width
        height: parent.height

        TableViewColumn {
            role: "number"
            title: "#"
            width: 30
        }
        TableViewColumn {
            role: "message"
            title: "Message"
            width: parent.width - 40
        }
    }
}

