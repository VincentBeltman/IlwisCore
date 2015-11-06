#include "errormodel.h"
#include <QString>
#include <QAbstractListModel>
#include <QDebug>

bool ErrorModel::instanceFlag = false;
ErrorModel* ErrorModel::_instance = NULL;
ErrorModel* ErrorModel::getInstance()
{
    qDebug() << "getInstance aangeroepen";
    if(! instanceFlag)
    {
        qDebug() << "Errormodel aanmaken en teruggeven";
        _instance = new ErrorModel();
        instanceFlag = true;
        return _instance;
    }
    else
    {
        qDebug() << "Bestaande errormodel terruggeven";
        return _instance;
    }
}

ErrorModel::~ErrorModel()
{
    instanceFlag = false;
}

QStringList ErrorModel::errorlist()
{
    return _errorlist;
}

void ErrorModel::addError(int id, QString msg)
{
    _errorlist.append(msg);
    emit errorlistChanged();
}
