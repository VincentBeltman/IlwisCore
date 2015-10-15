#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "operationmetadata.h"
#include "workflowmodel.h"
#include "symboltable.h"
#include "commandhandler.h"

using namespace Ilwis;
using namespace boost;

WorkflowModel::WorkflowModel()
{
}

WorkflowModel::WorkflowModel(const Ilwis::Resource &source, QObject *parent) : OperationModel(source, parent)
{
    _workflow.prepare(source);
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
//    Workflow.removeOperationFlow();

//    _workflow.removeOperationFlow();

//     for(auto iter = _flows.begin(); iter != _flows.end(); ++iter){
//         if ( (*iter)._beginOperation == operationIndex1 && (*iter)._endOperation == operationIndex2){
//             if ( (*iter)._inParam == indexStart && (*iter)._outParam == indexEnd){
//                _flows.erase(iter);
//                return;
//             }
//         }
//     }
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
 * @param input the input parameters that will be passed to the workflow
 */
void WorkflowModel::run(const QString &input)
{
    try{
        QStringList inputList = input.split("|");

        _workflow->createMetadata();

        ExecutionContext ctx;
        SymbolTable symbolTable;
        QString executeString = QString("%1_out=%2(").arg(_workflow->name()).arg(_workflow->name());

//        Loop through all input parameters and add them to the execute string
        for(int i=0 ;i<inputList.size(); ++i) {
            if(inputList[i]!=""){
                executeString.append(inputList[i]);

                if(i!= (inputList.size()-1)){
                    executeString.append(",");
                }
            }
        }
        executeString.append(")");

        qDebug() << executeString;

        bool ok = commandhandler()->execute(executeString, &ctx, symbolTable);
        if (!ok) {
            qDebug() << "Fail";
        }

        Symbol actual = symbolTable.getSymbol(QString("%1_out").arg(_workflow->name()));

        if(actual.isValid()){
            if(actual._type & itCOVERAGE){
                Ilwis::IRasterCoverage raster;
                raster.prepare("ilwis://internalcatalog/" + _workflow->name() + "_out" ,{"mustexist",true});

                qDebug() << "write output result to " << raster->source().url().toString();

                QUrl url;

//                Generate the stream
                raster->connectTo(url, "rastercoverage","stream",Ilwis::IlwisObject::cmOUTPUT);
//                raster->connectTo(url, "GTiff","gdal",Ilwis::IlwisObject::cmOUTPUT); //generate tiff
                raster->createTime(Ilwis::Time::now());
//                raster->store();
                raster->store({"storemode",Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA});
            }
        }
    } catch (const ErrorObject& err){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}
