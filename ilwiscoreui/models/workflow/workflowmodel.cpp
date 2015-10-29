#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "featurecoverage.h"
#include "../../IlwisCore/core/ilwiscontext.h"

using namespace Ilwis;
using namespace boost;

WorkflowModel::WorkflowModel()
{
}

WorkflowModel::WorkflowModel(const Ilwis::Resource &source, QObject *parent) : OperationModel(source, parent)
{
    _workflow.prepare(source);
}

void WorkflowModel::asignConstantInputData(int operationIndex, int parameterIndex, QVariant value) {
    OVertex vertex = _operationNodes[operationIndex];
    SPAssignedInputData constantInput = _workflow->assignInputData(vertex, parameterIndex);
    constantInput->value = value;
}

void WorkflowModel::addOperation(const QString &id)
{
    bool ok;
    quint64 opid = id.toULongLong(&ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("Invalid operation id used in workflow %1")).arg(name()));
        return ;
    }
    auto vertex = _workflow->addOperation({opid});
    _operationNodes.push_back(vertex);

}

void WorkflowModel::addFlow(int operationIndex1, int operationIndex2, const QVariantMap& flowpoints)
{
    if ( operationIndex1 >= 0 && operationIndex2 >= 0 && flowpoints.size() == 2) {
        try {
            const OVertex& fromOperationVertex = _operationNodes[operationIndex1];
            const OVertex& toOperationVertex = _operationNodes[operationIndex2];
            int outParamIndex = flowpoints["fromParameterIndex"].toInt();
            int inParamIndex = flowpoints["toParameterIndex"].toInt();
            EdgeProperties flowPoperties{outParamIndex, inParamIndex};
            _workflow->addOperationFlow(fromOperationVertex,toOperationVertex,flowPoperties);
        } catch (std::out_of_range e) {
           qDebug() << "False operation";
        }
    }
}

bool WorkflowModel::hasValueDefined(int operationindex, int parameterindex){
    try {
        const OVertex& operationVertex = _operationNodes[operationindex];
        return _workflow->hasValueDefined(operationVertex, parameterindex);
    } catch (std::out_of_range e) {
       return false;
    }
}

void WorkflowModel::deleteOperation(int index)
{
    try {
        if ( index < _operationNodes.size()){
            const OVertex& operationVertex = _operationNodes[index];
            _workflow->removeOperation(operationVertex);
            _operationNodes.erase(_operationNodes.begin() + index);
        }
    } catch (std::out_of_range e) {
        qDebug() << "False operation";
    }
}

void WorkflowModel::deleteFlow(int operationIndex1, int operationIndex2, int indexStart, int indexEnd)
{
    OVertex sourceNode = _operationNodes[operationIndex1];
    boost::graph_traits<WorkflowGraph>::out_edge_iterator ei, ei_end;
    for (boost::tie(ei,ei_end) = _workflow->getOutEdges(sourceNode); ei != ei_end; ++ei) {

        OVertex targetNode = _workflow->getTargetOperationNode(*ei);

        NodeProperties npNode = _workflow->nodeProperties(targetNode);
        NodeProperties npTarget = _workflow->nodeProperties(_operationNodes[operationIndex2]);

        if(npNode._operationid == npTarget._operationid)
        {
            EdgeProperties ep = _workflow->edgeProperties(*ei);

            if(ep._outputIndexLastOperation == indexStart && ep._inputIndexNextOperation == indexEnd)
            {
                _workflow->removeOperationFlow(*ei);
            }
        }
    }
}

/**
 * Runs the createMetadata function on the workflow.
 * The workflow will be put in the master catalog and will be usable.
 */
quint64 WorkflowModel::createMetadata() {
    return _workflow->createMetadata();
}
