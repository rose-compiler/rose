#ifndef ITEMTREEMODEL_H
#define ITEMTREEMODEL_H

#include <QStringList>
#include <QAbstractItemModel>

class ItemTreeNode;
class ItemTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        ItemTreeModel(QObject * parent=NULL);
        virtual ~ItemTreeModel();


        virtual Qt::ItemFlags flags (const QModelIndex & index) const;
        virtual QVariant data       (const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        virtual int rowCount    (const QModelIndex & parent = QModelIndex() ) const;
        virtual int columnCount (const QModelIndex & parent = QModelIndex() ) const;


        virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
        virtual QModelIndex parent ( const QModelIndex & index ) const;

        QStringList & headerCaptions() { return headerCap; }

    protected:
        void setRoot(ItemTreeNode * n);


    private:
        // Root of the tree; is not displayed only its children
        ItemTreeNode * root;

        QStringList headerCap;
};

#endif
