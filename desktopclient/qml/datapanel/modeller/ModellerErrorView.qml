import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1
//import WorkflowErrorModel 1.0


Rectangle {

    Timer {
        interval: 1000;
        running: true;
        repeat: true
        onTriggered: {
            errormodel.addError(1, "Testing");

            errors.model = errormodel.errorlist;
            console.log("error aangemaakt")
        }

    }


    id: modellerErrorView
    width: parent.width

//    WorkflowErrorModel {
//        id: errormodel
//    }

    ListView {
        model: errormodel
        id:errors
        width: parent.width
        height: parent.height

        delegate: Text {
            text: modelData
            width: 100
            height: 20
        }

        Component.onCompleted: {
            console.log("model.length");
        }
    }
}

