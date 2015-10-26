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

    function newForm(metaid, title){
        newFormWithOutput(metaid,title,false)
    }

    /**
      Shows the operation's form. A boolean (showOutput) has to be passed to this method which decides whether an ouput form is shown.
      */
    function newFormWithOutput(metaid, title, showOutput){
        operationid = metaid
        var form= formbuilder.index2Form(metaid, showOutput)
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

    Bench.ApplicationForm{
        id : appFrame
        width : parent.width/2
        x : parent.width/4
        height : parent.height - 30 < 0 ?  0 : parent.height - 30
        opacity : 0

    }
    Button {
        id : pushThrough
        text: "GOOGGO"
        onClicked: {
            console.log("trol", appFrame.currentAppForm.formresult)
        }
    }

//    Bench.ApplicationForm{
//        id : workflowFrame

//        width : parent.width / 2
//        height : parent.height - 30 < 0 ?  0 : parent.height - 30
//        opacity : 0

//    }
}

