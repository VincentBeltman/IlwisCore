#ifndef WORKFLOWMODEL_H
#define WORKFLOWMODEL_H

#include "models/operationmodel.h"
#include <QPoint>
#include <QMap>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "kernel_global.h"
#include "ilwistypes.h"
#include "operationmetadata.h"
#include "workflow.h"
#include <QQmlListProperty>

#include "rastercoverage.h"

namespace Ilwis {
class OperationMetaData;
typedef IlwisData<OperationMetaData> IOperationMetaData;
}
/**
 * Adds workflow specific accessors to an OperationModel.
 *
 * @brief The WorkflowModel class
 */
class ILWISCOREUISHARED_EXPORT WorkflowModel: public OperationModel
{
     Q_OBJECT
public:
    WorkflowModel();
    explicit WorkflowModel(const Ilwis::Resource &source, QObject *parent=0);

    Q_INVOKABLE void addOperation(const QString& id);
    Q_INVOKABLE void addFlow(int indexStart, int operationIndex2, const QVariantMap &flowpoints);
    Q_INVOKABLE void deleteOperation(int index);
    Q_INVOKABLE void deleteFlow(int operationIndex1, int operationIndex2, int indexStart, int indexEnd);

    Q_INVOKABLE bool hasValueDefined(int operationindex, int parameterindex);

    Q_INVOKABLE void run(const QString &input);
    Q_INVOKABLE void createMetadata();
private:
    Ilwis::IWorkflow _workflow;
    std::vector<Ilwis::OVertex> _operationNodes;
    QString makeOutputPath(const QString& filename);
};


typedef QQmlListProperty<WorkflowModel> QMLWorkflowList;

Q_DECLARE_METATYPE(QMLWorkflowList)

#endif // WORKFLOWMODEL_H
