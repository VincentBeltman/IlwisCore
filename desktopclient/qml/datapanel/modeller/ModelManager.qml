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
    Component {
        id : modelLayerOptions
        LayerManagement{}
    }

    Component {
        id : operationForms
        OperationForms{}
    }

    Component {
        id : runForms
        OperationForms{}
    }

    Component{
        id : metadata
        MetaData{
        }
    }

    function showOperationForm(operationid){
        if ( operationid){
            datapane.state = "smaller"
            var tab= modellerProperties.getTab(1)
            tab.active=true
            tab.item.newForm(operationid, qsTr("Set default values"))
        }
    }

    /**
      * Shows the run form for the whole workflow
      */
    function showRunForm(workflowid){
        if ( workflowid){
            datapane.state = "smaller"
            var tab= modellerProperties.getTab(2)
            tab.active=true
            tab.item.newForm(workflowid, qsTr("Set run values"))


        }
    }

    /**
      Calls the execute form methods of the OperationForms class, which returns the data the user put into the form.
      */
    function retrieveRunFormValues(workflowid){
        if(workflowid){
            var tab= modellerProperties.getTab(2)
            return tab.item.executeForm()
        }
    }

    TabView{
        id : modellerProperties
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        function tabClicked(index){
            if ( currentIndex === index){
                if ( modelmanager.height <= 60){
                    datapane.state = "smaller"
                }
                else{
                    datapane.state = ""
                    datapane.state = "bigger"
                }
            }

            currentIndex = index
        }


        Component.onCompleted: {
            var tab =addTab(qsTr("Model layers"), modelLayerOptions)
            tab.active = true

            addTab(qsTr("Operation Form"), operationForms)
            addTab(qsTr("Workflow Form"), runForms)
            tab = addTab(qsTr("Metadata"), metadata)
            tab.active = true // we need to be active as layers maybe added to it
        }

        style: DataPanel.ButtonBarTabViewStyle{}
    }



}

