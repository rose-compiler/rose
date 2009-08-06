#ifndef DISPLAYGRAPHNODE_H
#define DISPLAYGRAPHNODE_H

#include "DisplayNode.h"

/**
 * \brief Display Information for a Graph Node
 *
 *   This class currently is used to display CallGraphs
 *   Every node is responsible for deleting it's outgoing edges, so that a graph can be cleaned up in a clean way without double free's
 */
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

        void deleteAllEdges();

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

        void deleteOutEdge(DisplayEdge * e);
        void removeInEdge(DisplayEdge * e);


        virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

        QList<DisplayEdge *> inEdges;
        QList<DisplayEdge *> outEdges;


        int id;
};



namespace Ui { class LayoutControl; }

#include <QSet>

class QTimer;
class SgIncidenceDirectedGraph;
class SgGraphNode;
class rose_graph_integer_edge_hash_multimap;
class rose_graph_integer_node_hash_map;

/**
 * \brief Manages the display and layouting of a Graph
 *
 *  This class manages a graph consisting of DisplayGraphNode 's
 *  The graph can be displayed and layouted in a QGraphicsScene.
 *  For layouting a spring embedder algorithm is used, additional a control widget can be rendered into the QGraphicsView
 *  which can be used to control parameters of the spring embedder
 */
class DisplayGraph : public QObject
{
    Q_OBJECT
    public:
        DisplayGraph(QGraphicsScene * sc, QObject * par = 0);
        ~DisplayGraph();


        QList<DisplayGraphNode*> & nodes()  { return n; }

        void addNode (DisplayGraphNode* n );
        void addEdge (int nodeId1, int nodeId2, const QString & label="" );
        void addEdge (DisplayGraphNode * n1, DisplayGraphNode * n2, const QString & label="");

        QWidget * controlWidget()  { return uiWidget; }

        /// Adds a node representing a center of gravity
        /// returns the index of the gravity node
        int  addGravityNode();
        void addInvisibleEdge(int i1, int i2);


        /// good starting point for spring embedder
        void circleLayout();


        bool isAdjacentTo(DisplayGraphNode * n1, DisplayGraphNode * n2) const;

        static DisplayGraph * generateTestGraph(QGraphicsScene * sc,
                                                QObject * par=0);

        virtual void deleteNode(DisplayNode * n);


        void setOptimalDistance(qreal dist) { optimalDistance=dist; }
        void setDelta(qreal newDelta)       { curDelta = newDelta;  }

        qreal springBasedLayoutIteration(qreal delta);

    public slots:
        void on_cmdStartTimer_clicked();
        void on_cmdStopTimer_clicked();
        void on_cmdReset_clicked();

        void on_timerEvent();

    protected:

        qreal getCosBetween(QPointF & a, QPointF & b);
        qreal getSinBetween(QPointF & a, QPointF & b);

        QPointF repulsiveForce (const QPointF & n1, const QPointF & n2, qreal optDist);
        QPointF attractiveForce(const QPointF & n1, const QPointF & n2, qreal optDist);

        void updateWidget();

        QGraphicsScene * scene;

        /// Vector of forces, used in function repulsiveForce()
        QVector<QPointF> forcesList;
        QVector<QPointF> oldForcesList;

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
        /// to keep this data structure consistent don't use DisplayGraphNode-pointer
        /// outside of this class
        QMultiMap<DisplayGraphNode*, DisplayGraphNode *> edgeInfo;

};


/**
 * \brief Display information for a callgraph
 *
 *  Extends the DisplayGraph class to build display a CallGraph
 *  The class is written in a way that partial callgraphs can be displayed
 *  i.e. beginning from a node, all nodes are added with have a distance (measured in edges) nearer than a threshold
 *
 */
class DisplayCallGraph : public DisplayGraph
{
    public:
        DisplayCallGraph(QGraphicsScene * sc, QObject * par=0);

        /// Adds an edge, uses callGraphNodeId's instead of displayGraphNodeIds
        void addCgEdge(int from, int to);

        void addCgNode(int callGraphNodeId, SgGraphNode * n=NULL);


        /// Adds a function-node from the call graph to the display graph and
        /// all connected nodes with distance <= depth
        /// if node is not a function declaration or definition the function does nothing
        void addFunction(SgNode * node, int depth);

        /// set the call graph pointer which is used when addFunction() is called
        void setCg(SgIncidenceDirectedGraph * cg_);

        void clear();

        virtual void deleteNode(DisplayNode * n);

    protected:

        void addElements(rose_graph_integer_node_hash_map & nodes,
                         rose_graph_integer_edge_hash_multimap & edges);


        static void generateCgSubGraph(  SgIncidenceDirectedGraph * cg,
                                         int nodeId,
                                         rose_graph_integer_node_hash_map & nodeMapOut,
                                         rose_graph_integer_edge_hash_multimap & edgeMapOut,
                                         QSet<int> & visitedNodes,
                                         int curDepth);

        QColor colorNode;
        QColor colorBorderNode;

        QMap<int,DisplayGraphNode*> callToDisplayNodeMap;

        SgIncidenceDirectedGraph * cg;

};


#endif
