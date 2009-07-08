#include "rose.h"
#include "AstGraphWidget.h"

#include "SageMimeData.h"

#include <cmath>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QDropEvent>


#include <QDebug>

#include "TreeLayoutGenerator.h"
#include "AstFilters.h"
#include "DisplayGraphNode.h"

#include "SgNodeUtil.h"
#include <CallGraph.h>



AstGraphWidget::AstGraphWidget(QWidget * par)
	: 	QGraphicsView(par),
		scene(NULL),
		root(NULL),
		curFilter(NULL),
		curSgTreeNode(NULL)
{
	scene=new QGraphicsScene();

	setRenderHints(QPainter::Antialiasing);

	setScene(scene);
	setDragMode(QGraphicsView::ScrollHandDrag);
}

AstGraphWidget::~AstGraphWidget()
{
    if(curFilter)
        delete curFilter;

    if(root)
        delete root;

    delete scene;
}


void AstGraphWidget::setFilter(AstFilterInterface * filter)
{
	if(curFilter)
		delete curFilter;

    if( filter != NULL )
        curFilter = filter->copy();
    else
        curFilter = NULL;

	setNode(curSgTreeNode);
}



void AstGraphWidget::setNode(SgNode * node)
{
    if(root)
        delete root;

	root=NULL;

	curSgTreeNode=node;

	if(curSgTreeNode==NULL)
		return;

#if 0
	DisplayTreeGenerator gen;
	root = gen.generateTree(node,curFilter);

	qDebug() << "Simplifying Tree";
	DisplayTreeNode::simplifyTree(root);
	qDebug() << "Done";

	TreeLayoutGenerator layouter;
	layouter.layoutTree(root);

	root->setScene(scene);
#else
	/*
    DisplayGraph *g = DisplayGraph::generateLargeTestGraph(scene);
    g->controlWidget()->show();
    */

    SgProject * proj = getProjectOf(node);
    if(!proj)
        return;

    CallGraphBuilder cgb (proj);
    cgb.buildCallGraph();


    SgIncidenceDirectedGraph* incidenceGraph = cgb.getGraph();


    //DisplayGraph * g = DisplayGraph::generateCallGraph(scene,incidenceGraph);
    DisplayGraph * g = DisplayGraph::buildCallGraphForFunction(incidenceGraph,node,10,scene,NULL);



    //g->setDelta(1e-4);
    g->setOptimalDistance(80);

    qreal update=1e10;
    int iter=0;
    while(update>0.5 && iter < 10000)
    {
        update = g->springBasedLayoutIteration(1e-4);
        qDebug() << "Iteration" << iter << "Update" << update;
        iter++;
    }
    qDebug() << "Layout done in" << iter << "iterations";

    root=g;

    g->controlWidget()->show();

#endif
}




void AstGraphWidget::setFileFilter(int id)
{
    if(id==-1)
        setFilter(NULL);
    else
        setFilter(new AstFilterFileById(id));
}



void AstGraphWidget::mousePressEvent(QMouseEvent * ev)
{
	DisplayTreeNode * node = dynamic_cast<DisplayTreeNode*>(itemAt(ev->pos()));

	if(node)
	{
		SgNode * sgNode = node->getSgNode();
		clicked(sgNode);

		SgLocatedNode* sgLocNode = isSgLocatedNode(sgNode);
		if(sgLocNode)
		{
			Sg_File_Info* fi = sgLocNode->get_file_info();

			emit clicked(QString(fi->get_filenameString().c_str()),
						 fi->get_line(),fi->get_col());
		}
	}

	QGraphicsView::mousePressEvent(ev);
}

void AstGraphWidget::wheelEvent(QWheelEvent *ev)
{
    scaleView(	std::pow((double)2, ev->delta() / 240.0));
}



void AstGraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}






// ---------------------- Drop Functionality -----------------------------------

void AstGraphWidget::dragEnterEvent(QDragEnterEvent * ev)
{
    if (ev->mimeData()->hasFormat(SG_NODE_MIMETYPE))
    {
        if(this != ev->source())
            ev->acceptProposedAction();
    }
}


void AstGraphWidget::dropEvent(QDropEvent *ev)
{
    if(ev->source()==this)
        return;

    SgNode * node = getGeneralNode(ev->mimeData());

    if(node)
    {
        setNode(node);
        ev->acceptProposedAction();
    }
}

void AstGraphWidget::dragMoveEvent( QDragMoveEvent * ev)
{
    QWidget::dragMoveEvent(ev);
}












