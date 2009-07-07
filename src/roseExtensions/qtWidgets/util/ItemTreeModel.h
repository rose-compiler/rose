#ifndef ITEMTREEMODEL_H
#define ITEMTREEMODEL_H

#include <QStringList>
#include <QAbstractItemModel>

#include "ItemModelHelper.h"

class QTreeView;
class ItemTreeNode;
class ItemTreeModel : public QAbstractItemModel
{
    // ItemTreeNode is friend because it calls beginInsertRows etc
    friend class ItemTreeNode;

    Q_OBJECT

    public:
        ItemTreeModel(QObject * parent=NULL);
        virtual ~ItemTreeModel();

        virtual Qt::ItemFlags flags (const QModelIndex & index) const;
        virtual QVariant data       (const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        virtual int rowCount    (const QModelIndex & parent = QModelIndex() ) const;
        virtual int columnCount (const QModelIndex & parent = QModelIndex() ) const;

        virtual QModelIndex index( const ItemTreeNode * node, int column=0);

        virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
        virtual QModelIndex parent ( const QModelIndex & index ) const;

        //n may be null -> empty model
        void setRoot(ItemTreeNode * n);

        /// Call this version if setRoot is called only once
        /// the ownership of n stays at the caller! delete the ItemTreeNode yourself
        void setFixedRoot(ItemTreeNode *n);

        /// This function changes the view according to the information
        /// in the ItemTreeNodes
        /// currently only used for isFirstColumnSpanned
        /// bad design, because view should get all information directly from model
        /// but with the isFirstColumnSpanned there seems to be no other possibility
        static void setupView(QTreeView * view, const QModelIndex & subTree= QModelIndex() );

        /*
        virtual QStringList mimeTypes () const;
        virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
        virtual bool dropMimeData ( const QMimeData * data, Qt::DropAction action,
                                    int row, int column, const QModelIndex& parent);
        */
    private:
        // Root of the tree; is not displayed only its children
        ItemTreeNode * root;

        /// if true this class owns root (has to be freed)
        bool autoDelete;

        QStringList headerCap;
};

#endif
