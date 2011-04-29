#include "ItemTreeNode.h"

#include "ItemTreeModel.h"

#include <QList>
#include <QDebug>

ItemTreeNode::ItemTreeNode()
        : parent(NULL), notifyModel(NULL)
{}

ItemTreeNode::~ItemTreeNode()
{
        qDeleteAll(children);
}

int ItemTreeNode::addChild(ItemTreeNode * c)
{
    int newIndex=children.size();
    addChildAt(c,newIndex);
    return newIndex;
}

void ItemTreeNode::addChildAt(ItemTreeNode *c, int id)
{
    Q_ASSERT(id>=0 && id <= children.size());

    if(notifyModel)
        notifyModel->beginInsertRows(notifyModel->index(this),id,id);


    children.insert(id,c);
    c->parent=this;
    c->setNotifyModel(notifyModel);

    if(notifyModel)
        notifyModel->endInsertRows();

}

void ItemTreeNode::addChildren(const QList<ItemTreeNode*> & list)
{

    int sizeBefore=children.size();
    int sizeAfter = children.size()+list.size();
    if(notifyModel)
        notifyModel->beginInsertRows(notifyModel->index(this),sizeBefore,sizeAfter-1);

    foreach(ItemTreeNode * n, list)
    {
        children.push_back(n);
        children.back()->parent=this;
        children.back()->setNotifyModel(notifyModel);
    }

    if(notifyModel)
        notifyModel->endInsertRows();

}


void ItemTreeNode::removeChild(int id)
{
    if(notifyModel)
        notifyModel->beginRemoveRows(notifyModel->index(this),id,id);


    delete children[id];
    children.removeAt(id);

    if(notifyModel)
        notifyModel->endRemoveRows();
}


void ItemTreeNode::removeAllChildren()
{
    if(children.isEmpty())
        return;

    if(notifyModel)
        notifyModel->beginRemoveRows(notifyModel->index(this),0,children.size()-1);

    qDeleteAll(children);
    children.clear();

    if(notifyModel)
        notifyModel->endRemoveRows();

}


void  ItemTreeNode::takeAllChildren(QList<ItemTreeNode*> & output)
{
    output.clear();

    if(children.isEmpty())
        return;


    if(notifyModel)
        notifyModel->beginRemoveRows(notifyModel->index(this),0,children.size()-1);

    foreach(ItemTreeNode * n, children )
        output.push_back(n);

    children.clear();

    if(notifyModel)
        notifyModel->endRemoveRows();

}

int ItemTreeNode::getRow() const
{
        //assume that there is only one node on highest level (one root)
        if(parent==NULL)
                return 0;

        //const cast is safe, indexOf does no write access
        ItemTreeNode * nonConstThis = const_cast<ItemTreeNode*>(this);
        return parent->children.indexOf(nonConstThis);
}

QVariant ItemTreeNode::data(int role, int column) const
{
        return QString("NoData");
}

int ItemTreeNode::getChildId(ItemTreeNode * child)
{
    return children.indexOf(child);
}


QStringList ItemTreeNode::sectionHeader() const
{
    return QStringList();
}

Qt::ItemFlags ItemTreeNode::flags (int column) const
{
    return  Qt::ItemIsDragEnabled |
            Qt::ItemIsDropEnabled |
            Qt::ItemIsEnabled |
            Qt::ItemIsSelectable;
}


void ItemTreeNode::setNotifyModel(ItemTreeModel * m)
{
    notifyModel=m;
    foreach(ItemTreeNode * child, children)
        child->setNotifyModel(m);
}



// -------------------- Property Value Node -----------------------------------------

PropertyValueNode::PropertyValueNode(const QString & p, const QVariant & v)
    : prop(p),val(v),firstColumnSpanned(false)
{
}

QVariant PropertyValueNode::data(int role, int column) const
{
    switch(role)
    {
        case Qt::DisplayRole:
            if      (column==0)  return prop;
            else if (column==1)  return val;
        case Qt::DecorationRole:
            if( column ==0)      return icon;
        default:
            return QVariant();
    }
    return QVariant();
}

QStringList PropertyValueNode::sectionHeader() const
{
    return QStringList() << QObject::tr("Property") << QObject::tr("Value");
}


