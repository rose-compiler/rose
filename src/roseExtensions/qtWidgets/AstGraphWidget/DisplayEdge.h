#ifndef DISPLAYEDGE_H
#define DISPLAYEDGE_H


#include <QGraphicsItem>


class DisplayNode;

/**
 * \brief Display information for an edge
 *
 * used to render a edge, each edge has a source and destination node
 * The nodes notifiy the edge when they have changed position
 * Each edge may have a caption which is printed at the "center" of the edge
 *
 */
class DisplayEdge : public QGraphicsItem
{
        public:
                DisplayEdge(DisplayNode * from, DisplayNode * to);
                virtual ~DisplayEdge() {}

                DisplayNode *sourceNode() const  { return source; }
                DisplayNode *destNode()   const  { return dest; }

                /// Call this function after the position of a node changed
                void adjust();

                /// Each user defined QGraphicsItem needs a unique constant
                /// UserType+1 is the DisplayNode
                enum { EdgeType = UserType + 2 };
                int type() const { return EdgeType; }

                /// Changes the edge color
                void setColor(const QColor & c)  { color=c; }
                /// Changes edge width
                void setWidth(double width)              { penWidth=1; }

                enum PaintMode { STRAIGHT,    ///< rendered as straight line between nodes
                                 RECTANGULAR, ///< edge is splitted up into two lines, which are parallel to x/y axis, (half of a rectangle)
                                 BEZIER_QUAD, ///< quadratic bezier curve
                                 BEZIER_CUBIC ///< cubic bezier curve
                };

                ///Changes the painting mode
                void setPaintMode(PaintMode m) { paintMode=m; }


                void setEdgeLabel(const QString & label);
                const QString & getEdgeLabel()  const     { return edgeLabel; }

        protected:
                void setSourceNode(DisplayNode *node) { source=node; adjust(); }
                void setDestNode  (DisplayNode *node) { dest=node;   adjust(); }



                QRectF boundingRect() const;
                void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


        private:
                bool intersectPolyWithLine(QPointF & res, const QPolygonF & poly, const QLineF & l);

                DisplayNode * source;
                DisplayNode * dest;

                QPointF sourcePoint;
                QPointF destPoint;

                PaintMode paintMode;

                QString edgeLabel;

                double arrowSize;

                double penWidth;

                QColor color;
};

#endif
