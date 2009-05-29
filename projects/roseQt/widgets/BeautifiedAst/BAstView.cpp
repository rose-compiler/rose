#include "rose.h"

#include "BAstView.h"
#include "BAstModel.h"

BAstView::BAstView(QWidget * par)
    : QTreeView(par)
{
    model = new BAstModel(this);
    setModel(model);

    connect(this, SIGNAL( clicked(const QModelIndex & )),
            this, SLOT(viewClicked(const QModelIndex&)));
}

BAstView::~BAstView()
{
}


void BAstView::setNode(SgNode * node)
{
    model->setNode(node);
}

void BAstView::setFilter(AstFilterInterface * filter)
{
    model->setFilter(filter);
}

void BAstView::viewClicked(const QModelIndex & ind)
{
    if(! ind.isValid())
        return;

    SgNode * node= model->getNodeFromIndex(ind);
    emit clicked(node);

    SgLocatedNode* sgLocNode = isSgLocatedNode(node);
    if(sgLocNode)
    {
        Sg_File_Info* start = sgLocNode->get_startOfConstruct();
        Sg_File_Info *end   = sgLocNode->get_endOfConstruct();

        emit clicked(QString(start->get_filenameString().c_str()),
                     start->get_line(),
                     start->get_col(),
                     end->get_line(),
                     end->get_col());
    }
    else
        emit clicked(QString(),-1,-1,-1,-1);
}
