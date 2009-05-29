#include "ItemTreeModel.h"

#include "ItemTreeNode.h"


ItemTreeModel::ItemTreeModel(QObject * par)
    : QAbstractItemModel(par),root(NULL)
{

}

ItemTreeModel::~ItemTreeModel()
{
    if(root)
        delete root;
}


void ItemTreeModel::setRoot(ItemTreeNode * n)
{
    if(root)
        delete root;

    root= n;

    reset();
}

Qt::ItemFlags ItemTreeModel::flags (const QModelIndex & ind) const
{
    if(!ind.isValid())
        return 0;

    ItemTreeNode * node = static_cast<ItemTreeNode*> (ind.internalPointer());
    return node->flags(ind.column());
}


QVariant ItemTreeModel::data (const QModelIndex & ind, int role) const
{
    if(! ind.isValid())
        return QVariant();

    ItemTreeNode * node = static_cast<ItemTreeNode*> (ind.internalPointer());
    Q_ASSERT(node!=NULL);

    return node->data(role,ind.column());
}


QVariant ItemTreeModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation==Qt::Horizontal)
    {
        if(section < headerCap.size() )
            return headerCap[section];
    }
    return QVariant();

}

int ItemTreeModel::rowCount(const QModelIndex & par) const
{
    //Root of the tree
    if(! par.isValid())
        return root ? root->childrenCount() : 0;

    //Only the first column has children
    if(par.column()>=1)
        return 0;

    ItemTreeNode * parNode = static_cast<ItemTreeNode*> (par.internalPointer());
    return parNode->childrenCount();
}

int  ItemTreeModel::columnCount (const QModelIndex & par) const
{
    return headerCap.size();
}

QModelIndex ItemTreeModel::index ( int row, int column, const QModelIndex & par) const
{
    if(row<0 || column<0)
        return QModelIndex();

    if(row>= rowCount(par) || column>=columnCount(par))
        return QModelIndex();

    if(!par.isValid())
    {
        Q_ASSERT(root->child(row) != NULL);
        return createIndex(row,column,root->child(row));
    }
    else
    {
        ItemTreeNode * parNode = static_cast<ItemTreeNode*> (par.internalPointer());
        Q_ASSERT(parNode->child(row) != NULL);
        return createIndex(row, column, parNode->child(row));
    }
}


QModelIndex ItemTreeModel::parent(const QModelIndex & ind) const
{
    if(! ind.isValid())
        return QModelIndex();


    ItemTreeNode * node = static_cast<ItemTreeNode*> (ind.internalPointer());
    ItemTreeNode * parentNode = node->getParent();

    if(parentNode == root)
        return QModelIndex();
    else
    {
        Q_ASSERT(parentNode!=NULL);
        return createIndex(parentNode->getRow(),0,parentNode);
    }
}

