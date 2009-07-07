#include "BAstModel.h"
#include "BAstNode.h"

#include <QDir>
#include <QDebug>

#include "AstFilters.h"


BAstModel::BAstModel(QObject * par)
    : ItemTreeModel(par),curFilter(NULL),curNode(NULL)
{
}

BAstModel::~BAstModel()
{
}



void BAstModel::setNode(SgNode * node)
{
    curNode=node;
    updateModel();
}

void BAstModel::setFilter(AstFilterInterface * filter)
{
    curFilter = filter->copy();
    updateModel();
}

void BAstModel::updateModel()
{
    if(curNode==NULL)
    {
        setRoot(NULL);
        return;
    }

    ItemTreeNode * treeRoot = BAstNode::generate(curNode,curFilter);
    setRoot(treeRoot);
}

SgNode * BAstModel::getNodeFromIndex(const QModelIndex & ind)
{
    BAstNode * ret = static_cast<BAstNode *>(ind.internalPointer());
    Q_ASSERT(ret!=NULL);
    return ret->sgNode();
}


QVariant BAstModel::data (const QModelIndex & ind, int role) const
{
    if(! ind.isValid())
        return QVariant();

    ItemTreeNode * node = static_cast<ItemTreeNode*> (ind.internalPointer());
    Q_ASSERT(node!=NULL);

    BAstNode * bNode=dynamic_cast<BAstNode*> (node);
    Q_ASSERT(bNode!=NULL);

    return ItemTreeModel::data(ind,role);
}
