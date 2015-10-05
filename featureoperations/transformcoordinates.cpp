#include <functional>
#include <future>
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "raster.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operationhelperfeatures.h"
#include "operation.h"
#include "transformcoordinates.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(TransformCoordinates)

TransformCoordinates::TransformCoordinates()
{

}

TransformCoordinates::TransformCoordinates(quint64 metaid, const Ilwis::OperationExpression &expr): OperationImplementation(metaid,expr)
{

}

bool TransformCoordinates::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    _outputFeatures->coordinateSystem(_csy);
    _outputFeatures->envelope(_outputFeatures->coordinateSystem()->convertEnvelope(_inputFeatures->coordinateSystem(), _inputFeatures->envelope()));


    for(const auto& infeature : _inputFeatures){
       _outputFeatures->newFeatureFrom(infeature, _inputFeatures->coordinateSystem());
    }
    _outputFeatures->attributesFromTable(_inputFeatures->attributeTable());

    if ( ctx != 0) {
        QVariant value;
        value.setValue<IFeatureCoverage>(_outputFeatures);
        ctx->setOutput(symTable, value, _outputFeatures->name(), itFEATURE,_outputFeatures->source());
    }

    return true;
}

OperationImplementation *TransformCoordinates::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TransformCoordinates(metaid, expr);
}

quint64 TransformCoordinates::createMetadata()
{
    OperationResource operation({"ilwis://operations/transformcoordinates"});
    operation.setSyntax("transformcoordinates(inputfeaturemap, csydefintion)");
    operation.setDescription(TR("transforms the coordinates of all vertices of the features from one coordinate system to another"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itFEATURE,  TR("Source feature coverage"),TR("The feature coverage to be transformed to another coordinate system"));
    operation.addInParameter(1,itSTRING, TR("coordinate system definition"),TR("defintion of new projection in terms of epsg or proj4 or the url of an existing coordinate system"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE, TR("output feature coverage"), TR("output feature coverage where all vertices have new coordinates"));
    operation.setKeywords("features, vector, coordinatesystem");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation::State TransformCoordinates::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    QString inputfile = _expression.parm(0).value();
    QString code = _expression.parm(1).value();
    QString newmap = _expression.parm(0,false).value();

    // Goede file?
    if (!_inputFeatures.prepare(inputfile, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,inputfile,"");
        return sPREPAREFAILED;
    }
    // Zijn er wel punten?
    if ( _inputFeatures->featureCount(itLINE) == 0){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,TR("number of points"), TR("pointrastercrossing operation"));
        return sPREPAREFAILED;
    }
    //Valide coordinaatsysteen
    if (!_csy.prepare(code)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,code,"" );
        return sPREPAREFAILED;
    }

    // Is de nieuwe file wel schrijfbaar?
    OperationHelperFeatures helper;
    IIlwisObject obj = helper.initialize(_inputFeatures.as<IlwisObject>(), itFEATURE, itUNKNOWN) ;
    if ( !obj.isValid()) {
        ERROR2(ERR_INVALID_INIT_FOR_2,"FeatureCoverage",inputfile);
        return sPREPAREFAILED;
    }
    _outputFeatures = obj.as<FeatureCoverage>();
    _outputFeatures->name(newmap);

    return sPREPARED;
}


