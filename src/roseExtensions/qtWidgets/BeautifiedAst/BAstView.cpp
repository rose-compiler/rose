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

    connect(this, SIGNAL( clicked(const QModelIndex & )),
            this, SLOT(viewClicked(const QModelIndex&)));
}


BAstView::~BAstView()
{
}

void BAstView::updateModel()
{
    ItemTreeNode * root  = BAstNode::generate(curNode,curFilter);
    model->setRoot(root);
}

