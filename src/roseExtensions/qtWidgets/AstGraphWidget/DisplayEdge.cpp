#include "DisplayEdge.h"
#include "DisplayNode.h"

#include <QPainter>
#include <QDebug>
#include <cmath>

#include <QFontMetrics>

using namespace std;

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;


DisplayEdge::DisplayEdge(DisplayNode * from, DisplayNode * to)
        : source(from),
          dest(to),
          paintMode(BEZIER_CUBIC),
          arrowSize(10),
          penWidth(2),
          color(Qt::black)
{
    setAcceptedMouseButtons(0);
    adjust();
}


void DisplayEdge::adjust()
{
    if (!source || !dest)
        return;


    prepareGeometryChange();

 //   sourcePoint = mapFromItem(source,QPointF(0, source->boundingRect().height()/2));
//    destPoint   = mapFromItem(dest,QPointF(0, -dest->boundingRect().height()/2));


    QPolygonF srcBoundRect  = mapFromItem(source,source->boundingRect());
    QPolygonF destBoundRect = mapFromItem(dest,dest->boundingRect());


    QLineF centerLine( mapFromItem(source,source->boundingRect().center()),
                       mapFromItem(dest,dest->boundingRect().center()));

    bool res1,res2;
    res1 = intersectPolyWithLine(sourcePoint,srcBoundRect,centerLine);
    if(!res1)
        sourcePoint = centerLine.p1();

    res2 = intersectPolyWithLine(destPoint,destBoundRect,centerLine);\
    if(!res2)
        destPoint = centerLine.p2();

}


QRectF DisplayEdge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    double extra = (penWidth + arrowSize) / 2.0 + 3;

    QFont font;
    QFontMetrics fi(font);

    float width =  fi.width(edgeLabel);
    float height = fi.height();

    QRectF textRect(-width/2,-height/2,width,height);


    QRectF lineRect= QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
                    .normalized()
                    .adjusted(-extra, -extra, extra, extra);

    return textRect.united(lineRect);
}

void DisplayEdge::setEdgeLabel(const QString & label)
{
    prepareGeometryChange();
    edgeLabel=label;
}


void DisplayEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

    painter->setPen(QPen(color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if(!edgeLabel.isEmpty())
        painter->drawText((destPoint+sourcePoint)/2,edgeLabel);

    QPointF controlPoint(destPoint.x(),sourcePoint.y());

        QPainterPath path(sourcePoint);
    switch (paintMode)
    {
                case BEZIER_CUBIC:
                path.cubicTo(controlPoint,controlPoint,destPoint);
                painter->strokePath(path,painter->pen());
                break;
                case BEZIER_QUAD:
                path.quadTo(controlPoint,destPoint);
                painter->strokePath(path,painter->pen());
                break;
                case RECTANGULAR:
                        {
                                double halfY=sourcePoint.y() + (destPoint.y()-sourcePoint.y())/2;
                                path.lineTo(QPointF(sourcePoint.x(),halfY));
                                path.lineTo(QPointF(destPoint.x(),halfY));
                                path.lineTo(destPoint);
                                painter->strokePath(path,painter->pen());
                        }

                        //painter->drawLine(QLineF(sourcePoint,controlPoint));
                        //painter->drawLine(QLineF(controlPoint,destPoint));
                        break;
                case STRAIGHT:
                        controlPoint=sourcePoint;
                        painter->drawLine(QLineF(controlPoint,destPoint));
                        break;
                default:
                        qDebug() << "Unknown Edge Painting Mode";
    }

    QLineF arrowTangent(controlPoint,destPoint);

    // Draw the arrows if there's enough room
    double angle = acos(arrowTangent.dx() / arrowTangent.length());
    if (arrowTangent.dy() >= 0)
        angle = TwoPi - angle;

    QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
                                              cos(angle - Pi / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                              cos(angle - Pi + Pi / 3) * arrowSize);

    painter->setBrush(color);
    painter->drawPolygon(QPolygonF() << arrowTangent.p2() << destArrowP1 << destArrowP2);
}


bool DisplayEdge::intersectPolyWithLine(QPointF & res, const QPolygonF & poly, const QLineF & l)
{
    Q_ASSERT(poly.size()>0);
    QPointF p1 = poly[0];
    QPointF p2;
    QLineF polyLine;
    for (int i = 1; i < poly.size(); i++)
    {
        p2 = poly[i];
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType = polyLine.intersect(l, &res);
        if (intersectType == QLineF::BoundedIntersection)
            return true;

        p1 = p2;
    }
    return false;
}
