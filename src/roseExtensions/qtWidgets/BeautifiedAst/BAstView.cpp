#include "rose.h"

#include "BAstView.h"
#include "ItemTreeModel.h"
#include "BAstNode.h"

BAstView::BAstView(QWidget * par)
    : RoseTreeView(par)
{
    model = new ItemTreeModel(this);

    updateModel();

    setModel(model);

    // call setNode when node is dropped
    connect(this,SIGNAL(nodeDropped(SgNode *, const QPoint &)),
                     SLOT(setNode( SgNode *)));

}


BAstView::~BAstView()
{
}

void BAstView::updateModel()
{
    ItemTreeNode * root  = BAstNode::generate(curNode,curFilter);
    model->setRoot(root);
}

