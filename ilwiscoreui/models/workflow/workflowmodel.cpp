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
void WorkflowModel::createMetadata()
{
    _workflow->createMetadata();
}

/**
 * Runs all the operations in the workflow and generates output
 * @param inputAndOuput the input and output parameters that the user filled in
 */
void WorkflowModel::run(const QString &inputAndOuput)
{
    try{
        QStringList inputOutputList = inputAndOuput.split("|");

        _workflow->createMetadata();

        ExecutionContext ctx;
        SymbolTable symbolTable;
        QString outputs;
        QString inputs;

        QStringList outputList;

        //Loop through all input and output parameters
        for(int i=0 ;i<inputOutputList.size(); ++i) {
            //Add to execute string if its not an output parameter
            if(i < inputOutputList.size()-_workflow->outputParameterCount()){
                //Check if its not the first input parameter
                if(!inputs.isEmpty()){
                    inputs.append(",");
                }

                inputs.append(inputOutputList[i]);
            }else{
                //Check if its not the first output parameter
                if(!outputs.isEmpty()){
                    outputs.append(",");
                }

                //Put the outputs in a list (these will be used later)
                outputList.push_back(inputOutputList[i]);

                QStringList filenameAndFormat = outputList[i-_workflow->inputParameterCount()].split("@@");

                outputs.append(filenameAndFormat[0]);
            }
        }

        QString executeString = QString("%1=%2(%3)").arg(outputs).arg(_workflow->name()).arg(inputs);

        bool ok = commandhandler()->execute(executeString, &ctx, symbolTable);
        if (!ok) {
            //TODO show alert if the expresion did not execute.
            qDebug() << "Fail";
        }

        for(int i=0;i<outputList.size();++i){
            QStringList filenameAndFormat = outputList[i].split("@@");

            Symbol actual = symbolTable.getSymbol(QString("%1").arg(filenameAndFormat[0]));

            QString format;
            //Check which type of output is generated
            if(actual._type & itRASTER){
                format = "rastercoverage";
            }else if(actual._type & itFEATURE){
                format = "featurecoverage";
            }else if(actual._type & itTABLE){
                format = "table";
            }
            //TODO add more else ifs with more formats

            //TODO show alert if format isnt supported
            if(format != NULL){
                if(actual.isValid()){

                    QString outputName = "ilwis://internalcatalog/" + filenameAndFormat[0];

                    Ilwis::IIlwisObject object;
                    object.prepare(outputName ,{"mustexist",true});

                    //TODO do something with the type(memory, arc, jpeg etc.)

                    QUrl url = object->source().url(true).adjusted(QUrl::RemoveFilename).toString() + filenameAndFormat[0];

                    //Generate the stream
                    object->connectTo(url, format,"stream",Ilwis::IlwisObject::cmOUTPUT);

                    object->createTime(Ilwis::Time::now());
                    object->store({"storemode",Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA});
                }
            }
        }
    } catch (const ErrorObject& err){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}
