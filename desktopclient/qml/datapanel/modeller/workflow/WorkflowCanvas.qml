import QtQuick 2.0;
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import WorkflowModel 1.0
import QtQuick.Dialogs 1.1
import ".." as Modeller


Modeller.ModellerWorkArea {
    property WorkflowModel workflow;
    property OperationCatalogModel operationCatalog;
    property bool canvasActive : true;
    property var deleteItemIndex;
    property var deleteEdgeIndex;
    property int highestZIndex : 1;

    function asignConstantInputData(inputData, itemId) {
        var parameterIndexes = workflow.asignConstantInputData(inputData, itemId)
        wfCanvas.operationsList[itemId].resetInputModel()

        generateForm(parameterIndexes)
    }

    function deleteSelectedOperation(){
        for(var i=0; i < wfCanvas.operationsList.length; ++i){
            var item = wfCanvas.operationsList[i]
            if (item.isSelected) {
                deleteItemIndex = i
                deleteOperationDialog.open()
                break
            }
        }
    }

    function deleteSelectedEdge(){
        var flow = getSelectedEdge()
        if(flow != 0)
        {
            messageDialogEdge.open()
        }
    }

    function alterSelectedEdge(){
        var flow = getSelectedEdge()
        if(flow != 0)
        {
            //Retrieve target and rectangle before deleting the edge
            var target = flow.target;
            var attachedRect = flow.attachtarget;

            //Delete the edge
            var from = flow.source.itemid
            var to = flow.target.itemid
            var inputIndex = flow.flowPoints.toParameterIndex
            var outputIndex = flow.flowPoints.fromParameterIndex
            var operationObject = wfCanvas.operationsList[deleteItemIndex]

            workflow.deleteFlow(from, to, outputIndex, inputIndex)
            operationObject.flowConnections.splice(deleteEdgeIndex, 1)

            flow.target.resetInputModel()
            wfCanvas.canvasValid = false

            //Create a new edge
            wfCanvas.showAttachmentFormFromFlow(flow);
        }
    }

    function getSelectedEdge(){
        for(var i=0; i < wfCanvas.operationsList.length; ++i){
            var item = wfCanvas.operationsList[i]

            for(var j=0; j < item.flowConnections.length; j++)
            {
                var flow = item.flowConnections[j];

                if(flow.isSelected)
                {
                    deleteItemIndex = i;
                    deleteEdgeIndex = j;
                    return flow;
                }
            }
        }
        return 0;
    }

    MessageDialog {
        id: deleteOperationDialog
        title: "Deleting operation"
        text: "Are you sure you want to delete this operation?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            var item = wfCanvas.operationsList[deleteItemIndex]
            var flows = item.flowConnections;

            // First delete the operation at C++. THIS NEEDS TO BE DONE FIRST
            var parameterIndexes = workflow.deleteOperation(deleteItemIndex)

            // This removes 1 from the operation list beginning from deleteItemIndex
            wfCanvas.operationsList.splice(deleteItemIndex, 1)

            // Delete incomming connections of the operation which will be deleted
            for (var i = 0; i < wfCanvas.operationsList.length; i++) {
                var operation = wfCanvas.operationsList[i]
                var deleteFlows = [];
                // Search for deletable connections
                for (var j = 0; j < operation.flowConnections.length; j++) {
                    var flow = operation.flowConnections[j]
                    // If target is same as deleted operation
                    if (flow.target.itemid == item.itemid) {
                        deleteFlows.push(j)
                    }
                }
                // Delete the connections
                for (var j = 0; j < deleteFlows.length; j++) {
                    wfCanvas.operationsList[i].flowConnections.splice(deleteFlows[j] - j, 1)
                }
            }
            // Loop through all operations after the deleted item. We need to reset their itemid
            for (var i = deleteItemIndex; i < wfCanvas.operationsList.length; i++) {
                wfCanvas.operationsList[i].itemid = i
            }

            // Destroy the QML object.
            item.destroy()

            // Reset all targets of the flows of the deleted operation
            for (var i = 0; i < flows.length; i++) {
                flows[i].target.resetInputModel()
            }

            // Redraw lines
            wfCanvas.canvasValid = false
            wfCanvas.draw(true)

            generateForm(parameterIndexes)
        }
        Component.onCompleted: visible = false
    }

    MessageDialog {
        id: messageDialogEdge
        title: "Deleting edge"
        text: "Are you sure you want to delete this edge?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            var flow = wfCanvas.operationsList[deleteItemIndex].flowConnections[deleteEdgeIndex]
            var from = flow.source.itemid
            var to = flow.target.itemid
            var inputIndex = flow.flowPoints.toParameterIndex
            var outputIndex = flow.flowPoints.fromParameterIndex

            var parameterIndexes = workflow.deleteFlow(from, to, outputIndex, inputIndex)
            wfCanvas.operationsList[deleteItemIndex].flowConnections.splice(deleteEdgeIndex, 1)
            flow.target.resetInputModel()
            wfCanvas.canvasValid = false

            generateForm(parameterIndexes)
        }
        Component.onCompleted: {
            visible = false
        }
    }

    /**
      Calls the WorkflowModel's run method
      */
    function run(){
        workflow.createMetadata()
        manager.retrieveRunFormValues()
    }

    /**
      Calls the create meta data method of the WorkflowModel and regenerates the form
      */
    function generateForm(parameterIndexes) {
        if (workflow){
            var operationNames = {}
            for( var i=0; i < wfCanvas.operationsList.length; i++){
                var operationItem = wfCanvas.operationsList[i];
                operationItem.drawFlows(wfCanvas.ctx)
                operationNames[i + ". " + operationItem.operation.name] = {
                    inParameterCount: operationItem.operation.inParameterCount,
                    outParameterCount: operationItem.operation.outParameterCount
                };
            }

            workflow.createMetadata()
            manager.showRunForm(workflow.id, operationNames, parameterIndexes)
        }
    }

    /**
      Draws canvas from the workflow
      */
    function drawFromWorkflow() {
        var nodes = workflow.nodes, node, resource, unOrderdEdges = workflow.edges, edges = [], nodeEdges, edge, fromItemid,
                toItemId, fromOperation=false, toOperation=false, flowPoints;
        for(var i = 0; i < unOrderdEdges.length; i++) {
            edge = unOrderdEdges[i]
            if (edge.fromVertex in edges) {
                edges[edge.fromVertex].push(edge)
            } else {
                edges[edge.fromVertex] = [edge]
            }
        }

        for (var i = 0; i < nodes.length; i++) {
            node = nodes[i];
            resource = wfCanvas.getOperation(node.operationId)

            wfCanvas.createItem(node.x, node.y, resource)
        }
        for (var i = 0; i < nodes.length; i++) {
            node = nodes[i];
            nodeEdges = edges[node.vertex]
            if (nodeEdges) {
                for (var j = 0; j < nodeEdges.length; j++) {
                    edge = nodeEdges[j]
                    fromItemid = workflow.vertex2ItemID(node.vertex) //TODO: Temporary
                    toItemId = workflow.vertex2ItemID(edge.toVertex) //TODO: Temporary

                    for (var k = 0; k < wfCanvas.operationsList.length; k++) {
                        if (!fromOperation && wfCanvas.operationsList[k].itemid == fromItemid) {
                            fromOperation = wfCanvas.operationsList[k]
                        } else if (!toOperation && wfCanvas.operationsList[k].itemid == toItemId) {
                            toOperation = wfCanvas.operationsList[k]
                        }
                    }

                    if (fromOperation && toOperation){
                        flowPoints = {
                            "fromParameterIndex" : edge.fromParameter,
                            "toParameterIndex" : edge.toParameter
                        }
                        fromOperation.flowConnections.push({
                           "target" : toOperation,
                           "source" : fromOperation,
                           "attachtarget" : toOperation.index2Rectangle(edge.toRect),
                           "attachsource" : fromOperation.index2Rectangle(edge.fromRect),
                           "flowPoints" : flowPoints,
                           "isSelected" : false
                        })
                    }

                    fromOperation = false
                    toOperation = false
                }
            }
        }
        generateForm()
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

        Timer {
            interval: 30;
            running: true;
            repeat: true
            onTriggered: {
                wfCanvas.draw()
            }
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
                clear();
                canvasValid = true
                if ( workingLineBegin.x !== -1 && workingLineEnd.x !== -1){
                    ctx.beginPath();
                    ctx.lineWidth = 2;
                    ctx.moveTo(workingLineBegin.x, workingLineBegin.y);
                    ctx.strokeStyle = "red"
                    ctx.lineTo(workingLineEnd.x, workingLineEnd.y);
                    ctx.stroke();
                }
<<<<<<< HEAD
                generateForm()
=======
                for( var i=0; i < operationsList.length; i++){
                    operationsList[i].drawFlows(ctx)
                }
>>>>>>> 6a854d5e62f1246a0b596887e57d1c19f0059443
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
                currentItem = component.createObject(wfCanvas, {"x": x, "y": y, "operation" : resource,"itemid" : operationsList.length});
                if (currentItem == null) {
                    // Error Handling
                    console.log("Error creating object");
                }
                operationsList.push(currentItem)

            } else if (component.status == Component.Error) {
                // Error Handling
                console.log("Error loading component:", component.errorString());
            }
        }

        /*
     * Clear the Canvas
     */
        function clear() {
            if (ctx){
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

        function showAttachmentForm(target, attachRect){
            canvasActive = false;
            var fromOperation = operationsList[wfCanvas.currentIndex].operation
            attachementForm.operationFrom = fromOperation
            attachementForm.operationTo = target.operation
            attachementForm.attachRect = attachRect
            attachementForm.target = target
            attachementForm.state = "visible"
        }
        function showAttachmentFormFromFlow(flow) {
            canvasActive = false;
            attachementForm.operationFrom = flow.source.operation
            attachementForm.operationTo = flow.target.operation
            attachementForm.attachRect = flow.attachtarget
            attachementForm.target = flow.target
            attachementForm.source = flow.source
            attachementForm.state = "visible"
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
                if (drag.source.type === "singleoperation" || drag.source.type === "workflow") {
                    if (drag.source.type === "workflow") {
                        operations.refresh()
                    }
                    var oper = wfCanvas.getOperation(drag.source.ilwisobjectid)
                    wfCanvas.createItem(drag.x - 50, drag.y - 30,oper)
                    var paramterIndexes = workflow.addOperation(drag.source.ilwisobjectid)

                    wfCanvas.canvasValid = false

                    generateForm(paramterIndexes)
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
                if (canvasActive) {
                    wfCanvas.canvasValid = false;

                    var operationSelected = -1, highestZ = -1, smallestDistance = 100000,
                            selectedFlow = false, implicitIndexes, constantValues;

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
                            var distanceLine = distanceAC + distanceBC;


                            // Check if mouse intersects the line with offset of 10
                            if(distanceLine >= distanceAB &&
                               distanceLine < (distanceAB + wfCanvas.scale) &&
                               distanceLine - distanceAB < smallestDistance)
                            {
                                smallestDistance = distanceLine - distanceAB;
                                selectedFlow = flow;
                            }
                            flow.isSelected = false;
                        }

                        if ( isContained && item.z > highestZ ) {
                            operationSelected = i
                            highestZ = item.z
                        }
                        item.isSelected = false
                    }
                    wfCanvas.oldx = mouseX
                    wfCanvas.oldy = mouseY
                    wfCanvas.currentIndex = operationSelected
                    if (selectedFlow && operationSelected == -1) {
                        selectedFlow.isSelected = true
                    } else if (operationSelected > -1) {
                        item = wfCanvas.operationsList[operationSelected]
                        item.isSelected = true

                        implicitIndexes = workflow.implicitIndexes(operationSelected)
                        constantValues = workflow.getAsignedValuesByItemID(operationSelected)

                        if(implicitIndexes){
                            manager.showOperationFormWithHiddenFields(item, operationSelected, constantValues, implicitIndexes)
                        }else{
                            manager.showOperationForm(item, operationSelected, constantValues)
                        }

                        manager.showMetaData(item.operation)
                    } else {
                        manager.resetMetaData();
                    }
                }

            }

            onDoubleClicked: {
                var operationSelected = -1, item = 0, highestZ = 0;
                for(var i=0; i < wfCanvas.operationsList.length; ++i){

                    item = wfCanvas.operationsList[i]
                    var isContained = mouseX >= item.x && mouseY >= item.y && mouseX <= (item.x + item.width) && mouseY <= (item.y + item.height)

                    if ( isContained && item.z > highestZ ) {
                        operationSelected = i
                        highestZ = item.z
                    }
                }
                if (operationSelected > -1) {
                    var resource = mastercatalog.id2Resource(item.operation.id)
                    var filter = "itemid=" + resource.id
                    bigthing.newCatalog(filter, "workflow",resource.url,"other")
                }
            }

//            Keys.onEscapePressed: {
//                console.log("escape key");
//                wfCanvas.stopWorkingLine()
//            }

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

    Component.onDestruction: {
        var coordinates = [], node;
        for (var i = 0; i < wfCanvas.operationsList.length; i++) {
            node = wfCanvas.operationsList[i]
            coordinates[node.itemid] = node.x + '|' + node.y
        }

        workflow.store(coordinates)
    }
}
