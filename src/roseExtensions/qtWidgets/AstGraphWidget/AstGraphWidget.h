
#ifndef AstGRAPHWIDGET_H
#define AstGRAPHWIDGET_H


#include "RoseGraphicsView.h"
#include "DisplayNode.h"


class SgNode;
class AstFilterInterface;
class DisplayGraph;

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
class AstGraphWidget : public RoseGraphicsView
{
	Q_OBJECT

	public:
		AstGraphWidget(QWidget * parent=0);
		virtual ~AstGraphWidget();


	protected:

        virtual void updateGraph(SgNode * node, AstFilterInterface * filter);

		QGraphicsScene * scene;
		DisplayTreeNode * root;
};



#endif
