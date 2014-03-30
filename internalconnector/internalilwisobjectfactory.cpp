#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QSettings>
#include <QUrlQuery>
#include <QColor>

#include "kernel.h"
#include "domainitem.h"
#include "raster.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "juliantime.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "textdomain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "colorrange.h"
#include "colordomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "numericitem.h"
#include "coloritem.h"
#include "resource.h"
#include "geodeticdatum.h"
#include "internalilwisobjectfactory.h"
#include "ellipsoid.h"
#include "projection.h"
#include "projectionimplementation.h"
#include "projectionfactory.h"
#include "columndefinition.h"
#include "table.h"
#include "georefimplementation.h"
#include "georefimplementationfactory.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "ilwisobjectconnector.h"
#include "internalrastercoverageconnector.h"
#include "basetable.h"
#include "flattable.h"
#include "databasetable.h"
#include "conventionalcoordinatesystem.h"
#include "operationmetadata.h"
#include "epsg.h"
#include "catalog.h"

using namespace Ilwis;
using namespace Internal;

InternalIlwisObjectFactory::InternalIlwisObjectFactory() : IlwisObjectFactory("IlwisObjectFactory", "internal", "")
{
}

Ilwis::IlwisObject *InternalIlwisObjectFactory::create(const Resource& resource, const PrepareOptions &) const
{
    if ( resource.url().scheme()!="ilwis")
        return 0;


    if ( resource.ilwisType() & itELLIPSOID) {
        return createEllipsoid(resource);
    } else if ( resource.ilwisType() & itPROJECTION) {
        return createProjection(resource) ;
    }  else if ( resource.ilwisType() & itDOMAIN) {
        return createDomain(resource);
    } else if ( resource.ilwisType() & itCOORDSYSTEM) {
        return createCsyFromCode(resource);
    } else if ( resource.ilwisType() & itRASTER) {
        return createRasterCoverage(resource);
    } else if ( resource.ilwisType() & itTABLE) {
        return createTable(resource);
    } else if ( resource.ilwisType() & itOPERATIONMETADATA) {
        return createOperationMetaData(resource);
    } else if ( resource.ilwisType() & itGEOREF) {
        return createGeoreference(resource);
    } else if ( resource.ilwisType() & itFEATURE) {
        return createFeatureCoverage(resource);
    } else if ( resource.ilwisType() & itCATALOG) {
        return createCatalog(resource);
    }
    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createCatalog(const Resource& resource) const{
    if ( hasType(resource.ilwisType(), itCATALOG)){
        Catalog *cat = new Catalog(resource);
        const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
        if (!factory) {
            ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
            return 0;
        }
        ConnectorInterface *connector = factory->createFromResource<>(resource, "ilwis");
        if ( !connector) {
            ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
            return 0;
        }
        cat->setConnector(connector);

        return cat;

    }
    return nullptr;
}

IlwisObject *InternalIlwisObjectFactory::createFeatureCoverage(const Resource& resource) const{
    if ( !resource.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"resource");
        return 0;
    }
    FeatureCoverage *fcoverage = new FeatureCoverage(resource);
    if (!createCoverage(resource, fcoverage))
        return 0;

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory) {
        ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
        return 0;
    }
    ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");
    if ( !connector) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
        return 0;
    }
    fcoverage->setConnector(connector);
    connector->loadMetaData(fcoverage);

    return fcoverage;

}

IlwisObject *InternalIlwisObjectFactory::createOperationMetaData(const Resource& resource) const{
    return new OperationMetaData(resource);
}

IlwisObject *InternalIlwisObjectFactory::create(IlwisTypes type, const QString& subtype) const
{
    switch(type) {
    case itITEMDOMAIN:
    {
        QString sub = subtype.toLower();
        if (  sub == "thematic")
            return new ItemDomain<ThematicItem>();
        if ( sub == "identifier")
            return new ItemDomain<NamedIdentifier>();
        if ( sub == "indexed")
            return new ItemDomain<IndexedIdentifier>();
        if ( sub == "palette")
            return new ItemDomain<ColorItem>();
    }
    case itTEXTDOMAIN:
        return new TextDomain();
    case itRASTER:
        return new RasterCoverage();
    case itTABLE:
        return new FlatTable();
    case itGEOREF:
        return new GeoReference();
    case itCONVENTIONALCOORDSYSTEM:
        return new ConventionalCoordinateSystem();
    case itNUMERICDOMAIN:
        return new NumericDomain();
    case itCOLORDOMAIN:
        return new ColorDomain();
    case itPROJECTION:
        return new Projection();
    case itELLIPSOID:
        return new Ellipsoid();
    case itOPERATIONMETADATA:
        return new OperationMetaData();
    }
    if ( type & itFEATURE)
        return new FeatureCoverage();
    return 0;
}

bool InternalIlwisObjectFactory::canUse(const Resource& resource) const
{
    if ( resource.url().scheme()!="ilwis")
        return false;

    if ( resource.ilwisType() & itELLIPSOID) {
        return true;
    } else if ( resource.ilwisType() & itPROJECTION) {
        return true;
    } else if ( resource.ilwisType() & itGEODETICDATUM) {
        return true;
    } else if ( resource.ilwisType() & itDOMAIN) {
        return true;
    } else if ( resource.ilwisType() & itCOORDSYSTEM) {
        return true;
    } else if ( resource.ilwisType() & itRASTER) {
        return true;
    } else if ( resource.ilwisType() & itTABLE) {
        return true;
    }else if ( resource.ilwisType() & itOPERATIONMETADATA) {
        return true;
    } else if ( resource.ilwisType() & itGEOREF) {
        return true;
    } else if ( resource.ilwisType() & itFEATURE) {
        return true;
    } else if ( resource.ilwisType() & itCATALOG) {
        return true;
    }

    return false;
}



bool InternalIlwisObjectFactory::createCoverage(const Resource& resource, Coverage *coverage) const {

    if (!coverage->prepare())
        return false;

    //coverage->setName(QString("%1%2").arg(ANONYMOUS_PREFIX).arg(coverage->id()));

    ICoordinateSystem csy;
    QString typnm = resource["coordinatesystem"].typeName();
    if (typnm == "Ilwis::ICoordinateSystem")
        csy = resource["coordinatesystem"].value<Ilwis::ICoordinateSystem>();
    else if( typnm == "QString" &&
             resource["coordinatesystem"].toString() != sUNDEF  ) {
        Resource newresource = property2Resource(resource["coordinatesystem"], itCOORDSYSTEM);
        if ( newresource.isValid()) {
            if (!csy.prepare(newresource))
                return false;
        }
    }
    if ( csy.isValid()){
        coverage->coordinateSystem(csy);
    }

    Envelope bounds;
    if ( QString(resource["envelope"].typeName()) == "Ilwis::Box<double>") {
        bounds = resource["envelope"].value<Envelope>();
    }else if (QString(resource["envelope"].typeName()) == "QString" &&
              resource["envelope"].toString() != sUNDEF) {
        bounds = Envelope(resource["envelope"].toString());
    }
    if ( bounds.isValid()) {
        coverage->envelope(bounds);
    }
    if ( resource.ilwisType() == itRASTER) {
        IDomain dom;
        QString tpname = resource["domain"].typeName();
        if (tpname == "Ilwis::IDomain")
            dom = resource["domain"].value<Ilwis::IDomain>();
        else if( tpname == "QString" &&
                 resource["domain"].toString() != sUNDEF  ) {
            Resource newresource = property2Resource(resource["domain"], itDOMAIN);
            if ( newresource.isValid()) {
                if (!dom.prepare(newresource))
                    return false;
            }
        }
        if ( dom.isValid()){
            RasterCoverage *raster = static_cast<RasterCoverage *>(coverage);
            raster->datadef().domain(dom);
        }
    }
    return true;
}

IlwisObject *InternalIlwisObjectFactory::createRasterCoverage(const Resource& resource) const {
    if ( !resource.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,"resource");
        return 0;
    }
    RasterCoverage *gcoverage = new RasterCoverage(resource);
    if (!createCoverage(resource, gcoverage))
        return 0;

    Size<> sz;
    if ( QString(resource["size"].typeName()) == "Ilwis::Size"){
        sz = resource["size"].value<Size<>>();
    } else if (QString(resource["size"].typeName()) == "QSize") {
        sz = resource["size"].toSize();
    }

    IGeoReference grf;
    QString tpnam = resource["georeference"].typeName();
    if (tpnam == "Ilwis::IGeoReference")
        grf = resource["georeference"].value<Ilwis::IGeoReference>();
    else if( tpnam == "QString"  && resource["georeference"].toString() != sUNDEF) {
        Resource newresource = property2Resource(resource["georeference"], itGEOREF);
        if ( newresource.isValid()) {
            if (!grf.prepare(newresource))
                return 0;
        }
    } else{
        Envelope bounds = gcoverage->envelope();
        if ( bounds.isValid() && !bounds.isNull()){
//            GeoReference *cgeoref = new GeoReference();
//            GeoRefImplementationFactory *grfFac = kernel()->factory<GeoRefImplementationFactory>("georefimplementationfactory");
//            GeoRefImplementation *impl = grfFac->create("corners");
//            if ( !impl) {
//                ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"corners georef",resource.name());
//                return 0;
//            }
            grf = GeoReference::create("corners");
            grf->setName("subset_" + gcoverage->name());
            grf->coordinateSystem(gcoverage->coordinateSystem());
            grf->impl<CornersGeoReference>()->setEnvelope(bounds);
            grf->size(sz);
            if (!grf->compute()){
                ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "Georeference");
                return 0;
            }

        }

    }
    if ( grf.isValid())
        gcoverage->georeference(grf);
    if ( sz.isValid())
        gcoverage->size(sz);

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!factory) {
        ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1, "ilwis::ConnectorFactory");
        return 0;
    }
    InternalRasterCoverageConnector *connector = factory->createFromResource<InternalRasterCoverageConnector>(resource, "internal");
    if ( !connector) {
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "connector", resource.name());
        return 0;
    }
    gcoverage->setConnector(connector);
    connector->loadMetaData(gcoverage);

    return gcoverage;
}

Resource InternalIlwisObjectFactory::property2Resource(const QVariant& property, IlwisTypes type) const{
    if ( !property.isValid() || property.isNull() )
        return Resource();
    bool ok;
    quint64 id = property.toULongLong(&ok);
    if ( ok)
        return mastercatalog()->id2Resource(id);
    else
        return mastercatalog()->name2Resource(property.toString(), type);
}

IlwisObject *InternalIlwisObjectFactory::createDomain(const Resource& resource) const{
    if ( resource.ilwisType() == itTEXTDOMAIN || resource.code() == "text")
        return new TextDomain(resource);

    if ( resource.ilwisType() == itCOLORDOMAIN || resource.code() == "color")
        return new ColorDomain(resource);
    QString code = resource.code();
    if ( code != sUNDEF) {

        QSqlQuery db(kernel()->database());
        QString query = QString("Select linkedtable from codes where code = '%1'").arg(code);
        if (db.exec(query)) {
            if ( db.next()){
                QString table = db.value(0).toString();
                if ( table == "numericdomain"){
                    query = QString("Select * from numericdomain where code='%1'").arg(code);
                    if (db.exec(query)) {
                        if ( db.next()){
                            QSqlRecord rec = db.record();
                            NumericDomain *dv = new NumericDomain(resource);

                            dv->fromInternal(rec);
                            double vmin = rec.field("minv").value().toDouble();
                            double vmax = rec.field("maxv").value().toDouble();
                            double step = rec.field("resolution").value().toDouble();
                            int range_strict = rec.field("range_strict").value().toInt();
                            // TODO:, implement unit stuff
                            QString unit = rec.field("unit").value().toString();
                            if (unit == "Days"){
                                if ( fmod(step,1.0) == 0 && step != 0)
                                    dv->range(new TimeInterval(Time(vmin), Time(vmax)));
                                else
                                    dv->range(new TimeInterval(Time(vmin), Time(vmax),Duration(QString("%1D").arg(step))));
                            }else {
                                if ( fmod(step,1.0) == 0 && step != 0)
                                    dv->range(new NumericRange(vmin, vmax,1));
                                else
                                    dv->range(new NumericRange(vmin, vmax));
                            }

                            dv->setStrict(range_strict ? true : false);
                            QString parent = rec.field("parent").value().toString();
                            if ( parent != "" && parent !=  code) { // no parenting to itself
                                IDomain dom;
                                dom.prepare(parent);
                                if ( dom.isValid()) {
                                    dv->setParent(dom);
                                }
                            }
                            dv->setReadOnly(true);
                            return dv;
                        }else {
                            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
                        }
                    }

                } else if (table == "thematicdomain"){
                    //TODO: internal thematic domains
                }

            }
        }else {
            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
        }
    }else {
        if ( hasType(resource.ilwisType(), itITEMDOMAIN )){
            if ( hasType(resource.extendedType(), itNAMEDITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                return new ItemDomain<NamedIdentifier>(res);
            }
            if ( hasType(resource.extendedType(), itINDEXEDITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                return new ItemDomain<IndexedIdentifier>(res);
            }
            if ( hasType(resource.extendedType(), itTHEMATICITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                return new ItemDomain<ThematicItem>(res);
            }
            if ( hasType(resource.extendedType(), itNUMERICITEM)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                return new ItemDomain<NumericItem>(res);
            }
            if ( hasType(resource.extendedType(), itPALETTECOLOR)) {
                Resource res = resource;
                res.setIlwisType(itITEMDOMAIN);
                return new ItemDomain<ColorItem>(res);
            }
        } if ( hasType(resource.ilwisType(), itNUMERICDOMAIN)){
            return new NumericDomain(resource);
        }
    }

    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createCsyFromCode(const Resource& resource) const {
    QString code = resource.code();
    bool isUnknown = code == "unknown";
    if ( isUnknown)
        code = "epsg:4326";
    QString projParms = code;
    if ( code.left(6) != "proj4:"){
        QString query = QString("select * from projectedcsy where code='%1'").arg(code);
        QSqlQuery db(kernel()->database());
        if ( db.exec(query)) {
            if (db.next()) {
                QSqlRecord rec = db.record();
                projParms = rec.value("proj_params").toString();
            } else {
                kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("coordinatesystem", resource.name()));
                return 0;
            }
        }
    } else {
        projParms = code.mid(6);
    }
    ConventionalCoordinateSystem *csy = new ConventionalCoordinateSystem(resource);
    if ( isUnknown){
        csy->setName("unknown");
        csy->setCode("unknown");
        csy->setDescription(TR("Unknown coordinate system"));
    }else{
        csy->setName(resource.name());
        csy->setCode(resource.code());
    }
    csy->prepare("proj4=" + projParms);

    return csy;

}

IlwisObject *InternalIlwisObjectFactory::createProjection(const Resource& resource) const {
    QString query;
    QString code = resource.code();

    if ( code != "") {
        QSqlQuery db(kernel()->database());
        query = QString("Select * from projection where code = '%1'").arg(code);
        if ( db.exec(query)) {
            if (db.next()) {
                QSqlRecord rec = db.record();
                //if ( code == "longlat") // special case
                //    return new NullProjection(resource);
                const ProjectionFactory *factory =  kernel()->factory<ProjectionFactory>("ProjectionFactory",resource);
                if ( factory) {
                    ProjectionImplementation *projimpl = static_cast<ProjectionImplementation *>(factory->create(resource));
                    if (!projimpl) {
                        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("projection", resource.name()));
                        return 0;
                    }
                    Projection *proj = new Projection();
                    proj->setImplementation(projimpl);
                    proj->fromInternal(rec);
                    proj->setAuthority(rec.field("authority").value().toString());
                    proj->setWkt(rec.field("wkt").value().toString());
                    return proj;
                } else {
                    kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("ProjectionFactory",resource.name()));
                }
            } else {
                kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(code));
            }
        } else{
            kernel()->issues()->logSql(db.lastError());
        }
    } else {
        kernel()->issues()->log(TR(ERR_MISSING_CODE_FOR_SYSTEM_OBJECT));
    }
    return 0;
}

GeodeticDatum *InternalIlwisObjectFactory::createDatum(const Resource& resource) const {
    QString query;
    if ( resource.code() != sUNDEF) {
        QString code = resource.code();
        if ( code != "") {
            query = QString("Select * from datum where code = '%1'").arg(code);
        }
    }
    if ( resource["area"] != sUNDEF) {
        QString name = resource.name();
        QString area = resource["area"].toString();
        query = QString("Select * from datum where name='%1' and area='%1'").arg(name, area);
    }

    if ( query == "")
        return 0;

    QSqlQuery db(kernel()->database());
    if (db.exec(query) && db.next()) {
        GeodeticDatum *datum = new GeodeticDatum();
        QSqlRecord rec = db.record();
        datum->setName(rec.field("name").value().toString());
        datum->setDescription(rec.field("description").value().toString());
        datum->setAuthority(rec.field("authority").value().toString());
        datum->setArea(rec.field("area").value().toString());
        datum->setCode(rec.field("code").value().toString());
        datum->set3TransformationParameters(rec.field("dx").value().toDouble(),
                                            rec.field("dy").value().toDouble(),
                                            rec.field("dz").value().toDouble());

        return datum;
    }

    return 0;
}

IlwisObject *InternalIlwisObjectFactory::createGeoreference(const Resource& resource) const {
    GeoReference *cgrf = GeoReference::create("corners", resource);
    cgrf->setName( resource["name"].toString());
    cgrf->setCreateTime(Time::now());
    cgrf->setModifiedTime(Time::now());
    ICoordinateSystem csy;
    bool ok;
    quint64 id = resource["coordinatesystem"].toULongLong(&ok);
    if ( ok) {
        csy = mastercatalog()->get(id);
    } else
       csy =  resource["coordinatesystem"].value<ICoordinateSystem>();

    cgrf->coordinateSystem(csy);
    cgrf->impl<CornersGeoReference>()->setEnvelope(resource["envelope"].value<Envelope>());
//    Size sz = resource["size"].value<Size>();
    cgrf->size(resource["size"].value<Size<>>());
    cgrf->centerOfPixel(resource["centerofpixel"].toBool());

    return cgrf;
}

IlwisObject *InternalIlwisObjectFactory::createTable(const Resource& resource) const {

    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    ConnectorInterface *connector = factory->createFromResource<>(resource, "internal");

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
    Table *tbl;
    if ( resource.ilwisType() == itDATABASETABLE)
        tbl = new DatabaseTable(resource);
    else
        tbl = new FlatTable(resource);
    tbl->setConnector(connector);
    return tbl;

}
IlwisObject *InternalIlwisObjectFactory::createEllipsoid(const Resource& resource) const {
    QString query    ;
    QString code = resource.code();

    if ( code != sUNDEF) {
        query = QString("Select * from ellipsoid where code = '%1'").arg(code);

    }else {
        QUrlQuery queryItem(resource.url());
        if ( queryItem.hasQueryItem("wkt")) {

            QString wkt = queryItem.queryItemValue("wkt");
            if ( wkt != "") {
                query = QString("Select * from ellipsoid where wkt = '%1'").arg(wkt);
            }

        }
    }
    return createEllipsoidFromQuery(query, resource);
}

IlwisObject *InternalIlwisObjectFactory::createEllipsoidFromQuery(const QString &query, const Resource& resource) const {
    if ( query == "") {
        kernel()->issues()->log(TR("No valid ellipsoid query could be made"));
        return 0;
    }

    QSqlQuery db(kernel()->database());
    if (db.exec(query) && db.next()) {
        Ellipsoid *ellipsoid = new Ellipsoid(resource);
        ellipsoid->fromInternal(db.record());
        double ma = db.record().field("majoraxis").value().toDouble();
        double rf = db.record().field("invflattening").value().toDouble();
        ellipsoid->setEllipsoid(ma,rf);
        ellipsoid->setAuthority(db.record().field("authority").value().toString());
        return ellipsoid;
    }
    return 0;
}

