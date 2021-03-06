#ifndef SIMPLELINESETTER_H
#define SIMPLELINESETTER_H

#include "drawers/drawerinterface.h"
#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Geodrawer{

class SimpleLineSetter : public BaseSpatialAttributeSetter
{
public:
    SimpleLineSetter(const IOOptions &options);
    ~SimpleLineSetter();

    static DrawerAttributeSetter *create(const IOOptions& options);

    FeatureDrawing setSpatialAttributes(const SPFeatureI& feature,
                                 QVector<QVector3D>& vertices,
                                 QVector<QVector3D>& normals) const;

    void setColorAttributes(const VisualAttribute& attr,
                            const QVariant& value,const QColor& defaultColor,
                            const FeatureDrawing& drawing,
                            std::vector<VertexColor>& colors) const;

    NEW_DRAWER_ATTRIBUTE_SETTER
};
}
}

#endif // SIMPLELINESETTER_H
