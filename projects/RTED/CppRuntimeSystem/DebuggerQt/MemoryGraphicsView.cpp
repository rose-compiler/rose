#include "MemoryGraphicsView.h"
#include "MemoryManager.h"
#include "RsType.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>


#include <QDebug>

const float MemoryGraphicsView::BOX_WIDTH = 20;
const float MemoryGraphicsView::BOX_HEIGHT = 20;

const int MemoryGraphicsView::SPACE_BOX_TYPEINFO = 5;
const int MemoryGraphicsView::SPACE_TYPEINFO_TYPEINFO = 0;

const QColor MemoryGraphicsView::COLOR_INITIALIZED(Qt::green);
const QColor MemoryGraphicsView::COLOR_NOT_INITIALIZED(Qt::red);


MemoryGraphicsView::MemoryGraphicsView(QWidget *par)
    : QGraphicsView(par),
      scene( new QGraphicsScene(this)),
      mt(NULL)
{
    setScene(scene);
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
    for(int b = 0 ; b < mt->getSize(); b++)
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
        if( curAddr % 4 ==0 )
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
    qDebug() << "Painting type " << t->getName().c_str() << " Offset" << offset << "layer" << layer;
    qreal x = offset * BOX_WIDTH;
    qreal y = (SPACE_BOX_TYPEINFO + BOX_HEIGHT) + (layer * (BOX_HEIGHT +  SPACE_TYPEINFO_TYPEINFO));
    qreal width = t->getByteSize() * BOX_WIDTH;

    // Paint box
    QRectF rect(0,0,width, BOX_HEIGHT);
    QGraphicsRectItem * b = new QGraphicsRectItem(rect);
    b->moveBy(x,y);
    scene->addItem(b);

    // Paint Text into it
    QGraphicsSimpleTextItem * text = new QGraphicsSimpleTextItem(t->getDisplayName().c_str(),b);
    // Center text in box
    qreal xOffset = ( b->boundingRect().width() - text->boundingRect().width() ) /2;
    if(xOffset < 0)  xOffset=0;
    qreal yOffset = (b->boundingRect().height() - text->boundingRect().height() ) /2;
    if(yOffset < 0)   yOffset=0;

    text->moveBy(xOffset,yOffset);

    // Paint all subtypes
    for(int i=0; i< t->getSubtypeCount(); i++)
    {
        qDebug() << "Subtype offset" << i << t->getSubtypeOffset(i);
        paintTypeInfo(t->getSubtype(i), t->getSubtypeOffset(i)+offset,layer+1);
    }

}






