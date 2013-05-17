#ifndef RESAMPLERASTER_H
#define RESAMPLERASTER_H

namespace Ilwis {
class ResampleRaster : public OperationImplementation
{
public:
    ResampleRaster();
    ResampleRaster(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx=0);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare();

    static quint64 createMetadata();


private:
    IGridCoverage _inputGC;
    IGridCoverage _outputGC;
    IGeoReference _targetGrf;
    GridCoverage::InterpolationMethod _method;
    Box3D<qint32> _box;

};
}

#endif // RESAMPLERASTER_H
