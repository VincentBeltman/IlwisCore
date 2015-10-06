import QtQuick 2.0;
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller


Modeller.ModellerWorkArea {
    property WorkflowModel workflow;
    property var deleteItemIndex;



    function deleteSelectedOperation(){
        for(var i=0; i < wfCanvas.operationsList.length; ++i){
            var item = wfCanvas.operationsList[i]
            if (item.isSelected) {
                deleteItemIndex = i
                messageDialog.open()
                break
            }
        }
    }

    MessageDialog {
        id: messageDialog
        title: "Deleting operation"
        text: "Are you sure you want to delete this operation?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            var item = wfCanvas.operationsList[deleteItemIndex]
            wfCanvas.operationsList.splice(deleteItemIndex, 1)
            item.destroy()
            workflow.deleteOperation(item)
        }
        Component.onCompleted: visible = false
    }

    Canvas {
        Keys.onDeletePressed: {
            deleteSelectedOperation()
        }
        Keys.onBackPressed: {
            deleteSelectedOperation()
        }

        id : wfCanvas
        anchors.fill: parent

        property var ctx: getContext('2d')
        property bool canvasValid: true

        property double oldx : -1.0
        property double oldy : -1.0
        property point workingLineBegin : Qt.point(-1,-1)
        property point workingLineEnd : Qt.point(-1,-1)
        property int currentIndex : 0
        property var component;
        property var currentItem;
        property var operationsList: []
        property int count : 0

        Timer {
            interval: 30;
            running: true;
            repeat: true
            onTriggered: wfCanvas.draw()
        }


        /*
     * Get the resource by id from MasterCatalog
     */
        function getOperation(id) {
            var oper = operations.operation(id);
            return oper;
        }
        function invalidate() {
            canvasValid = false;
        }



        function draw(force){
            if (canvasValid == false || (force !== null && force)) {
                clear(ctx);
                canvasValid = true
                if ( workingLineBegin.x !== -1 && workingLineEnd.x !== -1){
                    ctx.beginPath();
                    ctx.lineWidth = 2;
                    ctx.moveTo(workingLineBegin.x, workingLineBegin.y);
                    ctx.strokeStyle = "red"
                    ctx.lineTo(workingLineEnd.x, workingLineEnd.y);
                    ctx.stroke();
                }
                for( var i=0; i < operationsList.length; ++i){
                    operationsList[i].drawFlows(ctx)
                }
                //wfCanvas.requestPaint();
            }
        }



        function createItem(x,y, resource) {
            component = Qt.createComponent("OperationItem.qml");
            if (component.status == Component.Ready)
                finishCreation(x,y,resource);
            else
                component.statusChanged.connect(finishCreation);
        }

        function finishCreation(x,y,resource) {
            if (component.status == Component.Ready) {
                currentItem = component.createObject(wfCanvas, {"x": x, "y": y, "operation" : resource,"itemid" : count});
                if (currentItem == null) {
                    // Error Handling
                    console.log("Error creating object");
                }
                operationsList.push(currentItem)
                workflow.addOperation(count, resource.id)
                ++count

            } else if (component.status == Component.Error) {
                // Error Handling
                console.log("Error loading component:", component.errorString());
            }
        }

        /*
     * Clear the Canvas
     */
        function clear() {
            if ( ctx){
                ctx.reset();
                ctx.clearRect(0, 0, width, height);
                ctx.stroke();
                wfCanvas.requestPaint();
            }
        }

        function stopWorkingLine(){
            wfCanvas.oldx = -1.0
            wfCanvas.oldy = -1.0
            wfCanvas.workingLineBegin = Qt.point(-1,-1)
            wfCanvas.workingLineEnd = Qt.point(-1,-1)
            wfCanvas.canvasValid = true
        }

        function showAttachementForm(yesno, target, attachRect){
            var fromOperation = operationsList[wfCanvas.currentIndex].operation
            attachementForm.operationFrom = fromOperation
            attachementForm.operationTo = target.operation
            attachementForm.attachRect = attachRect
            attachementForm.target = target
            attachementForm.state = yesno ? "visible" : "visible"
        }

        onWidthChanged: {
            // force re-draw if the ModellerPanel width has changed
            invalidate();
        }

        onHeightChanged: {
            // force re-draw if the ModellerPanel height has changed
            wfCanvas.draw(true);
        }

        DropArea {
            id: canvasDropArea
            anchors.fill: wfCanvas
            onDropped: {
                if (drag.source.type === "singleoperation") {
                    var oper = wfCanvas.getOperation(drag.source.ilwisobjectid)
                    wfCanvas.createItem(drag.x - 50, drag.y - 30,oper)
                    workflow.addOperation(wfCanvas.count, drag.source.ilwisobjectid)

                }

            }
        }
        FlowParametersChoiceForm{
            id : attachementForm
        }

        MouseArea {
            id: area
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            hoverEnabled: wfCanvas.workingLineBegin.x !== -1

            onPressed: {
                wfCanvas.canvasValid = false;
                for(var i=0; i < wfCanvas.operationsList.length; ++i){
                    var item = wfCanvas.operationsList[i]
                    var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)

                    for(var j=0; j < item.flowConnections.length; j++)
                    {
                        var flow = item.flowConnections[j];

                        // Retrieve basic X and Y positions of the line
                        var startPoint = flow.attachsource.center();
                        var endPoint = flow.attachtarget.center();
                        var ax = startPoint.x;
                        var ay = startPoint.y;
                        var bx = endPoint.x;
                        var by = endPoint.y;

                        // Calculate distance to check mouse hits a line
                        var distanceAC = Math.sqrt(Math.pow((ax-mouseX), 2) + Math.pow((ay-mouseY), 2));
                        var distanceBC = Math.sqrt(Math.pow((bx-mouseX), 2) + Math.pow((by-mouseY), 2));
                        var distanceAB = Math.sqrt(Math.pow((ax-bx), 2) + Math.pow((ay-by), 2));


                        // Check if mouse intersects the line with offset of 10
                        if((distanceAC + distanceBC) >= distanceAB &&
                           (distanceAC + distanceBC) < (distanceAB + 10))
                            flow.isSelected = true;
                         else
                            flow.isSelected = false;
                    }

                    if ( isContained) {
                        wfCanvas.oldx = mouseX
                        wfCanvas.oldy = mouseY
                        wfCanvas.currentIndex = i;
                        item.isSelected = true
                        manager.showOperationForm(item.operation.id)
                        manager.showMetaData(item.operation)
                    } else {
                        item.isSelected = false
                        manager.clearMetaData();
                    }
                }
            }

            Keys.onEscapePressed: {
                console.log("escape key");
                wfCanvas.stopWorkingLine()
            }

            onPositionChanged: {
                if ( attachementForm.state == "invisible"){
                    if ( wfCanvas.workingLineBegin.x !== -1){
                        wfCanvas.workingLineEnd = Qt.point( mouseX, mouseY)
                        wfCanvas.canvasValid = false
                    }
                    if ( wfCanvas.oldx >= 0 && wfCanvas.oldy >= 0 && wfCanvas.currentIndex >= 0)    {

                        var item = wfCanvas.operationsList[wfCanvas.currentIndex]
                        if ( item){
                            item.x += ( mouseX - wfCanvas.oldx)
                            item.y += (mouseY - wfCanvas.oldy)
                            wfCanvas.oldx = mouseX
                            wfCanvas.oldy = mouseY
                        }
                    }
                }
            }

            onReleased: {
                wfCanvas.stopWorkingLine()
            }
        }


    }
}
