#include "customtextedit.h"
#include <QDebug>
#include <QPainter>

#include <QTextCursor>
#include <QPaintEvent>
#include <cmath>

CustomTextEdit::CustomTextEdit(QWidget * p)
        : QTextEdit(p)
{

    connect(this,SIGNAL(textChanged()),viewport(),SLOT(update()));
	cursor1=QTextCursor(this->document());
	cursor1.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
	qDebug() << cursor1.position();

	cursor2=QTextCursor(this->document());
	cursor2.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
	qDebug() << cursor2.position();

	viewport()->update();
}

void CustomTextEdit::paintEvent(QPaintEvent * pEv)
{
    QPainter painter(viewport());
    painter.setClipping(false);
    QPen pen(Qt::red);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::red);

    painter.setRenderHint(QPainter::Antialiasing,true);

    QPoint p1=cursorRect(cursor1).bottomRight();
    QPoint p2=cursorRect(cursor2).topLeft();

    //painter.drawEllipse(cursorRect().center(),10,10);
    drawArrowBezier(p1,p2,&painter,curv);

	//Paint original window
	QPaintEvent newPe(QRect(0,0,1000,1000));
	QTextEdit::paintEvent(&newPe);
}



void CustomTextEdit::normalizeVec(QPointF & vec) const
{
    float xVal=vec.x();
    float yVal=vec.y();
    float norm = std::sqrt(xVal*xVal + yVal*yVal);
    vec /= norm;
}


QPointF CustomTextEdit::getOrtho(const QPointF & i, bool direction) const
{
    QPointF ret(i.y(), -i.x());
    return direction ? ret : -ret;
}


void CustomTextEdit::drawArrowBezier(const QPoint & p1, const QPoint & p2, QPainter * painter, float curvature)
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

/*
bool CustomTextEdit::viewportEvent(QEvent * ev)
{
	QPaintEvent * pe = dynamic_cast<QPaintEvent*>(ev);
	if(pe==NULL)
		return QTextEdit(ev);
}*/



