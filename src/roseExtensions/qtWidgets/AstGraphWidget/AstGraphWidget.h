
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
 *  \brief Layout of an AST in a graphicsview (similar to zgrViewer )
 *
 *  \image html AstGraphView.jpg
 *
 *  This widget shows the AST layouted as a tree. For actual layouting
 *  the class TreeLayoutGenerator is used, but this can be replaced easily
 *  (for example to use different algorithms, or layout a general graph instead of a tree)
 *
 *  The actual code for displaying the nodes and edges is implemented in DisplayTreeNode and DisplayEdge
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
