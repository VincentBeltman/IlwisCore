import QtQuick 2.2

Canvas{
    id : conditionContainerCanvas
    anchors.fill : parent

    Component.onCompleted: conditionItem.conditionContainerCanvas = conditionContainerCanvas
}
