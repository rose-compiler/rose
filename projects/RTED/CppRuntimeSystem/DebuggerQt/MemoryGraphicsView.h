#ifndef MEMORYGRAPHICSVIEW_H
#define MEMORYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QColor>

#include "Util.h"
#include "rted_typedefs.h"

class MemoryType;
class RsType;



#include <QGraphicsRectItem>

class RsTypeGraphicsRect : public QGraphicsRectItem
{
    public:
        RsTypeGraphicsRect(const RsType* t, Address addr, QGraphicsItem * parent=0);


        virtual void paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget);

        virtual QRectF boundingRect() const;


        static float BOX_WIDTH;
        static float BOX_HEIGHT;

    protected:
        const RsType* t;
        Address address;

        QPoint textOffset; ///< offset in pixel where to draw the typename

        QString text;   ///< typename
        QFont font;     ///< font to draw typename


        void buildInfoBox();

        virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
        virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
        virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event );


        QColor getTypeColor();

        QGraphicsItem * infoBox;
};

class MemoryGraphicsView : public QGraphicsView
{
    public:
        MemoryGraphicsView(QWidget* parent);

        void setMemoryType(const MemoryType& mt);

    protected:
        QGraphicsScene * scene;
        const MemoryType* mt;

        QVector<QGraphicsRectItem*> byteBoxes;

        /// Paints the rectangle and text for type t at given byte-offset
        /// at given y-layer
        void paintTypeInfo(const RsType* t, size_t offset, int layer);


        // Size of a box which represents a byte
        static const float BOX_WIDTH;
        static const float BOX_HEIGHT;

        // Space between the layer with init-information to the typeInfo
        static const int SPACE_BOX_TYPEINFO;
        // Space between the typeinfo layers
        static const int SPACE_TYPEINFO_TYPEINFO;

        // color of initialized byte-boxes
        static const QColor COLOR_INITIALIZED;
        // color of not initialized byte-boxes
        static const QColor COLOR_NOT_INITIALIZED;

};


#endif
