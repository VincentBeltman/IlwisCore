#include "kernel.h"
#include "raster.h"
#include "gridcoverage.h"
#include "pixeliterator.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericdomain.h"
#include "numericitem.h"
#include "numericitemrange.h"
#include "ilwisoperation.h"
#include "indexslicer.h"

using namespace Ilwis;

IndexSlicer::IndexSlicer(const IGridCoverage &cov) : _raster(cov)
{
}

void IndexSlicer::grid(const IGridCoverage &cov)
{
    _raster = cov;
}

IGridCoverage IndexSlicer::operator()(const QString &item1, const QString &item2, const QString &step)
{
    IDomain indexDomain = _raster->datadef().domain(DataDefinition::daINDEX);
    if (!indexDomain.isValid())
        return IGridCoverage();
    if ( hasType(indexDomain->valueType(), itNUMERICVALUES)) {

        bool ok;
        double itemIndex = item1.toDouble(&ok);
        if (!ok){
            ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("item boundary"), TR("Sub setting"));
        }

        QString basename = makeBaseName();

        double index;
        if ( indexDomain->valueType() == itNUMERICITEM)  {
            index = findIndexNumericalItem(indexDomain, itemIndex);
        } else if ( hasType(indexDomain->valueType(),itNUMERIC)) {
            index  = findIndexNumber(indexDomain, itemIndex);
        }
        QString cname;
        QString expr = makeExpression(index, basename, cname);

        IGridCoverage mp = Operation::calculate<IGridCoverage>(cname,expr);
        if ( mp.isValid())
            return mp;
    }
    return IGridCoverage();
}

QString IndexSlicer::makeBaseName() const {
    QString basename = _raster->name();
    int ind = 0;
    if( (ind=basename.lastIndexOf(".")) != -1){
        basename = basename.left(ind);
    }
    return basename;
}

QString IndexSlicer::makeExpression(double index, const QString& basename, QString& cname) {
    double rest1 = index - (int)index;
    QString expr =  _raster->datadef().range(DataDefinition::daINDEX)->interpolation();
    bool isContinous = _raster->datadef().range(DataDefinition::daINDEX)->isContinous();
    if ( std::abs(rest1) > EPS8 && isContinous) {
        int lowerIndex = std::floor(index);
        double rest2 = 1.0 - rest1;
        cname = QString("%1_%2_%3").arg(basename).arg(lowerIndex).arg(lowerIndex+1);
        expr = cname + "=" + expr.arg(QString("%1*%2[%3]")
                                      .arg(rest2)
                                      .arg(_raster->name())
                                      .arg(lowerIndex))
                .arg(QString("%1*%2[%3]").
                     arg(rest1).
                     arg(_raster->name()).
                     arg(lowerIndex+1));
    } else {
        cname = QString("%1_%2").arg(basename).arg((int)index);
        expr = QString("%1=%2[%3]").arg(cname).arg( _raster->name()).arg((int)index);

    }
    return expr;
}

double IndexSlicer::findIndexNumber(const IDomain& indexDomain, double itemIndex) const{
    INumericDomain numdom = indexDomain.get<NumericDomain>();
    SPNumericRange numrange = numdom->range<NumericRange>();
    if ( numrange->contains(itemIndex))
        return itemIndex;
    return rUNDEF;
}

double IndexSlicer::findIndexNumericalItem(const IDomain& indexDomain, double itemIndex) const{
    INumericItemDomain numdom = indexDomain.get<NumericItemDomain>();
    SPNumericItemRange numrange = numdom->range<NumericItemRange>();
    double index1 = numrange->index(itemIndex);
    if ( index1 == rUNDEF){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("item boundary"), TR("Index slicing"));
        return rUNDEF;
    }
    return index1;
}