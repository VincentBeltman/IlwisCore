#include "nodepropobject.h"

//using namespace Ilwis;

NodePropObject::NodePropObject(QObject *parent) :
    QObject(parent)
{
}

void NodePropObject::setProps(NodeProperties props, OVertex v) {
    _x = props._x;
    _y = props._y;
    _vertex = v;
    _operationId = props._operationid;
}
