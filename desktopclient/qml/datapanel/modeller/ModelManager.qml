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
        modellerProperties.getTab(1)
        forms.item.showOperationFormWithHiddenFields(operation, itemId, hiddenFields)
    }

    function showOperationForm(operation, itemId){
        modellerProperties.getTab(1)
        forms.item.showOperationForm(operation, itemId)
    }

    function showRunForm(workflowid){
        modellerProperties.getTab(1)
        forms.item.showRunForm(workflowid)
    }

    function retrieveRunFormValues(){
        return forms.item.retrieveRunFormValues()
    }

    function showMetaData(item){
        var tab = modellerProperties.getTab(2)
        tab.item.setDesc(item.description)
        tab.item.setName(item.syntax)
        tab.item.setKeywords(item.keywords)
    }

    function resetMetaData(workflow){
        var tab = modellerProperties.getTab(2)
        tab.item.setDesc(workflow.description)
        tab.item.setName(workflow.name)
        tab.item.setKeywords(workflow.keywords)
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
            id : metadataComp
            active: true
            title: "Metadata"
            MetaData{}
        }

        style: DataPanel.ButtonBarTabViewStyle{}
    }



}

