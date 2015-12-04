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
        modellerProperties.getTab(1)
        forms.item.showOperationFormWithHiddenFields(operation, itemId, constantValues, hiddenFields)
    }

    function showOperationForm(operation, constantValues, itemId){
        modellerProperties.getTab(1)
        forms.item.showOperationForm(operation, itemId, constantValues)
    }

    function showRunForm(workflowid){
        modellerProperties.getTab(1)
        forms.item.showRunForm(workflowid)
    }

    function retrieveRunFormValues(){
        return forms.item.retrieveRunFormValues()
    }

    function showMetaData(item) {
        metadata.item.showMetaData(item)
    }

    function resetMetaData() {
        metadata.item.resetMetaData()
    }

    function showWorkflowMetadata(workflow) {
        metadata.item.showWorkflowMetaData(workflow)
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
                    datapane.state = "bigger"
                }
            }

            currentIndex = index
        }


        Tab {
            id : modelLayerOptions
            title: "Model layers"
            active: true
            LayerManagement{}
        }

        Tab {
            id : forms
            active: true
            title: "Input form"

            FormsTab{}
        }

        Tab{
            id : metadata
            active: true
            title: "Metadata"
            MetaDataTab{}
        }

        style: DataPanel.ButtonBarTabViewStyle{}
    }



}

