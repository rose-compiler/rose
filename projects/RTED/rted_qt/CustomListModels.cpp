#include "CustomListModels.h"

#include <QDebug>

// -------------------------------- RuntimeVariablesModel ----------------------------


RuntimeVariablesModel::RuntimeVariablesModel(RuntimeVariablesType * a, int as,QObject * p)
    : QAbstractListModel(p),arr(a),arrSize(as)
{
}


int RuntimeVariablesModel::rowCount (const QModelIndex & ) const
{
    return arrSize;
}

QVariant RuntimeVariablesModel::data (const QModelIndex & ind, int role) const
{
    if(! ind.isValid() )
        return QVariant();

    if(role == Qt::DisplayRole)
        return QString( arr[ind.row()].name );

    return QVariant();
}

RuntimeVariablesType * RuntimeVariablesModel::getRuntimeVariable(const QModelIndex & ind)
{
    Q_ASSERT(ind.row()<arrSize);
    return & arr[ind.row()];
}

// -------------------------------- MemoryTypeModel --------------------------------

MemoryTypeModel::MemoryTypeModel(MemoryType * a, int as,QObject * p)
    : QAbstractListModel(p),arr(a),arrSize(as)
{
}


int MemoryTypeModel::rowCount (const QModelIndex &  ) const
{
    return arrSize;
}

QVariant MemoryTypeModel::data (const QModelIndex & ind, int role) const
{
    if(! ind.isValid() )
        return QVariant();

    if(role == Qt::DisplayRole)
        return static_cast<unsigned int>( arr[ind.row()].address);

    return QVariant();
}



MemoryType * MemoryTypeModel::getMemoryType(const QModelIndex & ind)
{
    Q_ASSERT(ind.row() < arrSize);
    return & arr[ind.row()];
}
