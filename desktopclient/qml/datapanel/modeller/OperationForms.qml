import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../workbench" as Bench
import "../../Global.js" as Global

Rectangle {
    id : operationForm
    width : 210
    height : parent.height
    visible : height > 0
    property var operationid
    property int itemId: -1

    /**
    Create a form for the workflow
    */
    function showRunForm(metaid, operationNames, parameterIndexes) {
        var validValues = [], parameterindex, action;

        if (appFrame.currentAppForm != null) {
            validValues = appFrame.currentAppForm.formresult.split('|')
            for (var i = 0; i < parameterIndexes.length; i++) {
                parameterindex = parameterIndexes[i].split('|');
                action = parameterindex[1];
                parameterindex = parameterindex[0];

                if (action === 'insert') {
                    validValues.splice(parameterindex, 0, "")
                } else if (action === 'remove') {
                    validValues.splice(parameterindex, 1)
                }
            }
            validValues = validValues.slice(0, canvas.workflow.getInputParameterCount())
        }

        var form = formbuilder.index2Form(metaid, true, false, "", operationNames, validValues)
        operationid = metaid
        appFrame.formQML = form
        appFrame.formTitle = qsTr("Set run values for workflow")
        appFrame.opacity = 1
        //canvas.workflow.resetParameterEntrySet()
    }

    function showForm(item, title, newItemId, constantValues){
        fillAppFrame(item.operation.id, title + "(" + item.getTitle() + ")", false, true, "", constantValues)
        itemId = newItemId
    }

    /**
      Shows the operation's form. Passes the hidden fields to the index2Form method.
      */
    function showOperationFormWithHiddenFields(item, title, newItemId, constantValues, hiddenFields){
        fillAppFrame(item.operation.id, title + "(" + item.getTitle() + ")", false, true, hiddenFields, constantValues)
        itemId = newItemId
    }

    function fillAppFrame(metaid, title, output, showEmpty, hiddenFields, constantValues) {
        var form= formbuilder.index2Form(metaid, output, showEmpty, hiddenFields, {}, constantValues)
        operationid = metaid
        appFrame.formQML = ""
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.opacity = 1
    }

    /**
      Executes the form
      */
    function executeForm() {
        appFrame.doExecute(operationid)
        return appFrame.currentAppForm.formresult
    }
    ScrollView{
        id: operationFormScrollView
        width: parent.width
        height: itemId > -1 ? parent.height - 30 : parent.height

        Bench.ApplicationForm{
            id : appFrame
            width : operationForm.width - 20
            height : operationForm.height - 30 < 0 ?  0 : operationForm.height - 30
            opacity : 0
        }
    }
    Button {
        id : saveConstantInputButton
        height : itemId > -1 ? 25 : 0
        width : 70
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: operationFormScrollView.bottom

        Image {
            height : parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: saveConstantInputText.left

            source : iconsource("save20.png")
        }
        Text {
            height : parent.height
            id : saveConstantInputText
            text: 'Save'
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        onClicked: {
            asignConstantInputData()
        }
    }

    function asignConstantInputData() {
        if (itemId > -1){
            modellerDataPane.asignConstantInputData(appFrame.currentAppForm.formresult, itemId)
        }
    }
}

