#ifndef CHECKERTABLEMODEL_H
#define CHECKERTABLEMODEL_H

#include <QAbstractTableModel>

#include <QIcon>
#include <QColor>


class CompassInterface;
class CompassChecker;
class CheckerTableModel : public QAbstractTableModel
{
    public:
        CheckerTableModel(CompassInterface * ci,QObject * parent = 0);
        virtual ~CheckerTableModel() {}


        virtual Qt::ItemFlags flags (const QModelIndex & index) const;
        virtual QVariant data       (const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual bool     setData    (const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

        virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        virtual int  rowCount    (const QModelIndex & parent = QModelIndex() ) const;
        virtual int  columnCount (const QModelIndex & parent = QModelIndex() ) const;

        const CompassChecker * getCompassChecker(const QModelIndex & ind);

    protected:
        CompassInterface * compInterface;


        QColor colorOk;
        QColor colorFailed;
        QColor colorTestFailed;

        const QIcon iconOk;
        const QIcon iconFailed;
        const QIcon iconTestFailed;
};



#include <QSortFilterProxyModel>
class CheckerFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    public:

        CheckerFilterModel(QObject *p = 0)
            : QSortFilterProxyModel(p),
              failed(true),passed(true),unselected(true),withVios(true)
        {}

    public slots:
        void showFailedTests(bool b)          { failed     = b; invalidateFilter(); }
        void showTestsWithViolations(bool b)  { withVios   = b; invalidateFilter(); }
        void showPassedTests(bool b)          { passed     = b; invalidateFilter(); }
        void showUnselectedTests(bool b)      { unselected = b; invalidateFilter(); }


    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        bool failed;
        bool passed;
        bool unselected;
        bool withVios;
};


#endif
