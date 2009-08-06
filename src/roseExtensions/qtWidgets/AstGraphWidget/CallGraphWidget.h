
#ifndef CALLGRAPHWIDGET_H
#define CALLGRAPHWIDGET_H

#include "RoseGraphicsView.h"

class QMouseEvent;
class DisplayCallGraph;

/**
 * \brief Displays a call-graph
 *
 * \image html CallGraphWidget.jpg
 *
 *  Displays a callgraph associated with a project, and layouts it with a spring embedder algorithm
 *  uses the ProjectManager to build up a callgraph
 */
class CallGraphWidget : public RoseGraphicsView
{
        Q_OBJECT

        public:
            CallGraphWidget(QWidget * parent=0);
            virtual ~CallGraphWidget();

        protected:
            virtual void mouseReleaseEvent(QMouseEvent * ev);

            virtual void updateGraph(SgNode * node, AstFilterInterface * filter);

            QGraphicsScene   * scene;
            DisplayCallGraph * callGraph;
};


#endif
