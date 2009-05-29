#include "rose.h"
#include "compassInterface.h"

#include "ViolationsTableModel.h"

#include <QFileInfo>
#include <QDebug>


ViolationsTableModel::ViolationsTableModel(const CompassChecker * ch,QObject * par)
    : QAbstractTableModel(par),
      curChecker(ch)
{
    hasError = curChecker->hasErrorMsg() && (curChecker->getNumViolations() == 0);
}


Qt::ItemFlags ViolationsTableModel::flags (const QModelIndex & ind) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

const CompassViolation * ViolationsTableModel::getViolation(const QModelIndex & ind)
{
    if(hasError)
        return NULL;

    Q_ASSERT(ind.row() < curChecker->getNumViolations());
    return curChecker->getViolation(ind.row());
}


QVariant ViolationsTableModel::data(const QModelIndex & ind, int role) const
{
    if( ! ind.isValid())
        return QVariant();


    if(hasError)
    {
        if      (role == Qt::DisplayRole)    return curChecker->getErrorMsg().c_str();
        else if (role == Qt::DecorationRole) return QIcon(":/Compass/icons/dialog-close.png");
        else    return QVariant();
    }



    Q_ASSERT(ind.row() < curChecker->getNumViolations());

    const CompassViolation * violation =curChecker->getViolation(ind.row());

    if(role == Qt::DisplayRole)
    {
        switch(ind.column())
        {
            case 0: return QFileInfo(QString(violation->getFilename().c_str())).fileName();
            case 1: return QVariant(violation->getLineStart());
            case 2: return QString(violation->getDesc().c_str());
            default: return QVariant();
        }
    }
    else if ( role == Qt::TextAlignmentRole)
    {
        switch(ind.column())
        {
            case 0: return Qt::AlignCenter;
            case 1: return Qt::AlignCenter;
            case 2: return QVariant(Qt::AlignLeft | Qt::AlignVCenter) ;
            default: return QVariant();
        }
    }
    else if ( role == Qt::DecorationRole && ind.column() ==0)
       return QIcon(":/Compass/icons/flag-red.png");
    else
        return QVariant();

}

QVariant ViolationsTableModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if(hasError && orientation == Qt::Horizontal && role==Qt::DisplayRole && section==0)
       return tr("Error Description");


    if(!hasError && orientation == Qt::Horizontal && section < 3)
    {
        if(role == Qt::DisplayRole)
        {
            switch(section)
            {
                case 0: return QObject::tr("File");
                case 1: return QObject::tr("Line");
                case 2: return QObject::tr("Description");
                default: return QVariant();
            }
        }
    }

    return QVariant();
}

int  ViolationsTableModel::rowCount(const QModelIndex &) const
{
    if(hasError)
        return 1;
    else
        return curChecker->getNumViolations();
}


int  ViolationsTableModel::columnCount (const QModelIndex &) const
{
    if(hasError)
        return 1;


    //Columns:
    // - File
    // - line
    // - violation description
    return 3;
}
