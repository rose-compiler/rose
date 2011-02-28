#include "sage3basic.h"
#include "AstBrowserWidget.h"

#include "QtAstModel.h"
#include "AstFilters.h"

#include <QDebug>

AstBrowserWidget::AstBrowserWidget(QWidget*par)
        : RoseTreeView(par)
{
        model=new QtAstModel(NULL);
        setModel(model);

        connect(this,SIGNAL(nodeDropped(SgNode *, const QPoint &)),
                     SLOT(setNode( SgNode *)));
}


AstBrowserWidget::AstBrowserWidget(SgNode * node, QWidget * par)
        : RoseTreeView(par)
{
        model=new QtAstModel(node);
        setModel(model);
        connect(this,SIGNAL(nodeActivated(const QModelIndex &)),SLOT(viewClicked(const QModelIndex &)));
        connect(this,SIGNAL(nodeActivatedAlt(const QModelIndex &)),SLOT(viewDoubleClicked(const QModelIndex &)));
}

AstBrowserWidget::~AstBrowserWidget()
{
        delete model;
}


void AstBrowserWidget::setNode(SgNode * node)
{
        model->setNode(node);
}

void AstBrowserWidget::setFilter(AstFilterInterface * newFilter)
{
    if( newFilter != NULL )
        model->setFilter( newFilter->copy() );
    else
        model->setFilter( new AstFilterAll() );
}

void AstBrowserWidget::setFileFilter(int id)
{
    if(id==-1)
        setFilter(NULL);
    else
        setFilter(new AstFilterFileById(id));
}
