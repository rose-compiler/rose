
#ifndef AstGRAPHWIDGET_H
#define AstGRAPHWIDGET_H


#include <QGraphicsView>
#include "DisplayNode.h"


class SgNode;
class AstFilterInterface;


class QDragEnterEvent;
class QDropEvent;
/**
 *  Layout of an ast in a graphicsview (similar to zgrViewer )
 *
 *  <img src="../AstGraphView.jpg"  alt="Screenshot">
 *
 *  This widget shows the AST layouted as a tree. For actual layouting
 *  the class TreeLayoutGenerator is used, but this can be replaced easily
 *  (for example to use different algorithms, or layout a general graph instead of a tree)
 *
 *  For displaying Qt's QGraphicsView is used. Therefore two custom QGraphicsItem's are introduced:
 *  DisplayEdge to  render the edges, and DisplayNode for node-rendering. The nodes are responsible for
 *  updating the adjacent edges.
 *
*/
class AstGraphWidget : public QGraphicsView
{
	Q_OBJECT

	public:
		AstGraphWidget(QWidget * parent=0);
		virtual ~AstGraphWidget();


	public slots:
		void setNode(SgNode * sgNode);
		void setFilter(AstFilterInterface * filter);
		void setFileFilter(int id);
	signals:
		void clicked(SgNode * node);

		void clicked(const QString & filename, int line, int column);


	protected:

		QGraphicsScene * scene;
		DisplayTreeNode * root;

		// Zoom
		virtual void wheelEvent(QWheelEvent *event);
		virtual void scaleView(qreal scaleFactor);
		virtual void mousePressEvent(QMouseEvent *event);

		// Drop
		virtual void dragEnterEvent(QDragEnterEvent * ev);
		virtual void dropEvent(QDropEvent *ev);
        virtual void dragMoveEvent( QDragMoveEvent * ev);

		AstFilterInterface * curFilter;
		SgNode * curSgTreeNode;
};



#endif
