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
    property var itemId

    function newForm(metaid, title, newItemId){
        operationid = metaid
        itemId = newItemId
        var form= formbuilder.index2Form(metaid, false, true)
        appFrame.formQML = form
        appFrame.formTitle = title
        appFrame.opacity = 1
//        appFrame.formResultChanged.connect(asignConstantInputData)
    }

    /**
      Executes the form
      */
    function executeForm() {
        appFrame.doExecute(operationid)
        return appFrame.currentAppForm.formresult
    }

    Bench.ApplicationForm{
        id : appFrame
        width : parent.width
        height : parent.height - 30 < 0 ?  0 : parent.height - 30
        opacity : 0
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

