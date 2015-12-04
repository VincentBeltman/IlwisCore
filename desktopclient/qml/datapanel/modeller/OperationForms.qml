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
    property var operationid
    property int itemId: -1

    /**
    Create a form for the workflow
    */
    function newWorkflowForm(metaid, title, operationNames){
        var form= formbuilder.index2WorkflowForm(metaid, operationNames)
        operationid = metaid
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.opacity = 1
    }

    function newForm(metaid, title){
        fillAppFrame(metaid, title, true, false, [])
    }

    /**
      Shows the operation's form. Passes the hidden fields to the index2Form method.
      */
    function newOperationFormWithHiddenFields(metaid, title, newItemId, hiddenFields){
        fillAppFrame(metaid, title, false, true, hiddenFields)
        itemId = newItemId
    }

    /**
      Shows the operation's form. A boolean (showOutput) has to be passed to this method which decides whether an ouput form is shown.
      */
    function newFormWithOutput(metaid, title){
        fillAppFrame(metaid, title, true, false, [])
    }

    function fillAppFrame(metaid, title, output, showEmpty, hiddenFields) {
        var form= formbuilder.index2Form(metaid, output, showEmpty ,hiddenFields)
        operationid = metaid
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
        anchors.fill: parent

        Bench.ApplicationForm{
            id : appFrame
            width : operationForm.width - 15
            height : operationForm.height - 30 < 0 ?  0 : operationForm.height - 30
            opacity : 0
        }
    }
    Component.onCompleted: {
        appFrame.formResultChanged.connect(asignConstantInputData)
    }
    function asignConstantInputData() {
        if (itemId > -1){
            modellerDataPane.asignConstantInputData(appFrame.currentAppForm.formresult, itemId)
        }
    }
}

