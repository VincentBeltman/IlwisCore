#include "workflowerrormodel.h"
#include <QString>
#include <QAbstractListModel>

ErrorMsg::ErrorMsg(const int &id, const QString &msg)
    : _id(id), _msg(msg)
{
}

int ErrorMsg::id() const
{
    return _id;
}

QString ErrorMsg::msg() const
{
    return _msg;
}

WorkflowErrorModel::WorkflowErrorModel(QObject *parent)
//    : QAbstractListModel(parent)
{
}

void WorkflowErrorModel::addError(int &id, QString &msg)
{
    ErrorMsg errormsg(id, msg);
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _errorMsgList << errormsg;
    endInsertRows();
}

int WorkflowErrorModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return _errorMsgList.count();
}

QVariant WorkflowErrorModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= _errorMsgList.count())
        return QVariant();

    const ErrorMsg &errorMsg = _errorMsgList[index.row()];
    if (role == idRole)
        return errorMsg.id();
    else if (role == msgRole)
        return errorMsg.msg();
    return QVariant();
}

QHash<int, QByteArray> WorkflowErrorModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[idRole] = "id";
    roles[msgRole] = "msg";
    return roles;
}



//QStringList WorkflowErrorModel::errorlist()
//{
//    return _errorlist;
//}

//QStringListModel *WorkflowErrorModel::errormodel()
//{
//    QStringListModel *model = new QStringListModel;

//    return model;
//}

//void WorkflowErrorModel::addError(int id, QString msg)
//{

//    _errorlist.append(msg);
//    _errormodel->setStringList(_errorlist);
//}
