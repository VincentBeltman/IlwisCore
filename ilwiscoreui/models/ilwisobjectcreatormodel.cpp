#include "ilwisobjectcreatormodel.h"

IlwisObjectCreatorModel::IlwisObjectCreatorModel(QObject *parent) : QObject(parent)
{

}

IlwisObjectCreatorModel::IlwisObjectCreatorModel(const QString &label, quint64 type, const QString &creatorUrl, int height, QObject *parent):
QObject(parent),
  _label(label),
  _type(type),
  _creatorUrl(creatorUrl),
  _height(height)
{

}

IlwisObjectCreatorModel::~IlwisObjectCreatorModel()
{

}

QString IlwisObjectCreatorModel::label() const
{
    return _label;
}

QString IlwisObjectCreatorModel::componentUrl() const
{
    return _creatorUrl;
}

QString IlwisObjectCreatorModel::type() const
{
    return QString::number(_type);
}

int IlwisObjectCreatorModel::height() const
{
    return _height;
}

