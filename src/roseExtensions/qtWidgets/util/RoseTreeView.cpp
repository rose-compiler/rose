#include "rose.h"

#include "RoseTreeView.h"
#include "ItemTreeModel.h"
#include "AstFilters.h"
#include "SageMimeData.h"

#include <QApplication>
#include <QDataStream>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPainter>
#include <QPaintEvent>
#include <cmath>

#include <QDebug>


RoseTreeView::RoseTreeView(QWidget * par)
    : QTreeView(par),
      curFilter(NULL),
      curNode(NULL),
      dragEnabled(true),
      dropEnabled(true)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    model = new ItemTreeModel(this);

    updateModel();

    setModel(model);

    /*connect(this, SIGNAL( clicked(const QModelIndex & )),
            this, SLOT(viewClicked(const QModelIndex&)));*/
}


RoseTreeView::~RoseTreeView()
{
}

void RoseTreeView::setNode(SgNode * node)
{
    curNode = node;
    updateModel();
}

void RoseTreeView::setFilter(AstFilterInterface * filter)
{
    if(curFilter)
        delete curFilter;

    curFilter = filter->copy();
    updateModel();
}

bool RoseTreeView::gotoNode( SgNode *node )
{
    QModelIndex pos( findSgNodeInModel( node, *model, model->index(0,0, QModelIndex() ) ) );

    if( !pos.isValid() )
    {
        qDebug() << "invalid position ...";
        return false;
    }

    setCurrentIndex( pos );
    return true;
}

void RoseTreeView::viewClicked(const QModelIndex & ind)
{
    if(! ind.isValid())
        return;

    QVariant nodeAsVariant = model->data(ind,SgNodeRole);
    if(!nodeAsVariant.isValid())
        return;

    SgNode * node= qvariant_cast<SgNode*>( nodeAsVariant);
    if( node == NULL ) return;

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


void RoseTreeView::viewDoubleClicked( const QModelIndex & ind )
{
    if(! ind.isValid() )
        return;


    QVariant nodeAsVariant = model->data(ind,SgNodeRole);
    if(!nodeAsVariant.isValid())
        return;

    SgNode * node= qvariant_cast<SgNode*>( nodeAsVariant);
    Q_ASSERT(node);
    emit doubleClicked(node);
}



// ------------------------- Drag and Drop Functions --------------------------


void RoseTreeView::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
        dragStartPosition = ev->pos();

    QTreeView::mousePressEvent(ev);
}

void RoseTreeView::mouseMoveEvent(QMouseEvent *ev)
{
    if(!dragEnabled)
        return  QTreeView::mouseMoveEvent(ev);

    if (!(ev->buttons() & Qt::LeftButton))
        return QTreeView::mouseMoveEvent(ev);;
    if ((ev->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return QTreeView::mouseMoveEvent(ev);;


    // get the SgNode out of the model
    QVariant nodeAsVariant = model->data(indexAt(dragStartPosition),SgNodeRole);
    if(!nodeAsVariant.isValid())
        return  QTreeView::mouseMoveEvent(ev);;
    SgNode * node = qvariant_cast<SgNode*>(nodeAsVariant);
    if(!node)
        return;

    // create drag object, set pixmap and data
    QDrag *drag = new QDrag(viewport());
    //QMimeData *mimeData = new QMimeData();
    QIcon icon = qvariant_cast<QIcon>( model->data(indexAt(dragStartPosition),Qt::DecorationRole));
    if(!icon.isNull())
        drag->setPixmap(icon.pixmap(16,16));

    //mimeData->setData(SG_NODE_MIMETYPE, d);
    drag->setMimeData( createSageMimeData( node ) );
    drag->exec();



    QTreeView::mouseMoveEvent(ev);
}

void RoseTreeView::dragEnterEvent(QDragEnterEvent * ev)
{
    if( acceptMimeData( ev->mimeData() ) && this != ev->source() )
        ev->accept();
    else
        ev->ignore();
}

void RoseTreeView::dragMoveEvent(QDragMoveEvent * ev)
{
    QWidget::dragMoveEvent(ev);
}


void RoseTreeView::dropEvent(QDropEvent *ev)
{
    if(ev->source()==this ||  ev->source()== viewport() ||  !dropEnabled )
        return;

    handleMimeData( ev->mimeData(), ev->pos() );

    ev->acceptProposedAction();
}

bool RoseTreeView::acceptMimeData( const QMimeData *mimeData )
{
    return mimeData->hasFormat(SG_NODE_MIMETYPE) && dropEnabled ;
}

void RoseTreeView::handleMimeData( const QMimeData *mimeData, const QPoint& pos )
{
    SgNode *node( getGeneralNode( mimeData ) );
    emit nodeDropped( node, pos );
}


void RoseTreeView::addArrow(const QModelIndex & i1, const QModelIndex & i2)
{
    arrows.push_back(qMakePair(i1,i2));
    viewport()->update();
}

void RoseTreeView::removeAllArrows()
{
    arrows.clear();
    viewport()->update();
}


void RoseTreeView::paintEvent(QPaintEvent * pEv)
{
    QPainter painter(viewport());
    painter.setClipRegion(QRegion(QRect(0,0,100000,100000)),Qt::NoClip);
    painter.setClipping(false);
    QPen pen(Qt::red);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::red);

    painter.setRenderHint(QPainter::Antialiasing,true);

    for(int i=0; i<arrows.size(); i++)
    {
        QRect r1 = visualRect(arrows[i].first);
        QRect r2 = visualRect(arrows[i].second);
        QPoint p1=r1.bottomLeft();
        QPoint p2=r2.topLeft();

        drawArrowBezier(p1,p2,&painter,-0.3);
    }

    //Paint original window
    // TODO
    QPaintEvent newPe(QRect(0,0,1000,1000));
    QTreeView::paintEvent(&newPe);

}

void RoseTreeView::normalizeVec(QPointF & vec) const
{
    float xVal=vec.x();
    float yVal=vec.y();
    float norm = std::sqrt(xVal*xVal + yVal*yVal);
    vec /= norm;
}


QPointF RoseTreeView::getOrtho(const QPointF & i, bool direction) const
{
    QPointF ret(i.y(), -i.x());
    return direction ? ret : -ret;
}


void RoseTreeView::drawArrowBezier(const QPoint & p1, const QPoint & p2, QPainter * painter, float curvature)
{
    QPointF midpoint = (p1+p2)/2;
    QPointF p1p2 = p2-p1;

    QPointF ortho = getOrtho(p1p2);
    QPointF controlPoint= midpoint+curvature*ortho;

    //Create Painter path starting from p1
    QPainterPath arrowLine(p1);
    //paint quadratic bezier curve
    arrowLine.quadTo(controlPoint,p2);
    painter->strokePath(arrowLine,painter->pen());


    //Optional using cubic Bezier
    //QPointF c1 = p1+0.2*p1p2+ curvature*ortho;
    //QPointF c2 = p1+0.8*p1p2+ curvature*ortho;

    //painter->drawEllipse(c1,4,4);
    //painter->drawEllipse(controlPoint,4,4);
    //path.cubicTo(c1,c2,p2);
    //controlPoint=c2

    // Paint the arrow head
    // rotate the coordinate system such that [controlpoint,p2] is the new y-axis
    QPointF yAxis = p2-controlPoint;

    normalizeVec(yAxis);
    QPointF xAxis = getOrtho(yAxis);

    QMatrix mat(xAxis.x(), -yAxis.x(),
                xAxis.y(), -yAxis.y(),
                p2.x(),p2.y());

    painter->setWorldTransform(QTransform(mat),true);

    static const float ARROW_OPEN_FACTOR   = 0.4; //opening angle of the head: 1 means 90 degrees, 0 means no
    static const float ARROW_LENGTH_FACTOR = 0.1; //the arrow side length is computed as tangentLength*ARROW_LENGTH_FACTOR
    static const float ARROW_LENGTH_MAX    = 16;  //limits for the arrowhead sideLength
    static const float ARROW_LENGTH_MIN    = 10;


    QPointF tangent  = p2-controlPoint;
    float tangLength = std::sqrt(tangent.x()*tangent.x() + tangent.y()*tangent.y());
    float arrLength  = tangLength*ARROW_LENGTH_FACTOR;

    if(arrLength > ARROW_LENGTH_MAX)      arrLength=ARROW_LENGTH_MAX;
    if(arrLength < ARROW_LENGTH_MIN)      arrLength=ARROW_LENGTH_MIN;


    QPainterPath head(QPointF(0,0));

    head.lineTo(QPointF(ARROW_OPEN_FACTOR * arrLength,arrLength));
    head.lineTo(QPointF(-ARROW_OPEN_FACTOR * arrLength,arrLength));
    head.lineTo(QPointF(0,0));
    painter->drawPath(head);
}















