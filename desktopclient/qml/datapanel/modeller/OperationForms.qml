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
    function showRunForm(metaid, operationNames) {
        var form= formbuilder.index2WorkflowForm(metaid, operationNames)
        operationid = metaid
        appFrame.formQML = form
        appFrame.formTitle = qsTr("Set run values for workflow")
        appFrame.opacity = 1
    }

    function showForm(item, title, newItemId, constantValues){
        fillAppFrame(item.operation.id, title + "(" + item.getTitle() + ")", false, true, constantValues, [])
        itemId = newItemId
    }

    /**
      Shows the operation's form. Passes the hidden fields to the index2Form method.
      */
    function showOperationFormWithHiddenFields(item, title, newItemId, constantValues, hiddenFields){
        fillAppFrame(item.operation.id, title + "(" + item.getTitle() + ")", false, true, constantValues, hiddenFields)
        itemId = newItemId
    }

    /**
      Shows the operation's form. A boolean (showOutput) has to be passed to this method which decides whether an ouput form is shown.
      */
    function showFormWithOutput(metaid, title){
        fillAppFrame(metaid, title, true, false, [], [])//TODO
    }

    function fillAppFrame(metaid, title, output, showEmpty, constantValues, hiddenFields) {
        var form= formbuilder.index2Form(metaid, output, showEmpty ,hiddenFields)
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
        anchors.fill: parent

        Bench.ApplicationForm{
            id : appFrame
            width : operationForm.width - 20
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

