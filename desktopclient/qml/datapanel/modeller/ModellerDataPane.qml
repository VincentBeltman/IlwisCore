import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import TabModel 1.0
import WorkflowModel 1.0
import ScenarioBuilderModel 1.0
import ScenarioDesignerModel 1.0
import "./workflow" as WorkFlow

Item {
    id: modellerDataPane
    width : parent.width
    height : parent.height
    property TabModel tabmodel
    property ScenarioDesignerModel scenario
    property int ontTopZValue: 1000000
    property double factor : 1.1

    function addDataSource(filter, sourceName, sourceType){
        if ( filter !== "" ){
            if (sourceType === "workflow") {
                scenario = scenarios.create()

                var resource = mastercatalog.id2Resource(filter.split('=')[1]);
                canvas.workflow = scenario.addWorkflow(filter);
                if (resource) {
                    canvas.workflow.load()
                    canvas.drawFromWorkflow()
                }
            }
        }
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
    }



    function datapaneChanged(index){
        if ( index == 0) {
            defview.state = "visible"
            canvas.state = "invisible"
            operview.state = "invisible"
            templateBuilder.state = "invisible"

        }else if ( index == 1){
            defview.state = "invisible"
            canvas.state = "invisible"
            operview.state = "visible"
            templateBuilder.state = "invisible"
        } else if ( index == 2){
            defview.state = "invisible"
            canvas.state = "invisible"
            operview.state = "invisible"
            templateBuilder.state = "visible"
        }else{
            defview.state = "invisible"
            canvas.state = "visible"
            operview.state = "invisible"
            templateBuilder.state = "invisible"
        }
    }

    function deleteSelectedOperation(){
        canvas.deleteSelectedOperation()
    }

    function deleteSelectedEdge(){
        canvas.deleteSelectedEdge()
    }

    function alterSelectedEdge(){
        canvas.alterSelectedEdge()
    }

    function canvasZoomOut(){
        scaleCanvas(1/factor);
    }

    function canvasZoomIn(){
        scaleCanvas(factor);
    }

    /**
    Sets the canvas' zoom back to 100%
    */
    function defaultZoom(){
        tform.xScale = 1
        tform.yScale = 1

        canvas.height = canvas.parent.height
        canvas.width = canvas.parent.width

        setPercentage()
    }

    function scaleCanvas(scaleFactor){
        canvas.height /= scaleFactor;
        canvas.width /= scaleFactor;
        tform.xScale *=scaleFactor;
        tform.yScale *=scaleFactor;

        setPercentage()
    }

    /**
    Sets the zoom percentage based on the tform's xScale
    */
    function setPercentage(){
        modellertools.zoomLevel.text = Math.round((tform.xScale *100)) + "%"
    }

    function asignConstantInputData(inputData, operationid) {
        canvas.asignConstantInputData(inputData, operationid);
    }

    /**
      Calls the WorkflowCanvas's run method
      */
    function run() {
        canvas.run()
    }

    function addError(id, error) {
        errorview.addError(id, error)
    }

    signal exit;

    property bool canSeparate : true
    property string panelType : "workflow"


    ModellerTools{
        id : modellertools
    }

    SplitView {
        anchors.top : modellertools.bottom
        width : parent.width
        orientation: Qt.Vertical
        height : parent.height - modellertools.height

        ModellerErrorView {
            height: 0
            id : errorview
            width : parent.width
            state: "smaller"
            y: 45
            z: ontTopZValue
            visible: false

            states: [
                State {
                    name : "bigger"
                    PropertyChanges {
                        target: errorview
                        height : 80
                        visible: true
                    }
                    PropertyChanges {
                        target: datapane
                        height : parent.height - modellertools.height - 170 - 80
                    }
                },
                State { name: "smaller"

                    PropertyChanges {
                        target: errorview
                        height : 0
                        visible: false
                    }
                }
            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]
        }

        Item {
            id : datapane
            width : parent.width
            height : parent.height - 170

            function asignConstantInputData(vertexIndex, parameterIndex, value){
                canvas.asignConstantInputData(vertexIndex, parameterIndex, value)
            }



            WorkFlow.WorkflowCanvas {
                id: canvas
                state : "visible"
                transform : Scale{
                    id : tform
                }
                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    onWheel: {
                        if(wheel.angleDelta.y > 0){  // zoom in
                            modellerDataPane.canvasZoomIn();
                        }
                        else{                        // zoom out
                            modellerDataPane.canvasZoomOut();
                        }
                    }
                    onPressed:{mouse.accepted = false}
                    onPositionChanged:{mouse.accepted = false}
                    onReleased:{mouse.accepted = false}
                }
            }


            ModellerDefinitionView{ id : defview}
            ModellerTemplateBuilder{ id : templateBuilder}
            ModellerOperationalView{ id : operview}

            states: [
                State { name: "smaller"

                    PropertyChanges {
                        target: datapane
                        height : parent.height - 170
                    }
                },
                State {
                    name : "bigger"
                    PropertyChanges {
                        target: datapane
                        height : parent.height - 10
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation { properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
                }
            ]
        }

        ModelManager{
            id : manager
            height : 170
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
