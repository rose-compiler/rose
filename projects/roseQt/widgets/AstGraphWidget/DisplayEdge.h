#ifndef DISPLAYEDGE_H
#define DISPLAYEDGE_H


#include <QGraphicsItem>


class DisplayNode;

class DisplayEdge : public QGraphicsItem
{
	public:
		DisplayEdge(DisplayNode * from, DisplayNode * to);
		virtual ~DisplayEdge() {}

		DisplayNode *sourceNode() const  { return source; }
	    DisplayNode *destNode()   const  { return dest; }

		void setSourceNode(DisplayNode *node) { source=node; adjust(); }
	    void setDestNode(DisplayNode *node)   { dest=node;   adjust(); }


	    /// Call this function after the position of a node changed
	    void adjust();

		/// Each user defined QGraphicsItem needs a unique constant
	    /// UserType+1 is the DisplayNode
		enum { EdgeType = UserType + 2 };
		int type() const { return EdgeType; }


		void setColor(const QColor & c)  { color=c; }
		void setWidth(double width)		 { penWidth=1; }




		enum PaintMode { STRAIGHT, RECTANGULAR, BEZIER_QUAD,BEZIER_CUBIC };

		void setPaintMode(PaintMode m) { paintMode=m; }


		void setEdgeLabel(const QString & label)  { edgeLabel=label; }
		const QString & getEdgeLabel()  const     { return edgeLabel; }

	protected:
	     QRectF boundingRect() const;
	     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


	private:
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
