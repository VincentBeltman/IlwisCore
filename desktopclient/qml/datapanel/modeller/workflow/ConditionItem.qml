import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../../Global.js" as Global


Rectangle {
    property int standardHeight: 180
    property int standardWidth: 200
    property int padding: 20

    id : conditionItem
    width: standardWidth
    height: standardHeight
    color: "#80bfff"
    border.width: 1
    border.color: "black"
    property var operationsList : []
    property var conditionContainerCanvas
    property var canvasComponent

    function setCanvasColor(color){
        conditionRectangle.color = color;
    }

    function addToOperationList(operationIndex) {
        if(operationsList.indexOf(operationIndex) == -1) {
            operationsList.push(operationIndex);
            resizeOneTime()
        }
    }

    function removeFromOperationList(operationIndex) {
        if(operationsList.indexOf(operationIndex) > -1) {
            operationsList.pop(operationIndex);
            resizeOneTime()
        }
    }

    function disableCanvas() {
        conditionContainerCanvas.destroy();
    }

    function enableCanvas() {
        canvasComponent = Qt.createComponent("ConditionCanvas.qml");
        if (canvasComponent.status == Component.Ready)
            finishCreation(canvasComponent);
        else
            canvasComponent.statusChanged.connect(finishCreation);
    }

    function finishCreation(component) {
        if (canvasComponent.status == Component.Ready) {
            var sprite = canvasComponent.createObject(conditionRectangle, {});
            if (sprite == null) {
                console.log("Error creating object");
            }
        } else if (canvasComponent.status == Component.Error) {
            console.log("Error loading component:", canvasComponent.errorString());
        }
    }

    function resize() {
        var minX = 1000000, maxX = -1000000, minY = 1000000, maxY = -1000000, operation, xChanged = false, yChanged = false;

        // Search for min and max, x and y
        for (var operationIndex in operationsList) {
            operation = wfCanvas.operationsList[operationIndex]

            if (operation.x < minX) {
                minX = operation.x
                xChanged = true
            }
            if (operation.x + operation.width > maxX) {
                maxX = operation.x + operation.width
                xChanged = true
            }

            if (operation.y < minY) {
                minY = operation.y;
                yChanged = true
            }
            if (operation.y + operation.height > maxY) {
                maxY = operation.y + operation.height
                yChanged = true
            }
        }

        conditionItem.width = xChanged ? maxX - minX + (padding * 2) : standardWidth
        conditionItem.height = yChanged ? maxY - minY + listRectangle.height + (padding * 2) : standardHeight
        if (xChanged) conditionItem.x = minX - padding
        if (yChanged) conditionItem.y = minY - listRectangle.height - padding
    }

    function resizeOneTime() {
        disableCanvas()
        resize()
        enableCanvas()
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

        ConditionCanvas {
        }
    }
}
