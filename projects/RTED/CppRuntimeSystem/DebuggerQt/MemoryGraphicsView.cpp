#include "MemoryGraphicsView.h"
#include "MemoryManager.h"
#include "RsType.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>


#include <QDebug>

const float MemoryGraphicsView::BOX_WIDTH = 20;
const float MemoryGraphicsView::BOX_HEIGHT = 25;

const int MemoryGraphicsView::SPACE_BOX_TYPEINFO = 20;
const int MemoryGraphicsView::SPACE_TYPEINFO_TYPEINFO = 0;

const QColor MemoryGraphicsView::COLOR_INITIALIZED(Qt::green);
const QColor MemoryGraphicsView::COLOR_NOT_INITIALIZED(Qt::red);




MemoryGraphicsView::MemoryGraphicsView(QWidget *par)
    : QGraphicsView(par),
      scene( new QGraphicsScene(this)),
      mt(NULL)
{
    setScene(scene);
    RsTypeGraphicsRect::BOX_HEIGHT = BOX_HEIGHT;
    RsTypeGraphicsRect::BOX_WIDTH=   BOX_WIDTH;

}

void MemoryGraphicsView::setMemoryType(MemoryType * _mt)
{
    qDeleteAll(byteBoxes);
    byteBoxes.clear();
    scene->clear();

    mt = _mt;
    if(!mt)
        return;

    byteBoxes.resize(mt->getSize());
    QRectF box(0,0,BOX_WIDTH,BOX_HEIGHT);
    for(unsigned int b = 0 ; b < mt->getSize(); b++)
    {
        byteBoxes[b] = new QGraphicsRectItem(box);
        byteBoxes[b]->moveBy(b*BOX_WIDTH,0);
        scene->addItem(byteBoxes[b]);

        QBrush bgBrush ( mt->isInitialized(b,b+1) ? COLOR_INITIALIZED : COLOR_NOT_INITIALIZED );
        QPen p;
        p.setWidth(2);
        byteBoxes[b]->setBrush(bgBrush);
        byteBoxes[b]->setPen(p);

        //Addr label
        addr_type curAddr = mt->getAddress() +b;
        if( curAddr % 8 ==0 )
        {
            QString addrStr = QString("0x%1").arg(curAddr,0,16);
            QGraphicsTextItem * ti = new QGraphicsTextItem(addrStr, byteBoxes[b]);
            ti->moveBy(- ti->boundingRect().width()/2, -BOX_HEIGHT);
        }
    }

    MemoryType::TypeInfoMap::const_iterator it = mt->getTypeInfoMap().begin();
    for(; it != mt->getTypeInfoMap().end();  ++it)
        paintTypeInfo(it->second,it->first,0);

}


void MemoryGraphicsView::paintTypeInfo(RsType * t, addr_type offset, int layer)
{
    RsTypeGraphicsRect * gr = new RsTypeGraphicsRect(t,mt->getAddress()+offset);
    qreal x = offset * BOX_WIDTH;
    qreal y = (SPACE_BOX_TYPEINFO + BOX_HEIGHT) + (layer * (BOX_HEIGHT +  SPACE_TYPEINFO_TYPEINFO));

    gr->moveBy(x,y);
    scene->addItem(gr);

    // Paint all subtypes
    for(int i=0; i< t->getSubtypeCount(); i++)
        paintTypeInfo(t->getSubtype(i), t->getSubtypeOffset(i)+offset,layer+1);

}

// -------------------------------- RsTypeGraphicsRect ---------------------------------

#include <QFontMetrics>
#include <QLabel>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsProxyWidget>

float RsTypeGraphicsRect::BOX_WIDTH = 0;
float RsTypeGraphicsRect::BOX_HEIGHT = 0;


RsTypeGraphicsRect::RsTypeGraphicsRect(RsType * t_,addr_type a,QGraphicsItem * par)
    : QGraphicsRectItem(par),t(t_),address(a),infoBox(NULL)
{
    if(dynamic_cast<RsClassType*>(t))
        font.setBold(true);

    setAcceptHoverEvents(true);

    setBrush(QBrush(getTypeColor()));

    setRect(QRectF(0,0, t->getByteSize() * BOX_WIDTH,BOX_HEIGHT ));

    text = t->getDisplayName().c_str();


    QFontMetrics fm (font);
    QRect textBoundingRect = fm.boundingRect(text);
    int textWidth = textBoundingRect.width();
    int boxWidth = (int)boundingRect().width();

    if(textWidth > boxWidth)
    {
        textOffset.setX(0);
        font.setPixelSize(boxWidth / text.size() ); // make font smaller to fit the box
    }
    else
        textOffset.setX((boxWidth-textWidth)/2);    // center text in box

    // Center in y direction
    int yOffset = (int) (rect().height() - fm.height() ) /2;
    if(yOffset < 0)
        yOffset=0;
    textOffset.setY(yOffset + fm.height()-2);

}

QRectF RsTypeGraphicsRect::boundingRect() const
{
    QRectF r = rect();
    r.setWidth(r.width()+10);
    r.setHeight(r.height()+10);
    return r;
}

void RsTypeGraphicsRect::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    // paint the box
    QGraphicsRectItem::paint(painter,option,widget);

    painter->setFont(font);
    painter->drawText(textOffset,text);
}


void RsTypeGraphicsRect::buildInfoBox()
{
    if(infoBox)
        return;


    QLabel * label = new QLabel();
    label->setFrameStyle(QFrame::StyledPanel |QFrame::Raised );

    QString labelText;
    labelText += "<b>" + text + "</b>";

    RsBasicType * basicType = dynamic_cast<RsBasicType*>(t);
    if(basicType)
    {
        labelText += "<br>Value: ";
        labelText += basicType->readValueAt(address).c_str();
    }
    label->setText(labelText);

    if(!scene())
    {
        qDebug() << "No scene to add infoBox";
        return;
    }

    infoBox = scene()->addWidget(label);
    infoBox->setZValue(2);
}


void RsTypeGraphicsRect::hoverEnterEvent ( QGraphicsSceneHoverEvent * ev )
{
    buildInfoBox();
    Q_ASSERT(infoBox);
    infoBox->setVisible(true);

    font.setItalic(true);

    //prepareGeometryChange();
    QPen p = pen();
    p.setWidth(2);
    setPen(p);
    setZValue(1);


    QGraphicsRectItem::hoverEnterEvent(ev);
}

void RsTypeGraphicsRect::hoverLeaveEvent ( QGraphicsSceneHoverEvent * ev )
{
    Q_ASSERT(infoBox);
    infoBox->setVisible(false);

    font.setItalic(false);

    //prepareGeometryChange();

    QPen p = pen();
    p.setWidth(1);
    setPen(p);
    setZValue(0);

    QGraphicsRectItem::hoverLeaveEvent(ev);
}

void RsTypeGraphicsRect::hoverMoveEvent ( QGraphicsSceneHoverEvent * ev )
{
    infoBox->setPos( mapToScene(ev->pos()+QPoint(10,10)));
    QGraphicsRectItem::hoverMoveEvent(ev);

}

QColor RsTypeGraphicsRect::getTypeColor()
{

    RsBasicType * basicType = dynamic_cast<RsBasicType*>(t);
    RsArrayType * arrType =   dynamic_cast<RsArrayType*>(t);
    RsClassType * classType = dynamic_cast<RsClassType*>(t);

    /*
    int typeId=0;
    if(basicType)
        typeId = basicType->getSgType();

    if(arrType)   typeId = RsBasicType::Unknown+1;
    if(classType) typeId = RsBasicType::Unknown+2;
    int typeMax = RsBasicType::Unknown+2;
    int saturation = 255;
    int brightness = 255;
    int hue        = (float)typeId/(float)typeMax * 255;
       */

    QColor c;

    if(classType)   c = Qt::gray; //c.setRgb(0,207,250,160);  // blue
    if(arrType)    { c = Qt::gray; c= c.lighter(120); } // c.setRgb(30,255,95,160);  // green
    if(basicType)   c.setRgb(0,207,250);  // blue //c.setRgb(255,255,40);     // yellow

    return c;
}

