#include "rose.h"
#include "ItemTreeModel.h"

#include "ItemTreeNode.h"

#include <QTreeView>
#include <QDebug>
#include <QMimeData>

ItemTreeModel::ItemTreeModel(QObject * par)
    : QAbstractItemModel(par),root(NULL),autoDelete(true)
{

}

ItemTreeModel::~ItemTreeModel()
{
    if(root && autoDelete)
        delete root;
}

void ItemTreeModel::setRoot(ItemTreeNode * n)
{
    // when setFixedRoot was called, you cannot call this function!
    // see documentation of setFixedRoot
    Q_ASSERT(autoDelete);


    if(root)
    {
        delete root;
        root=NULL;
    }

    root = n;
    headerCap.clear();

    if(root)
    {
        headerCap = root->sectionHeader();
        root->setNotifyModel(this);
    }

    reset();
}

void ItemTreeModel::setFixedRoot(ItemTreeNode * n)
{
    Q_ASSERT(root==NULL);
    Q_ASSERT(autoDelete); //you can call this function only once!
    autoDelete=false;

    root = n;
    headerCap.clear();
    headerCap = root->sectionHeader();
    root->setNotifyModel(this);


    reset();
}


Qt::ItemFlags ItemTreeModel::flags (const QModelIndex & ind) const
{
    if(!ind.isValid())
        return Qt::ItemIsDropEnabled;

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


QVariant ItemTreeModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation==Qt::Horizontal)
    {
        if(section < headerCap.size() )
            return headerCap[section];
    }
    return QVariant();

}

int  ItemTreeModel::columnCount (const QModelIndex & par) const
{
    return headerCap.size();

}


QModelIndex ItemTreeModel::index( const ItemTreeNode * node, int column)
{
    if(!node || node==root)
        return QModelIndex();

    return createIndex( node->getRow(), column, (void*)node);
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


void ItemTreeModel::setupView(QTreeView * view, const QModelIndex & subTreeInd)
{
    Q_ASSERT(view);

    const QAbstractItemModel * model = view->model();

    if(!model)
        return;

    //because of function setFirstColumnSpanned
    #if QT_VERSION >= 0x040400
    for(int r=0; r < model->rowCount(subTreeInd); r++ )
    {
        //qDebug() << "Cur Subtree:" << subTreeInd << " row:" << r;
        QModelIndex curInd = model->index(r,0,subTreeInd);

        ItemTreeNode * node = static_cast<ItemTreeNode*> (curInd.internalPointer());
        view->setFirstColumnSpanned( r,subTreeInd,node->isFirstColumnSpanned() );

        setupView(view,curInd);
    }
    #endif
}

/*
QStringList ItemTreeModel::mimeTypes() const
{
    QStringList types;
    types << "SgNode";
    return types;
}


QMimeData * ItemTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes)
    {
        if (index.isValid())
        {
            SgNode * node = qvariant_cast<SgNode*>( data(index,SgNodeRole));
            if(!node)
                continue;

            //stream.writeBytes((const char*)node,sizeof(SgNode*));
            //stream << data(index,SgNodeRole);
            stream.writeRawData((char*)&node,sizeof(SgNode*));
        }
    }

    mimeData->setData("SgNode", encodedData);
    return mimeData;
}

bool ItemTreeModel::dropMimeData(const QMimeData *data,Qt::DropAction action,
                                 int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("SgNode"))
        return false;

    QByteArray encodedData = data->data("SgNode");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd())
    {
        SgNode * node = NULL;
        //SgNode * node = qvariant_cast<SgNode*>(v);
        int bytesRead = stream.readRawData((char*)&node,sizeof(SgNode*));
        Q_ASSERT(bytesRead==sizeof(SgNode*));

        if(node)
            qDebug() << "Node dropped " << node << node->class_name().c_str();
        else
            qDebug() << "Received null node";
    }

    return true;
}
*/
