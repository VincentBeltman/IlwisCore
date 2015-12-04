import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import "../../controls" as Controls
import "../../Global.js" as Global
import ".." as DataPanel

Rectangle {
    id : modelmanager
    width: parent.width
    height: 62

    /**
      Calls the newForm method and passes through the fields that should be hidden
      */
    function showOperationFormWithHiddenFields(operation, itemId, constantValues, hiddenFields){
        if (operation) {
            operationForm.item.showOperationFormWithHiddenFields(operation, qsTr("Set constant values"), itemId, constantValues, hiddenFields)
        }
    }

    function showOperationForm(operation, itemId, constantValues){
        if (operation) {
            operationForm.item.showForm(operation, qsTr("Set constant values "), itemId, constantValues)
        }
    }

    /**
      * Shows the run form for the whole workflow
      */
    function showRunForm(workflowid){
        if ( workflowid){
            workflowForm.item.showFormWithOutput(workflowid, qsTr("Set run values for workflow"))
        }
    }

    /**
      * Calls the execute form methods of the OperationForms class, which returns the data the user put into the form.
      */
    function retrieveRunFormValues(){
        return workflowForm.item.executeForm()
    }

    TabView{
        id : formTabs
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        function tabClicked(index){
            if ( currentIndex === index){
                if ( modelmanager.height <= 60){
                    datapane.state = "smaller"
                }
                else{
                    datapane.state = "bigger"
                }
            }

            currentIndex = index
        }


        Tab {
            id : operationForm
            active : true
            title: qsTr("Operation form")
            OperationForms{}
        }

        Tab {
            title: qsTr("Workflow form")
            id : workflowForm
            active: true
            OperationForms{}
        }

        style: DataPanel.ButtonBarTabViewStyle{
            selectColor: Global.alternateSelectedTabColor
        }
    }



}

