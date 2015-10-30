#ifndef WORKFLOWERRORMODEL_H
#define WORKFLOWERRORMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractListModel>
#include "ilwiscoreui_global.h"

class ErrorMsg
{
public:
    ErrorMsg(const int &id, const QString &msg);

    int id() const;
    QString msg() const;

private:
    int _id;
    QString _msg;
};

class ILWISCOREUISHARED_EXPORT WorkflowErrorModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum Roles {
            idRole = Qt::UserRole + 1,
            msgRole
        };

        WorkflowErrorModel(QObject *parent = 0);

        Q_INVOKABLE void addError(int &id, QString &msg);

        int rowCount(const QModelIndex & parent = QModelIndex()) const;

        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;
    private:
        QList<ErrorMsg> _errorMsgList;



//    Q_PROPERTY(QStringList errorlist READ errorlist NOTIFY errorlistChanged)
//    Q_PROPERTY(QStringListModel errormodel READ errormodel NOTIFY errormodelChanged)

//    WorkflowErrorModel(QObject *parent=0);
//    ~WorkflowErrorModel();

//    QStringList errorlist();
//    QStringListModel errormodel();

//    Q_INVOKABLE void addError(int id, QString msg);

//private:
//    QStringList _errorlist;
//    QStringListModel* _errormodel;

//signals:
//    void errorlistChanged();
//    void errormodelChanged();
};

#endif // WORKFLOWERRORMODEL_H
