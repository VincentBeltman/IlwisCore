#ifndef INTERNALOBJECTFACTORY_H
#define INTERNALOBJECTFACTORY_H

namespace Ilwis {

class Coverage;
class GeodeticDatum;
class GeoReference;

namespace Internal {
/*!
 \brief factory used to create IlwisObjects that do not come from an external source

 There are a number of IlwisObject that are based on internal definitions. Many objects in the realm of
 spatial reference system are based on known defaults and thus are generated from a set of internal tables.
Furthermore there are a number of objects that are default to the system (e.g. domain Image). They are
also generated by the internal object factories.

*/
class InternalIlwisObjectFactory : public IlwisObjectFactory
{
public:
    InternalIlwisObjectFactory();

    virtual Ilwis::IlwisObject *create(const Resource & resource,const IOOptions& options=IOOptions()) const;
    virtual IlwisObject *create(IlwisTypes type, const QString &subtype=sUNDEF) const;
    bool canUse(const Ilwis::Resource &resource) const;

private:
    IlwisObject *createEllipsoid(const Resource &resource, const IOOptions &options) const;
    GeodeticDatum *createDatum(const Resource &resource, const Ilwis::IOOptions &options) const;
    IlwisObject *createProjection(const Resource &resource, const Ilwis::IOOptions &options) const;
    IlwisObject *createEllipsoidFromQuery(const QString &query, const Resource& resource) const;
    IlwisObject *createDomain(const Resource &resource, const Ilwis::IOOptions &options) const;
    IlwisObject *createCsyFromCode(const Resource &resource, const Ilwis::IOOptions &options) const;
    IlwisObject *createRasterCoverage(const Resource &resource, const Ilwis::IOOptions &options) const;
    IlwisObject *createTable(const Resource &resource, const IOOptions &options) const;
    IlwisObject *createOperationMetaData(const Resource &resource) const;
    IlwisObject *createGeoreference(const Resource &resource, const Ilwis::IOOptions &options) const;
    bool createCoverage(const Ilwis::Resource &resource, Coverage *coverage, const Ilwis::IOOptions &options) const;
    IlwisObject *createFeatureCoverage(const Resource &resource, const Ilwis::IOOptions &options) const;
    IlwisObject *createCatalog(const Resource &resource, const Ilwis::IOOptions &options) const;
    GeoReference *createGrfFromCode(const Ilwis::Resource &resource) const;
    IlwisObject *createCsy(const Resource &resource, const IOOptions &options) const;
};
}
}

#endif // INTERNALOBJECTFACTORY_H
