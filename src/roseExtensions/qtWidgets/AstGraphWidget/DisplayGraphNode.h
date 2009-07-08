#ifndef DISPLAYGRAPHNODE_H
#define DISPLAYGRAPHNODE_H

#include "DisplayNode.h"

class DisplayGraphNode : public DisplayNode
{
    public:
        DisplayGraphNode(QGraphicsScene * sc = NULL);
        DisplayGraphNode(const QString & caption,QGraphicsScene * scene = NULL);

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

        /// True if there is a incoming or outgoing edge to/from the other node
        /// has to search in two list -> rather slow
        bool isAdjacentTo(DisplayGraphNode * otherNode) const;

        int getId() const     { return id;}
        void setId(int newId) { id=newId; }
    protected:
        virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

        QList<DisplayEdge *> inEdges;
        QList<DisplayEdge *> outEdges;


        int id;
};



namespace Ui { class LayoutControl; }

class QTimer;
class SgIncidenceDirectedGraph;
class rose_graph_integer_edge_hash_multimap;
class rose_graph_integer_node_hash_map;

class DisplayGraph : public QObject
{
    Q_OBJECT
    public:
        DisplayGraph(QGraphicsScene * sc, QObject * par = 0);
        ~DisplayGraph();


        QList<DisplayGraphNode*> & nodes()  { return n; }

        int addNode (DisplayGraphNode* n );
        void addEdge (int nodeId1, int nodeId2, const QString & label="" );
        void addEdge (DisplayGraphNode * n1, DisplayGraphNode * n2, const QString & label="");

        QWidget * controlWidget()  { return uiWidget; }

        /// Adds a node representing a center of gravity
        /// returns the index of the gravity node
        int  addGravityNode();
        void addInvisibleEdge(int i1, int i2);


        /// good starting point for spring embedder
        void circleLayout();


        bool isAdjacentTo(int node1, int node2) const { return edgeInfo.contains(node1,node2); }

        static DisplayGraph * generateTestGraph(QGraphicsScene * sc,
                                                QObject * par=0);

        static DisplayGraph * generateLargeTestGraph(QGraphicsScene * sc,
                                                QObject * par=0);

        static DisplayGraph * generateCallGraph(QGraphicsScene * sc,
                                                SgIncidenceDirectedGraph * cg,
                                                rose_graph_integer_node_hash_map & nodeMap,
                                                rose_graph_integer_edge_hash_multimap & edgeMap,
                                                QObject * par=0);


        static DisplayGraph * buildCallGraphForFunction(SgIncidenceDirectedGraph * cg,
                                                        SgNode * funcNode,
                                                        int depth,
                                                        QGraphicsScene * sc,
                                                        QObject * par);


        static void generateCgSubGraph(  SgIncidenceDirectedGraph * cg,
                                         int nodeId,
                                         rose_graph_integer_node_hash_map & nodeMapOut,
                                         rose_graph_integer_edge_hash_multimap & edgeMapOut,
                                         int curDepth);



        void setOptimalDistance(qreal dist) { optimalDistance=dist; }
        void setDelta(qreal newDelta)       { curDelta = newDelta; }

        qreal springBasedLayoutIteration(qreal delta);

    protected slots:
        void on_cmdStartTimer_clicked();
        void on_cmdReset_clicked();

        void on_timerEvent();

    protected:

        static void buildCgVisit(SgIncidenceDirectedGraph * cg,
                                 DisplayGraph *g,
                                 int curNodeIndex,
                                 int lastNodeIndex,
                                 DisplayGraphNode * lastDisplayNode,
                                 const QString & edgeCaption,
                                 QMap<int,DisplayGraphNode*> & addedNodes,
                                 int curDepth);

        static QString getNodeLabelFromId(SgIncidenceDirectedGraph * cg, int nodeId);
        static QString getEdgeLabelFromId(SgIncidenceDirectedGraph * cg, int fromNode, int toNode);




        QPointF repulsiveForce (const QPointF & n1, const QPointF & n2, qreal optDist);
        QPointF attractiveForce(const QPointF & n1, const QPointF & n2, qreal optDist);

        void updateWidget();

        QGraphicsScene * scene;

        /// Vector of forces, used in function repulsiveForce()
        QVector<QPointF> forces;

        /// The bigger the delta the more a force changes the position
        /// called curDelta, because it may be decremented each iteration
        qreal curDelta;

        /// Iteration counter
        int curIteration;

        /// Parameter for force calculation,
        /// optimal distance between two adjacent nodes
        qreal optimalDistance;

        /// Members for the Control Widget
        QWidget * uiWidget;
        Ui::LayoutControl * ui;
        QTimer * timer;


        /// List of all nodes
        QList<DisplayGraphNode * >  n;

        /// Multimap of (undirected) edges, edges are stored in nodes too
        /// but with the multimap the lookup of adjacency info is faster
        /// and invisible edges are supported i.e. edges considered in layouter
        /// but not in display
        /// do keep this datastructure consistent do use node-pointer outside this class!
        QMultiMap<int,int> edgeInfo;
        QMultiMap<DisplayGraphNode*, DisplayGraphNode *> edgeInfoGi;


};


#endif
