#include "rose.h"

#include "RoseGraphicsView.h"


#include "AstFilters.h"
#include "SageMimeData.h"

#include <QApplication>
#include <QDataStream>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QWheelEvent>

#include "DisplayNode.h"

#include <cmath>

#include <QDebug>


RoseGraphicsView::RoseGraphicsView(QWidget * par)
    : QGraphicsView(par),
      dragEnabled(true),
      dropEnabled(true),
      curFilter(NULL),
      curNode(NULL)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    setRenderHints(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);

    connect ( this, SIGNAL(nodeDropped(SgNode*,QPoint)),
              this, SLOT(setNode(SgNode*)) );

}


RoseGraphicsView::~RoseGraphicsView()
{
    if(curFilter)
        delete curFilter;
}
void RoseGraphicsView::setNode(SgNode * node)
{
    curNode = node;
    updateGraph(curNode,curFilter);
}

void RoseGraphicsView::setFilter(AstFilterInterface * filter)
{
    if(curFilter)
        delete curFilter;

    curFilter = filter->copy();
    updateGraph(curNode,curFilter);
}



void RoseGraphicsView::setFileFilter(int id)
{
    if(id==-1)
        setFilter(NULL);
    else
        setFilter(new AstFilterFileById(id));
}


void RoseGraphicsView::mousePressEvent(QMouseEvent * ev)
{
    DisplayNode * node = dynamic_cast<DisplayNode*>(itemAt(ev->pos()));

    if(node)
    {
        SgNode * sgNode = node->getSgNode();
        nodeActivated(sgNode);

        SgLocatedNode* sgLocNode = isSgLocatedNode(sgNode);
        if(sgLocNode)
        {
            Sg_File_Info* fi = sgLocNode->get_file_info();

            emit clicked(QString(fi->get_filenameString().c_str()),
                         fi->get_line(),fi->get_col());
        }
    }

    // start drag&drop on right mouse button
    if (ev->button() == Qt::RightButton)
        dragStartPosition = ev->pos();


    QGraphicsView::mousePressEvent(ev);
}


// ------------------------- Drag and Drop Functions --------------------------


void RoseGraphicsView::mouseMoveEvent(QMouseEvent *ev)
{
    if(!dragEnabled)
        return  QGraphicsView::mouseMoveEvent(ev);

    if (!(ev->buttons() & Qt::RightButton))
        return QGraphicsView::mouseMoveEvent(ev);;
    if ((ev->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return QGraphicsView::mouseMoveEvent(ev);;


    // get the sgnode
    DisplayNode * dispNode = dynamic_cast<DisplayNode*>(itemAt(dragStartPosition));
    if(!dispNode)
        return;

    SgNode * sgNode = dispNode->getSgNode();
    if(!sgNode)
        return;

    // create drag object, set pixmap and data
    QDrag *drag = new QDrag(viewport());

    drag->setMimeData( createSageMimeData( sgNode ) );
    drag->exec();

    QGraphicsView::mouseMoveEvent(ev);
}

void RoseGraphicsView::dragEnterEvent(QDragEnterEvent * ev)
{
    if( acceptMimeData( ev->mimeData() ) && this != ev->source() )
        ev->accept();
    else
        ev->ignore();

    QGraphicsView::dragEnterEvent(ev);
}

void RoseGraphicsView::dragMoveEvent(QDragMoveEvent * ev)
{
    QWidget::dragMoveEvent(ev);
}


void RoseGraphicsView::dropEvent(QDropEvent *ev)
{
    if(ev->source()==this ||  ev->source()== viewport() ||  !dropEnabled )
        return;

    handleMimeData( ev->mimeData(), ev->pos() );
    ev->acceptProposedAction();
}

bool RoseGraphicsView::acceptMimeData( const QMimeData *mimeData )
{
    return mimeData->hasFormat(SG_NODE_MIMETYPE) && dropEnabled ;
}

void RoseGraphicsView::handleMimeData( const QMimeData *mimeData, const QPoint& pos )
{
    SgNode *node( getGeneralNode( mimeData ) );
    emit nodeDropped( node, pos );
}




void RoseGraphicsView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void RoseGraphicsView::wheelEvent(QWheelEvent *ev)
{
    scaleView(  std::pow((double)2, ev->delta() / 240.0));
}

