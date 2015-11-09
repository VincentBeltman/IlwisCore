#ifndef NODEPROPOBJECT_H
#define NODEPROPOBJECT_H

#include <QObject>
#include "kernel_global.h"

namespace Ilwis {
struct NodeProperties;
}


class KERNELSHARED_EXPORT NodePropObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint16 x MEMBER _x)
    Q_PROPERTY(quint16 y MEMBER _y)
    Q_PROPERTY(quint32 vertex MEMBER _vertex)
    Q_PROPERTY(quint64 operationId MEMBER _operationId)
public:
    explicit NodePropObject(QObject *parent = 0);
    void setProps(const Ilwis::NodeProperties& props, int v);

private:
    quint16 _x;
    quint16 _y;
    quint32 _vertex;
    quint64 _operationId;
};

#endif // NODEPROPOBJECT_H
