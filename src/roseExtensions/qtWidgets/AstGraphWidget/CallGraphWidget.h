
#ifndef CALLGRAPHWIDGET_H
#define CALLGRAPHWIDGET_H

#include "RoseGraphicsView.h"

class QMouseEvent;
class DisplayCallGraph;

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
