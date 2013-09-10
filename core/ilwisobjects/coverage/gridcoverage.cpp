#include "kernel.h"
#include "raster.h"
#include "gridcoverage.h"
#include "pixeliterator.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericitem.h"
#include "numericitemrange.h"
#include "ilwisoperation.h"

quint64 Ilwis::GridBlockInternal::_blockid = 0;

using namespace Ilwis;

GridCoverage::GridCoverage()
{
}

GridCoverage::GridCoverage(const Resource& res) : Coverage(res){

}

GridCoverage::~GridCoverage()
{
}

const IGeoReference& GridCoverage::georeference() const
{
    return _georef;
}

void GridCoverage::georeference(const IGeoReference &grf)
{
    _georef = grf;
    if ( _grid.isNull() == false) { // remove the current grid, all has become uncertain
        _grid.reset(0);

    }
    if ( _georef.isValid()) {
        _georef->compute();
        setCoordinateSystem(grf->coordinateSystem()); // mandatory
    }
}

IlwisTypes GridCoverage::ilwisType() const
{
    return itGRID;
}

GridCoverage *GridCoverage::copy()
{
    GridCoverage *gc = new GridCoverage();
    copyTo(gc);
    return gc;

}


void GridCoverage::copyBinary(const IGridCoverage& gc, int index) {
    IGridCoverage gcNew;
    gcNew.set(this);
    Size inputSize =  gc->size();
    Size sz(inputSize.xsize(),inputSize.ysize(), 1);
    gcNew->georeference()->size(sz);
    PixelIterator iterIn(gc, Box3D<>(Voxel(0,0,index), Voxel(inputSize.xsize(), inputSize.ysize(), index + 1)));
    PixelIterator iterOut(gcNew, Box3D<>(Size(inputSize.xsize(), inputSize.ysize(), 1)));
    for_each(iterOut, iterOut.end(), [&](double& v){
         v = *iterIn;
        ++iterIn;
    });
}

IGridCoverage GridCoverage::get(quint32 index1, quint32 index2)
{
    IGridCoverage gcovParent;
    gcovParent.set(this);
    IGridCoverage gcov = OperationHelperRaster::initialize(gcovParent,itGRID, itDOMAIN | itCOORDSYSTEM | itGEOREF);
    gcov->size(Size(size().xsize(), size().ysize()));
    gcov->_grid.reset(gcov->_grid->clone(index1, index2));

    return gcov;

}

IGridCoverage GridCoverage::get(const QString &item1, const QString &item2)
{
    IDomain indexDomain = datadef().domain(DataDefinition::daINDEX);
    if (!indexDomain.isValid())
        return IGridCoverage();
    if ( indexDomain->valueType() == itNUMERICITEM)  {
        double item1Index = item1.toDouble();
        INumericItemDomain numdom = indexDomain.get<NumericItemDomain>();
        SPNumericItemRange numrange = numdom->range<NumericItemRange>();
        double index1 = numrange->index(item1Index);
        double rest1 = index1 - (int)index1;
        if ( std::abs(rest1) > EPS8 && numrange->isContinous()) {
            int lowerIndex = std::floor(index1);
            double rest2 = 1.0 - rest1;
            QString expr = numrange->interpolation();
            QString nm = name();
            int ind = 0;
            if( (ind=nm.lastIndexOf(".")) != -1){
                nm = nm.left(ind);
            }
            nm = QString("%1_%2_%3").arg(nm).arg(lowerIndex).arg(lowerIndex+1);
            expr = nm + "=" + expr.arg(QString("%1*%2[%3]").arg(rest2).arg(name()).arg(lowerIndex)).arg(QString("%1*%2[%3]").arg(rest1).arg(name()).arg(lowerIndex+1));
            IGridCoverage mp = Operation::execute<IGridCoverage>(expr);
            if ( mp.isValid())
                return mp;
        }
    }
    return IGridCoverage();
}

Grid *GridCoverage::grid()
{
    Locker lock(_mutex);
    if ( _georef->isValid() && !connector().isNull()) {
        if ( _grid.isNull()) {
            _grid.reset(connector()->loadGridData(this));
            if (_grid.isNull())
                return 0;
        }
        Grid *grd = _grid.data();
        return grd;
    }
    return 0;
}

void GridCoverage::copyTo(IlwisObject *obj)
{
    Locker lock(_mutex);
    Coverage::copyTo(obj);
    GridCoverage *gc = static_cast<GridCoverage *>(obj);
    gc->_georef = _georef;
    if ( _grid) {
        gc->_grid.reset(_grid->clone());
    }

}

Resource GridCoverage::resource(int mode) const
{
    Resource res = Coverage::resource(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
        res["georeference()"] = georeference()->id();
        res.setExtendedType( res.extendedType() | itGEOREF);
    }
    return res;
}

Size GridCoverage::size() const
{
    if (_size.isValid() && !_size.isNull())
        return _size;
    if (!_grid.isNull())
        const_cast<GridCoverage *>(this)->_size = _grid->size();
    else if ( _georef.isValid())
        const_cast<GridCoverage *>(this)->_size = _georef->size();

    return _size;

}

void GridCoverage::size(const Size &sz)
{
    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0) {
        _size = sz;
        if (!_grid.isNull())
            _grid->setSize(sz);
        if (_georef.isValid())
            _georef->size(sz);
    }


}





