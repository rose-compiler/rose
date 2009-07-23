#ifndef MEMORYGRAPHICSVIEW_H
#define MEMORYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QColor>

#include "Util.h"

class MemoryType;
class RsType;




class MemoryGraphicsView : public QGraphicsView
{
    public:
        MemoryGraphicsView( QWidget * parent);

        void setMemoryType(MemoryType * mt);

    protected:
        QGraphicsScene * scene;
        MemoryType * mt;

        QVector<QGraphicsRectItem*> byteBoxes;

        /// Paints the rectangle and text for type t at given byte-offset
        /// at given y-layer
        void paintTypeInfo(RsType * t, addr_type offset, int layer);


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
