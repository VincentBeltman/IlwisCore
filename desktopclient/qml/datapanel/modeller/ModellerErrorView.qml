import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1
import WorkflowErrorModel 1.0
import ErrorModel 1.0

Rectangle {
    width: parent.width

    function addError(id, error) {
        errorWrapper.errorModel.addError(id, error)
    }

    WorkflowErrorModel {
        id: errorWrapper

        errorModel.onErrorlistChanged: {
            if(errorview.state == "smaller")
            {
                errorview.state = "bigger"
            }
        }
    }

    ListView {
        model: errorWrapper.errorModel.errorlist
        id:errors
        width: parent.width
        height: parent.height

        delegate: Text {
            text: modelData
            width: 100
            height: 20
        }
    }
}

