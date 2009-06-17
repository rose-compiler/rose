#ifndef CUSTOM_LIST_MODEL_H
#define CUSTOM_LIST_MODEL_H

#include <QAbstractListModel>

#include "RuntimeSystem.h"


class RuntimeVariablesModel : public QAbstractListModel
{
    public:
        RuntimeVariablesModel(RuntimeVariablesType * arr, int arrSize,QObject * parent = 0);
        ~RuntimeVariablesModel() {}

        virtual int rowCount (const QModelIndex & parent = QModelIndex() ) const;
        virtual QVariant data (const QModelIndex & index, int role = Qt::DisplayRole ) const;

        RuntimeVariablesType * getRuntimeVariable(const QModelIndex & ind);

    protected:
        RuntimeVariablesType * arr;
        int arrSize;
};


class MemoryTypeModel : public QAbstractListModel
{
    public:
        MemoryTypeModel(MemoryType * arr, int arrSize, QObject * parent);

        virtual int rowCount (const QModelIndex & parent = QModelIndex() ) const;
        virtual QVariant data (const QModelIndex & index, int role = Qt::DisplayRole ) const;

        MemoryType * getMemoryType(const QModelIndex & ind);

    protected:
        MemoryType * arr;
        int arrSize;
};


#endif
