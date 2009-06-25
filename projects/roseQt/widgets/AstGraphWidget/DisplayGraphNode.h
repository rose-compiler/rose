#ifndef DISPLAYGRAPHNODE_H
#define DISPLAYGRAPHNODE_H

#include "DisplayNode.h"

class DisplayGraphNode : public DisplayNode
{
    public:
        DisplayGraphNode(QGraphicsScene * sc = NULL);
        DisplayGraphNode(const QString & caption, QGraphicsScene * scene = NULL);

        virtual ~DisplayGraphNode();


        const QList<DisplayEdge *> & getInEdges()  const { return inEdges;  }
        const QList<DisplayEdge *> & getOutEdges() const { return outEdges; }

        int outEdgeCount() const { return outEdges.size(); }
        int inEdgeCount()  const { return inEdges.size();  }

        DisplayEdge * getInEdge (int id) { return inEdges[id];  }
        DisplayEdge * getOutEdge(int id) { return outEdges[id]; }

        void addOutEdge(DisplayNode * to );
        void addInEdge (DisplayNode * from);

        /// Node takes ownership of this edge
        static void addEdge(DisplayEdge * edge);

        /// Overwritten to set scene of edges
        virtual void setScene(QGraphicsScene * scene);

        bool isAdjacentTo(DisplayGraphNode * o) const;
    protected:
        virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

        QList<DisplayEdge *> inEdges;
        QList<DisplayEdge *> outEdges;
};



namespace Ui { class LayoutControl; }
class QTimer;

class DisplayGraph : public QObject
{
    Q_OBJECT
    public:
        DisplayGraph(QObject * par = 0);
        ~DisplayGraph();


        QList<DisplayGraphNode*> & nodes()  { return n; }

        void addNode (DisplayGraphNode* n );
        void addEdge (int nodeId1, int nodeId2 );
        void addEdge (DisplayGraphNode * n1, DisplayGraphNode * n2);

        QWidget * controlWidget()  { return uiWidget; }

        static DisplayGraph * generateTestGraph(QGraphicsScene * sc, QObject * par=0);
    protected slots:
        void on_cmdStartTimer_clicked();
        void on_cmdReset_clicked();

        void on_timerEvent();
    protected:

        void springBasedLayoutIteration(qreal delta);
        QPointF repulsiveForce (const QPointF & n1, const QPointF & n2);
        QPointF attractiveForce(const QPointF & n1, const QPointF & n2);


        void updateWidget();

        qreal curDelta;
        int curIteration;

        qreal optimalDistance;

        QWidget * uiWidget;
        Ui::LayoutControl * ui;

        QList<DisplayGraphNode * >  n;

        QTimer * timer;
        //static const qreal OPTIMAL_DISTANCE=20;
};


#endif
