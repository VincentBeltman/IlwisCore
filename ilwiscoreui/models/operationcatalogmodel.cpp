#include <future>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include <QThread>
#include "kernel.h"
#include "connectorinterface.h"
#include "resource.h"
#include "ilwisobject.h"
#include "raster.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "operationmodel.h"
#include "workspacemodel.h"
#include "uicontextmodel.h"
#include "ilwiscontext.h"
#include "operationworker.h"
#include "dataformat.h"
#include "operationcatalogmodel.h"

using namespace Ilwis;

OperationCatalogModel::OperationCatalogModel(QObject *) : CatalogModel()
{


}

QQmlListProperty<OperationsByKeyModel> OperationCatalogModel::operationKeywords()
{
    return  QQmlListProperty<OperationsByKeyModel>(this, _operationsByKey);
}

void OperationCatalogModel::nameFilter(const QString &filter)
{
    CatalogModel::nameFilter(filter);
    _currentOperations.clear();
    _operationsByKey.clear();
    emit operationsChanged();
}

quint64 OperationCatalogModel::operationId(quint32 index, bool byKey) const{
    if ( byKey){

    }
    else if ( index < _currentOperations.size()){
        return _currentOperations[index]->id().toULongLong();
    }
    return i64UNDEF;
}

quint64 OperationCatalogModel::serviceId(quint32 index) const
{
    if ( index < _services.size()){
        return _services[index].id();
    }
    return i64UNDEF;
}

QStringList OperationCatalogModel::serviceNames() const{
    QStringList names;
    for(const Resource& service : _services){
        if ( service.hasProperty("longname"))
            names.push_back(service["longname"].toString());
        else
            names.push_back(service.name());
    }
    return names;

}

QQmlListProperty<OperationModel> OperationCatalogModel::operations()
{
    try{
        if ( _currentOperations.isEmpty()) {

            gatherItems();

            _currentOperations.clear();

            std::map<QString, std::vector<OperationModel *>> operationsByKey;

            for(auto item : _currentItems){
                QString keywords = item->resource()["keyword"].toString();
                if ( item->resource().ilwisType() != itOPERATIONMETADATA)
                    continue;
                if ( keywords.indexOf("internal") != -1)
                    continue;
                _currentOperations.push_back(new OperationModel(item->resource(), this));
                if ( keywords == sUNDEF)
                    keywords = TR("Uncatagorized");
                QStringList parts = keywords.split(",");
                for(auto keyword : parts){
                    operationsByKey[keyword].push_back(new OperationModel(item->resource(), this));
                }
            }
            for(auto operation : operationsByKey){
                _operationsByKey.push_back(new OperationsByKeyModel(operation.first, operation.second, this));
            }

        }

        return  QMLOperationList(this, _currentOperations);
    }
    catch(const ErrorObject& err){

    }
    return  QMLOperationList();
}
void OperationCatalogModel::prepare(){
    _refresh  = true;
    gatherItems();
}

void OperationCatalogModel::gatherItems() {
    if (!_refresh)
        return;

    WorkSpaceModel *currentModel = uicontext()->currentWorkSpace();
    bool isDefault = false;
    if (currentModel){
        auto n = currentModel->name();
        isDefault = n == "default";
    }
    if ( currentModel == 0 || isDefault){
        if ( !_view.isValid()){
            QUrl location("ilwis://operations");
            QString descr ="main catalog for ilwis operations";
            Resource res(location, itCATALOGVIEW ) ;
            res.name("ilwis-operations",false);
            QStringList lst;
            lst << location.toString();
            res.addProperty("locations", lst);
            res.addProperty("type", "operation" );
            res.addProperty("filter",QString("type=%1").arg(itOPERATIONMETADATA));
            res.setDescription(descr);
            setView(CatalogView(res));

            location = QUrl("ilwis://operations");
            descr ="main catalog for ilwis services";
            res = Resource(location, itCATALOGVIEW ) ;
            res.name("ilwis-services",false);
            lst.clear();
            lst << location.toString();
            res.addProperty("locations", lst);
            res.addProperty("type", "operation" );
            res.addProperty("filter",QString("type=%1 and keyword='service'").arg(itOPERATIONMETADATA));
            res.setDescription(descr);
            CatalogView view(res);
            view.prepare();

            _services = view.items();
        }
    }else {
        setView(currentModel->view());
    }
    CatalogModel::gatherItems();
    std::set<QString> keywordset;
    for(auto item : _currentItems){
        QString keywords = item->resource()["keyword"].toString();
        if ( item->resource().ilwisType() != itOPERATIONMETADATA)
            continue;
        if ( keywords.indexOf("internal") != -1)
            continue;
        _currentOperations.push_back(new OperationModel(item->resource(), this));
        if ( keywords == sUNDEF)
            keywords = TR("Uncatagorized");
        QStringList parts = keywords.split(",");
        for(auto keyword : parts){
            keywordset.insert(keyword);
        }
    }
    for(auto keyword : keywordset)
        _keywords.push_back(keyword);

    qSort(_keywords.begin(), _keywords.end());

    _keywords.push_front(""); // all
}

QStringList OperationCatalogModel::keywords() const
{
    return _keywords;
}

void OperationCatalogModel::workSpaceChanged()
{
    _currentItems.clear();
    _currentOperations.clear();
    _operationsByKey.clear();
    _services.clear();
    _refresh = true;

    emit operationsChanged();
}

QString OperationCatalogModel::executeoperation(quint64 operationid, const QString& parameters) {
    if ( operationid == 0 || parameters == "")
        return sUNDEF;

    Resource operationresource = mastercatalog()->id2Resource(operationid);
    if ( !operationresource.isValid())
        return sUNDEF;

    QString expression;
    QStringList parms = parameters.split("|");

    for(int i = 0; i < parms.size() - 1; ++ i){ // -1 because the last is the output parameter
        if ( expression.size() != 0)
            expression += ",";
        expression += parms[i];
    }
    QString output = parms[parms.size() - 1];
    QString format="{format(ilwis,\"stream\")}";
    if ( output.indexOf("@@") != -1 ){
        QStringList parts = output.split("@@");
        output = parts[0];
        QString formatName = parts[1];
        if ( formatName != "Memory"){ // special case
            QString query = "name='" + formatName + "'";
            std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, query);
            if ( formats.size() == 1){
                format = "{format(" + (*formats.begin()).second.property(DataFormat::fpCONNECTOR).toString() + ",\"" +
                        (*formats.begin()).second.property(DataFormat::fpCODE).toString() + "\")}";
            }
            output = context()->workingCatalog()->source().url().toString() + "/" + output + format;
        }else{
            IlwisTypes outputtype = operationresource["pout_1_type"].toULongLong();
            if ( outputtype == itRASTER)
                format = "{format(stream,\"rastercoverage\")}";
            else if (hasType(outputtype, itFEATURE))
                format = "{format(stream,\"featurecoverage\")}";
            else if (hasType(outputtype, itTABLE))
                format = "{format(stream,\"table\")}";
            output = output + format;
        }
    }else {
        output = output + format;
    }
    expression = QString("script %1=%2(%3)").arg(output).arg(operationresource.name()).arg(expression);
    qDebug() << expression;

    OperationExpression opExpr(expression);



    try {
        QThread* thread = new QThread;
        OperationWorker* worker = new OperationWorker(opExpr);
        worker->moveToThread(thread);
        thread->connect(thread, &QThread::started, worker, &OperationWorker::process);
        thread->connect(worker, &OperationWorker::finished, thread, &QThread::quit);
        thread->connect(worker, &OperationWorker::finished, worker, &OperationWorker::deleteLater);
        thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();


    return "TODO";
    } catch (const ErrorObject& err){
        emit error(err.message());
    }
    return sUNDEF;

}





