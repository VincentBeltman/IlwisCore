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
#include "nodepropobject.h"
#include "edgepropobject.h"
#include <QQmlListProperty>

#include "rastercoverage.h"
using namespace Ilwis;
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
    ~WorkflowModel();
    explicit WorkflowModel(const Ilwis::Resource &source, QObject *parent=0);

    QString makeOutputPath(const QString& filename);

    Q_PROPERTY(QQmlListProperty<NodePropObject>  nodes READ getNodes CONSTANT)
    Q_PROPERTY(QQmlListProperty<EdgePropObject>  edges READ getEdges CONSTANT)


    /*!
     * \brief Assigns constant input data to the operation
     *
     * Expects input for every parameter of the operation. Updates every parameter of the operation
     *
     * \param inputData the new input seperated by |
     * \param operationIndex The index of the operation
     * \return a list of changes
     */
    Q_INVOKABLE QStringList asignConstantInputData(QString inputData, int operationIndex);
    /*!
     * \brief Adds an operation to the workflow
     * \param id The resource id
     * \return a list of changes
     */
    Q_INVOKABLE QStringList addOperation(const QString& id);
    /*!
     * \brief Adds a flow to the workflow
     * \param vertexFrom The index of vertex where the flow comes from
     * \param vertexTo The index of vertex where the flow goes to
     * \param flowpoints A map with information for the new flow
     * \param rectFrom The index of the rectangle where the flow comes from
     * \param rectTo The index of the rectangle where the flow goes to
     * \return a list of changes
     */
    Q_INVOKABLE QStringList addFlow(int vertexFrom, int vertexTo, const QVariantMap &flowpoints, int rectFrom, int rectTo);
    /*!
     * \brief Deletes an operation from the workflow
     * \param index The index of the vertex that should be deleted
     * \return a list of changes
     */
    Q_INVOKABLE QStringList deleteOperation(int index);
    /*!
     * \brief Deletes a flow from the workflow
     * \param vertexFrom The index of vertex where the flow comes from
     * \param vertexTo The index of vertex where the flow goes to
     * \param parameterFrom The index of the parameter where the flow comes from
     * \param parameterto The index of the parameter where the flow goes to
     * \return a list of changes
     */
    Q_INVOKABLE QStringList deleteFlow(int vertexFrom, int vertexTo, int parameterFrom, int parameterto);
    /*!
     * \brief Checks whether combination of operation and parameter has value defined
     * \param operationIndex The vertex
     * \param parameterIndex The parameter
     * \return true when has defined false if not
     */
    Q_INVOKABLE bool hasValueDefined(int operationIndex, int parameterIndex);
    /*!
     * \brief Retrieves the input parameter count of the vertex
     * \param operationIndex The vertex
     * \return the amount of input parameters of the vertex
     */
    Q_INVOKABLE int operationInputParameterCount(int operationIndex);
    /*!
     * \brief Retrieves the output parameter count of the vertex
     * \param operationIndex The vertex
     * \return the amount of output parameters of the vertex
     */
    Q_INVOKABLE int operationOutputParameterCount(int operationIndex);
    /*!
     * \brief Returns the values of an operation which have already been defined (a flow has been drawn to it)
     * \param operationIndex The vertex
     * \return a string of fields which have been defined, seperated by |
     */
    Q_INVOKABLE QString implicitIndexes(int operationIndex);

    /*!
     * \brief retrieves all information of all nodes
     * \return a list with all information about all nodes
     */
    QQmlListProperty<NodePropObject> getNodes() ;
    /*!
     * \brief retrieves all information of all edges
     * \return a list with all information about all edges
     */
    QQmlListProperty<EdgePropObject> getEdges();
    /*!
     * \brief Returns all values (even empty) of a vertex
     * \param operationIndex The vertex
     * \return The values of the vertex seperated by |
     */
    Q_INVOKABLE QStringList getAsignedValuesByItemID(int operationIndex);
    /*!
     * \brief returns the amount of input parameters of the workflow
     * \return the input parameter count
     */
    Q_INVOKABLE int getInputParameterCount() { return _inputParameterCount;}

    /*!
     * \brief saves the workflow and the coordinates of all vertexes
     * \param coordinates The coordinates of all vertexes
     */
    Q_INVOKABLE void store(const QStringList &coordinates);
    /*!
     * \brief loads the workflow model from an existing model
     */
    Q_INVOKABLE void load();
    /*!
     * \brief creates the metadata of the workflow
     */
    Q_INVOKABLE void createMetadata();

private:
    Ilwis::IWorkflow _workflow;
    QList<NodePropObject *> _nodeProps;
    QList<EdgePropObject *> _edgeProps;
    int _inputParameterCount = 0;
};


typedef QQmlListProperty<WorkflowModel> QMLWorkflowList;

Q_DECLARE_METATYPE(QMLWorkflowList)


#endif // WORKFLOWMODEL_H
