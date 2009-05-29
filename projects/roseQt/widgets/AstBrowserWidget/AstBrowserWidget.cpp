#include "rose.h"
#include "AstBrowserWidget.h"

#include "QtAstModel.h"
#include "AstFilters.h"

#include <QDebug>

AstBrowserWidget::AstBrowserWidget(QWidget*par)
	: QTreeView(par)
{
	model=new QtAstModel(NULL);
	setModel(model);
	connect(this,SIGNAL(clicked(const QModelIndex &)),SLOT(viewClicked(const QModelIndex &)));
	connect(this,SIGNAL(doubleClicked(const QModelIndex &)),SLOT(viewDoubleClicked(const QModelIndex &)));
}


AstBrowserWidget::AstBrowserWidget(SgNode * node, QWidget * par)
	: QTreeView(par)
{
	model=new QtAstModel(node);
	setModel(model);
	connect(this,SIGNAL(clicked(const QModelIndex &)),SLOT(viewClicked(const QModelIndex &)));
	connect(this,SIGNAL(doubleClicked(const QModelIndex &)),SLOT(viewDoubleClicked(const QModelIndex &)));
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

void AstBrowserWidget::viewClicked(const QModelIndex & ind)
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

void AstBrowserWidget::viewDoubleClicked( const QModelIndex & ind )
{
    if(! ind.isValid() )
        return;

    SgNode * node = model->getNodeFromIndex( ind );
    emit doubleClicked( node );
}
