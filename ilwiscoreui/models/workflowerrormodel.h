#ifndef WORKFLOWERRORMODEL_H
#define WORKFLOWERRORMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractListModel>
#include "ilwiscoreui_global.h"
#include "workflow/errormodel.h"

class ILWISCOREUISHARED_EXPORT WorkflowErrorModel : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY(ErrorModel* errorModel READ errorModel)


    WorkflowErrorModel();
    ~WorkflowErrorModel();
    ErrorModel *errorModel() const { return ErrorModel::getInstance(); }
};

#endif // WORKFLOWERRORMODEL_H
