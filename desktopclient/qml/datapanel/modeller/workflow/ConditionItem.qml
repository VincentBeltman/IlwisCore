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

            Component.onCompleted: {
                var ctx = getContext("2d")
               // setup the stroke
               ctx.lineWidth = 4
               ctx.strokeStyle = "blue"
               // setup the fill
               ctx.fillStyle = "steelblue"
               // begin a new path to draw
               ctx.beginPath()
               // top-left start point
               ctx.moveTo(50,50)
               // upper line
               ctx.lineTo(150,50)
               // right line
               ctx.lineTo(150,150)
               // bottom line
               ctx.lineTo(50,150)
               // left line through path closing
               ctx.closePath()
               // fill using fill style
               ctx.fill()
               // stroke using line width and stroke style
               ctx.stroke()
            }
        }
    }
}
