#ifndef VIOLATIONSTABLEMODEL_H
#define VIOLATIONSTABLEMODEL_H

#include <QAbstractTableModel>

#include <QIcon>
#include <QColor>


class CompassChecker;
class CompassViolation;

class ViolationsTableModel : public QAbstractTableModel
{
    public:
        ViolationsTableModel(const CompassChecker * ch,QObject * parent = 0);
        virtual ~ViolationsTableModel() {}


        virtual Qt::ItemFlags flags (const QModelIndex & index) const;
        virtual QVariant data       (const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        virtual int  rowCount    (const QModelIndex & parent = QModelIndex() ) const;
        virtual int  columnCount (const QModelIndex & parent = QModelIndex() ) const;

        const CompassViolation * getViolation(const QModelIndex & ind);

        const CompassChecker * getCurChecker() const { return curChecker; }

    protected:
        const CompassChecker * curChecker;
        bool hasError;
};



#endif
