#include <QString>
#include <QStringList>
#include "kernel.h"
#include "factory.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "catalogexplorer.h"
#include "connectorfactory.h"


using namespace Ilwis;

std::vector<createCatalogExplorer> ConnectorFactory::_explorers;

uint Ilwis::qHash(const ConnectorFilter& filter ){

    return ::qHash(filter._provider);
}

bool Ilwis::operator==(const ConnectorFilter& filter1, const ConnectorFilter& filter2 ){
    bool typeOk = (filter1._objectTypes & filter2._objectTypes) != 0;
    //bool providerOk = filter1._provider == filter2._provider;
    QRegExp regexpr(filter1._provider,Qt::CaseSensitive, QRegExp::Wildcard);
    bool providerOk = regexpr.indexIn(filter2._provider) != -1;
    return  typeOk && providerOk;
}

inline bool Ilwis::operator<(const ConnectorFilter& filter1, const ConnectorFilter& filter2 ){
    return filter1._provider < filter2._provider;
}
//---------------------------------------------------------------
uint Ilwis::qHash(const ConnectorFormatSelector& filter ){

    return ::qHash(filter._format);
}

bool Ilwis::operator==(const ConnectorFormatSelector& filter1, const ConnectorFormatSelector& filter2 ){
    bool ok = filter1._format == filter2._format;
    if (  filter1._provider == sUNDEF || filter2._provider == sUNDEF)
        return ok;
     return  ok && filter1._provider == filter2._provider;
}


inline bool Ilwis::operator<(const ConnectorFormatSelector& filter1, const ConnectorFormatSelector& filter2 ){
    return filter1._provider < filter2._provider;
}

ConnectorFactory::ConnectorFactory() : AbstractFactory("ConnectorFactory","ilwis","Creates all object connectors" )
{
}

void ConnectorFactory::addCreator(quint64 objecttypes, const QString& provider, ConnectorCreate func)
{
    if ( objecttypes == 0 || func == 0) {
        kernel()->issues()->log(TR(ERR_INVALID_INIT_FOR_2).arg("parameters","Connector Factory"));
        return;
    }
    ConnectorFilter filter(objecttypes, provider);
    if (!_creatorsPerObject.contains(filter)) {
        _creatorsPerObject.insert(filter, func );
    }
}

void ConnectorFactory::addCreator(const QString& format,const QString &provider,  ConnectorCreate func){
    if ( provider == "" || format == "" || provider == sUNDEF || format == sUNDEF){
        kernel()->issues()->log(TR(ERR_INVALID_INIT_FOR_2).arg("parameters","Connector Factory"));
        return;
    }
    ConnectorFormatSelector filter(format, provider);
    if (!_creatorsPerFormat.contains(filter)){
        _creatorsPerFormat.insert(filter, func);
    }
}

std::nullptr_t ConnectorFactory::registerCatalogExplorer(createCatalogExplorer func)
{
    _explorers.push_back(func);

    return nullptr;
}

std::vector<CatalogExplorer*> ConnectorFactory::explorersForResource(const Resource& resource, const QString &provider) const{
   std::vector<CatalogExplorer*> explorers;
   for( createCatalogExplorer createFunc : _explorers){
       IOOptions empty; // not options needed here
       CatalogExplorer *explorer = createFunc(resource, empty);
       if ( explorer){
           bool resourceOk = explorer->canUse(resource);
           bool providerOk = explorer->provider() == provider || provider == sUNDEF;
           if (  resourceOk && providerOk) {
               explorers.push_back(explorer);
           }else
               delete explorer;
       }
   }
   return explorers;
}






