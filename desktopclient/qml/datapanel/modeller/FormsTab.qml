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
    function showOperationFormWithHiddenFields(operation, itemId, hiddenFields){
        if (operation) {
            datapane.state = "smaller"
            var tab = formTabs.getTab(0)
            tab.item.newOperationFormWithHiddenFields(operation, qsTr("Set constant values"), itemId,hiddenFields)
        }
    }

    function showOperationForm(operation, itemId){
        if (operation) {
            datapane.state = "smaller"
            var tab = formTabs.getTab(0)
            tab.item.newForm(operation, qsTr("Set constant values "), itemId)
        }
    }

    /**
      * Shows the run form for the whole workflow
      */
    function showRunForm(workflowid){
        if ( workflowid){
            datapane.state = "smaller"
            var tab = formTabs.getTab(1)
            tab.active = true
            tab.item.newFormWithOutput(workflowid, qsTr("Set run values for workflow"))
        }
    }

    /**
      * Calls the execute form methods of the OperationForms class, which returns the data the user put into the form.
      */
    function retrieveRunFormValues(){
        var tab = modellerProperties.getTab(3)
        return tab.item.executeForm()
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

