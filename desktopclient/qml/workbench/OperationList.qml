import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0

import "../Global.js" as Global

Rectangle {

    signal makeForm(string objectid, string name, string url)

    property var operationsModel : operations.operations
    property bool byKey : false

    width : parent.width
    color : "white"
    opacity: width > 4 ? 1 : 0
    clip : true
    ListView {
        id : operationsList
        anchors.fill: parent
        model : operationsModel
        interactive : !byKey
        delegate : Item{
            id : currentOperation
            height : 35;
            width : parent.width;
            Rectangle {
                anchors.fill: parent
                color: operationsList.currentIndex === index ? Global.selectedColor : index  % 2 == 0 ? "#F7F9FC" : "#DCDCDC"
                Text {
                    id : operationName
                    text : displayName
                    anchors.top : parent.top
                    width : parent.width
                    height : 17
                    x : 5
                    font.pointSize: 12
                }
                Text {
                    id : operationSyntax
                    text : syntax()
                    anchors.top : operationName.bottom
                    width : parent.width
                    height : 12
                    font.pointSize: 8
                    x : 5
                    color : "grey"
                }
            }

            focus : true
            MouseArea {
                id: mouseArea
                hoverEnabled: true
                anchors.fill: parent
                cursorShape: Qt.ArrowCursor
//                property variant operationNameDrag
//                drag.target: operationNameDrag

//                onPressed: {

//                    operationNameDrag = Qt.createQmlObject('import QtQuick 2.0; Text {
//                        id : operationNameDrag
//                        text : displayName
//                        width : operationName.width
//                        height : operationName.height
//                        x : operationName.x
//                        y : operationName.y
//                        font.pointSize: 12
//                        property string name :  displayName !== null ? displayName : ""
//                        property string ilwisobjectid : id !== null ? id : ""
//                        property string type : "operation"

//                        Drag.keys: [ id ]
//                        Drag.active: mouseArea.drag.active
//                        Drag.hotSpot.x: x + width/2
//                        Drag.hotSpot.y: y + height/2
//                        opacity : Drag.active / 2

//                        states: State {
//                            when: mouseArea.drag.active
//                            ParentChange { target: operationNameDrag; parent: root }
//                            AnchorChanges { target: operationNameDrag; anchors.verticalCenter: undefined; anchors.horizontalCenter: undefined }
//                        }
//                    }', mouseArea, "dynamicName");

//                }

//                onReleased: {
//                    operationNameDrag.Drag.drop()
//                    operationNameDrag.parent = mouseArea
//                    operationNameDrag.anchors.fill = mouseArea
//                    operationNameDrag.destroy();
//                    // open formular
//                    applicationForm.state = operationsList.currentIndex == index && applicationForm.state != "minimized" ? "minimized" : "maximized"
//                    operationsList.currentIndex = index;
//                    makeForm(id, displayName, url)
//                }

                onClicked: {
                    applicationForm.state = operationsList.currentIndex == index && applicationForm.state != "minimized" ? "minimized" : "maximized"
                    operationsList.currentIndex = index;
                    makeForm(id, displayName, url)
                }
            }

        }
    }

}
