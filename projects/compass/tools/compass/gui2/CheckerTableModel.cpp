#include "rose.h"
#include "compassInterface.h"

#include "CheckerTableModel.h"


#include <QDebug>

CheckerTableModel::CheckerTableModel(CompassInterface * ci,QObject * par)
    : QAbstractTableModel(par),
      compInterface(ci),
      colorOk(Qt::green),
      colorFailed(Qt::red),
      colorTestFailed(Qt::red),
      iconOk(":/Compass/icons/flag-green.png"),
      iconFailed(":/Compass/icons/flag-red.png"),
      iconTestFailed(":/Compass/icons/dialog-close.png")
{
    //colorOk= colorOk.darker(150);
    colorOk.setAlpha(100);
    colorFailed.setAlpha(100);
}


Qt::ItemFlags CheckerTableModel::flags (const QModelIndex & ind) const
{
    Qt::ItemFlags flags;
    flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if(ind.column()==0)
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}


const CompassChecker * CheckerTableModel::getCompassChecker(const QModelIndex & ind)
{
    Q_ASSERT(ind.row() < compInterface->numCheckers());
    return compInterface->getCompassChecker(ind.row());
}


bool CheckerTableModel::setData (const QModelIndex & ind, const QVariant & value, int role)
{
    if(ind.isValid() &&
       ind.row() < compInterface->numCheckers() &&
       ind.column() == 0 &&
       role==Qt::CheckStateRole )
    {
        bool enabled = (value==Qt::Checked);
        compInterface->getCompassChecker(ind.row())->setEnabled(enabled);
        dataChanged(ind,ind);
        return true;
    }

    return false;
}



QVariant CheckerTableModel::data(const QModelIndex & ind, int role) const
{
    if( ! ind.isValid())
        return QVariant();

    Q_ASSERT(ind.row() < compInterface->numCheckers());

    const CompassChecker * curChecker = compInterface->getCompassChecker(ind.row());
    bool hasViolations = curChecker->getNumViolations() >0 || curChecker->hasErrorMsg();

    if(role == Qt::DisplayRole)
    {
        switch(ind.column())
        {
            case 0: return QString(curChecker->getName().c_str());
            case 1: return curChecker->hasErrorMsg() ? QVariant(QString("-")) :    QVariant(curChecker->getEllapsedTime());
            case 2: return curChecker->hasErrorMsg() ? QVariant(QString("Error")) :QVariant(curChecker->getNumViolations());
            default: return QVariant();
        }
    }
    else if ( role == Qt::TextAlignmentRole)
    {
        switch(ind.column())
        {
            case 0: return QVariant(Qt::AlignLeft | Qt::AlignVCenter) ;
            case 1: return Qt::AlignCenter;
            case 2: return Qt::AlignCenter;
            default: return QVariant();
        }
    }
    else if ( role == Qt::CheckStateRole && ind.column() == 0)
    {
        return curChecker->isEnabled() ? Qt::Checked : Qt::Unchecked;
    }
    else if ( role == Qt::BackgroundColorRole && curChecker->wasRun())
    {
        if(curChecker->hasErrorMsg())
            return QVariant();

        return hasViolations ? colorFailed : colorOk;
    }
    else if ( role == Qt::DecorationRole && ind.column() == 0 && curChecker->wasRun())
    {
        if(curChecker->hasErrorMsg())
            return iconTestFailed;

        return hasViolations ? iconFailed : iconOk;
    }
    else
        return QVariant();

}

QVariant CheckerTableModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && section < 3)
    {
        if(role == Qt::DisplayRole)
        {
            switch(section)
            {
                case 0: return QObject::tr("Checker");
                case 1: return QObject::tr("Time");
                case 2: return QObject::tr("Violations");
                default: return QVariant();
            }
        }
        else if ( role == Qt::DecorationRole )
        {
            switch(section)
            {
                case 0: return QIcon(":/Compass/icons/document-properties.png");
                case 1: return QIcon(":/Compass/icons/chronometer.png");
                case 2: return QIcon(":/Compass/icons/flag-yellow.png");
                default: return QVariant();
            }
        }
    }

    return QVariant();
}

int  CheckerTableModel::rowCount(const QModelIndex &) const
{
    compInterface->numCheckers();
}


int  CheckerTableModel::columnCount (const QModelIndex &) const
{
    //Columns:
    // - checker name
    // - checker runtime
    // - violations
    return 3;
}






// ------------------------ FILTER MODEL ------------------------------------------


bool CheckerFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    CheckerTableModel * srcModel = dynamic_cast<CheckerTableModel*>(sourceModel());

    if(!srcModel)
    {
        qDebug()<< "CheckerFilterModel can only be used with CheckerTabelModel!";
        return false;
    }

    QModelIndex ind = srcModel->index(sourceRow,0,sourceParent);
    const CompassChecker * checker = srcModel->getCompassChecker(ind);

    bool origResult = QSortFilterProxyModel::filterAcceptsRow(sourceRow,sourceParent);

    bool newRes=origResult;

    if(failed && checker->hasErrorMsg())
        return origResult;

    if(!failed )
        newRes = newRes & !checker->hasErrorMsg();

    if(!passed )
        newRes = newRes & (checker->getNumViolations() > 0);

    if(!unselected )
        newRes = newRes & (srcModel->data(ind,Qt::CheckStateRole) != Qt::Unchecked);

    if(!withVios)
        newRes = newRes & (checker->getNumViolations() == 0);


    return newRes;

}








