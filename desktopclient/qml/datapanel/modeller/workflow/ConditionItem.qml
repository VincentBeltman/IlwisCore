import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global


Rectangle {
    id : conditionItem
    width: 200
    height: 180
    color: "#80bfff"
    border.width: 1
    border.color: "black"
    property var operationList : []

    function setCanvasColor(color){
        conditionRectangle.color = color;
    }

    function addToOperationList(operationIndex) {
        if(operationList.indexOf(operationIndex) == -1) {
            operationList.push(operationIndex)
        }
    }

    function removeFromOperationList(operationIndex) {
        if(operationList.indexOf(operationIndex) != -1) {
            operationList.splice(operationList.indexOf(operationIndex), 1)
        }
    }

    ListModel {
        id: testModel
        ListElement {
            testCondition: "a > b"
        }
        ListElement {
            testCondition: "c > d"
        }
        ListElement {
            testCondition: "e > f"
        }
        ListElement {
            testCondition: "g > h"
        }
    }

    Rectangle {
        id: listRectangle

        border.width: 1
        border.color: "black"
        color: "#cce5ff"
        height : 40
        width : parent.width - 8
        x : 4
        y: 4

        ListView{
            id : conditionList

            clip : true
            height : parent.height
            width : parent.width
            x: 0
            y: 0

            model : testModel
            delegate: Text {
                text: testCondition
                width: conditionList.width
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }


    Rectangle {
        id: conditionRectangle

        anchors.topMargin: 4
        anchors.top: listRectangle.bottom
        border.width: 1
        border.color: "black"
        color : "white"
        height: parent.height - listRectangle.height - 12
        width : parent.width - 8
        x : 4

        Canvas{
            id : conditionContainerCanvas
            anchors.fill : parent
        }
    }
}
