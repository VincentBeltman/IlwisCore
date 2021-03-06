#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "drawers/drawerinterface.h"
#include "drawers/draweroperation.h"
#include "../rootdrawer.h"
#include "../layersview.h"
#include "showpolygonareas.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(ShowPolygonAreas)

ShowPolygonAreas::ShowPolygonAreas()
{

}

ShowPolygonAreas::~ShowPolygonAreas()
{

}

ShowPolygonAreas::ShowPolygonAreas(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool ShowPolygonAreas::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;
        RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);

        if ( _index != iUNDEF){
            UPDrawer& drawer = rootdrawer->drawer(_index, _type);
            drawer->setAttribute("polygonareas", _areaVisibility);
        }else if ( _code != sUNDEF){
            UPDrawer& drawer = rootdrawer->drawer(_code, _type)    ;
            drawer->setAttribute("polygonareas", _areaVisibility);
        }
        rootdrawer->redraw();
        return true;
    }catch(const VisualizationError& err){
    }
    return false;
}

Ilwis::OperationImplementation *ShowPolygonAreas::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ShowPolygonAreas(metaid, expr)    ;
}

Ilwis::OperationImplementation::State ShowPolygonAreas::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if ( (_rootDrawer = getRootDrawer()) == 0){
        return sPREPAREFAILED;
    }

    QString type = _expression.parameterCount() == 3 ? "main" : _expression.input<QString>(3);

    bool ok;
    int index = _expression.parm(1).value().toInt(&ok);
    if ( ok){
        if ( type == "main" && index >= 0 && index < _rootDrawer->attribute("maindrawercount").toInt()){
            _index = index;
        }else {
            _index = index;
        }
    }else {
        _code = _expression.input<QString>(1);
    }

    if ( type == "main")
        _type = DrawerInterface::dtMAIN;
    else if ( type == "post")
        _type = DrawerInterface::dtPOST;
    else if ( type == "pre"){
        _type = DrawerInterface::dtPRE;
    }else{
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"), type);
        return sPREPAREFAILED;
    }

    _areaVisibility = _expression.input<bool>(2);

    return sPREPARED;
}

quint64 ShowPolygonAreas::createMetadata()
{
    OperationResource operation({"ilwis://operations/ShowPolygonAreas"});
    operation.setSyntax("ShowPolygonAreas(viewid, drawername-index,visibility=!true | false[,type])");
    operation.setDescription(TR("sets or resets the visibility of a layer"));
    operation.setInParameterCount({3,4});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING|itINTEGER , TR("layer index/code"), TR("location of the layer to be used. In the case of pre or post drawers the index is the order number"));
    operation.addInParameter(2,itBOOL , TR("visibility"));
    operation.addOptionalInParameter(3,itSTRING , TR("drawer type"), TR("the type fo drawer, pre, main, or post. The default is main"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}


