#include "kernel.h"
#include "uicontextmodel.h"
#include "visualizationmanager.h"
#include "coveragelayermodel.h"

CoverageLayerModel::CoverageLayerModel()
{
}

CoverageLayerModel::CoverageLayerModel(quint32 layerIndex, const Ilwis::Resource &resource,
                                       const QList<VisualAttributeEditor *> &editors,
                                       Ilwis::Geodrawer::DrawerInterface *drawer,QObject *obj) :
    IlwisObjectModel(resource, obj), _propertyEditors(editors), _drawer(drawer)
{
    for(auto editor : _propertyEditors){
        editor->setParent(this);
        editor->setlayer(layerIndex, this);
    }
}

VisualAttributeEditor *CoverageLayerModel::propertyEditor(const QString &name)
{
    for(auto editor : _propertyEditors){
        if ( editor->editorName() == name)
            return editor;
    }
    return 0;
}

Geodrawer::DrawerInterface *CoverageLayerModel::drawer()
{
    return _drawer;
}

QQmlListProperty<VisualAttributeEditor> CoverageLayerModel::propertyEditors()
{
    return QQmlListProperty<VisualAttributeEditor>(this, _propertyEditors);
}


