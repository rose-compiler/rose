#include "rose.h"
#include "DisplayGraphNode.h"
#include "DisplayEdge.h"

#include "AstDisplayInfo.h"


#include <QGraphicsScene>

#include <QDebug>


#include <cmath>

using namespace std;

typedef rose_graph_integer_node_hash_map      NodeMap;
typedef rose_graph_integer_edge_hash_multimap EdgeMap;



// ---------------------- DisplayGraphNode -------------------------------

DisplayGraphNode::DisplayGraphNode(QGraphicsScene * sc)
    : DisplayNode(sc),id(-1)
{
}

DisplayGraphNode::DisplayGraphNode(const QString & caption,QGraphicsScene * sc)
    : DisplayNode(caption,sc),id(-1)
{
}



DisplayGraphNode::~DisplayGraphNode()
{
    // every node deletes its outgoing edges
    qDeleteAll(outEdges);
}

void DisplayGraphNode::addOutEdge(DisplayNode * to)
{
    outEdges.push_back(new DisplayEdge(this,to));

    if(scene)
        scene->addItem(outEdges.back());
}

void DisplayGraphNode::addInEdge(DisplayNode * from)
{
    inEdges.push_back(new DisplayEdge(from,this));

    if(scene)
        scene->addItem(outEdges.back());
}

void DisplayGraphNode::addEdge(DisplayEdge * edge)
{
    DisplayGraphNode * from = dynamic_cast<DisplayGraphNode*> (edge->sourceNode());
    DisplayGraphNode * to   = dynamic_cast<DisplayGraphNode*> (edge->destNode());

    Q_ASSERT(from && to); // there should only be GraphNodes in this Graph

    from->outEdges.push_back(edge);
    to  ->inEdges.push_back(edge);

    Q_ASSERT(from->scene == to->scene);

    if(from->scene)
        from->scene->addItem(edge);
}


QVariant DisplayGraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionHasChanged)
    {
        foreach(DisplayEdge * edge, inEdges)
            edge->adjust();

        foreach(DisplayEdge * edge,outEdges)
            edge->adjust();
    }
    return DisplayNode::itemChange(change, value);
}

void DisplayGraphNode::setScene(QGraphicsScene * sc)
{
    if(!sc && scene)
    {
        foreach(QGraphicsItem * i, inEdges)
            scene->removeItem(i);

        foreach(QGraphicsItem * i, outEdges)
            scene->removeItem(i);
    }

    if(sc)
    {
        foreach(QGraphicsItem * i, inEdges)
            sc->addItem(i);

        foreach(QGraphicsItem * i, outEdges)
            sc->addItem(i) ;
    }

    DisplayNode::setScene(sc);
}

bool DisplayGraphNode::isAdjacentTo(DisplayGraphNode * o) const
{
    foreach(DisplayEdge * e, inEdges)
        if(e->sourceNode() == o)
            return true;

    foreach(DisplayEdge * e, outEdges)
        if(e->destNode() == o)
            return true;

    return false;
}




// ---------------------- DisplayGraph -------------------------------

#include "ui_LayoutControl.h"
#include <QTimer>

DisplayGraph::DisplayGraph(QGraphicsScene * sc, QObject * par)
    : QObject(par),
      scene(sc),
      curIteration(0),
      uiWidget(new QWidget()),
      ui(new Ui::LayoutControl()),
      timer(new QTimer(this))
{
    ui->setupUi(uiWidget);

    curDelta = ui->spnDelta->value() * 10e-4;
    optimalDistance = ui->spnOptimalDistance->value();
    updateWidget();

    connect(ui->cmdStartTimer,SIGNAL(clicked()), SLOT(on_cmdStartTimer_clicked()));
    connect(ui->cmdStopTimer,SIGNAL(clicked()),timer, SLOT(stop()));
    connect(ui->cmdSingleStep,SIGNAL(clicked()), SLOT(on_timerEvent()));
    connect(ui->cmdReset,SIGNAL(clicked()), SLOT(on_cmdReset_clicked()));

    connect(timer,SIGNAL(timeout()),SLOT(on_timerEvent()));
}


DisplayGraph::~DisplayGraph()
{
    delete uiWidget;
    delete ui;

    qDeleteAll(n);
}

void DisplayGraph::updateWidget()
{
    ui->lblCurIteration->setText(QString("%1").arg(curIteration));
    ui->lblCurrentDelta->setText(QString("%1").arg(curDelta));
}


void DisplayGraph::on_cmdStartTimer_clicked()
{
    on_cmdReset_clicked();
    timer->start(ui->spnTimerInterval->value());
}



void DisplayGraph::on_cmdReset_clicked()
{
    curIteration=0;
    curDelta = ui->spnDelta->value() *1e-4;
    updateWidget();
}



void DisplayGraph::addEdge(DisplayGraphNode * n1, DisplayGraphNode * n2, const QString & label)
{
    int i1 = n.indexOf(n1);
    int i2 = n.indexOf(n2);
    Q_ASSERT(i1 >=0 && i2 >=0); // nodes have to be added before edges are added
    addEdge(i1,i2,label);

}

void DisplayGraph::addEdge ( int i1, int i2, const QString & label)
{
    // create visible part
    DisplayEdge * e = new DisplayEdge(n[i1],n[i2]);
    e->setEdgeLabel(label);
    e->setPaintMode(DisplayEdge::STRAIGHT);
    DisplayGraphNode::addEdge(e);
    // ..and invisible part
    addInvisibleEdge(i1,i2);
}

void DisplayGraph::addInvisibleEdge(int i1, int i2)
{
    edgeInfo.insert(i1,i2);
    edgeInfo.insert(i2,i1);

    edgeInfoGi.insert(n[i1],n[i2]);
    edgeInfoGi.insert(n[i2],n[i1]);
}

int DisplayGraph::addNode(DisplayGraphNode * node)
{
    node->setScene(scene);
    n.push_back(node);
    node->setId(n.size()-1);
    return n.size()-1;
}

int DisplayGraph::addGravityNode()
{
    DisplayGraphNode * node = new DisplayGraphNode("x",scene);
    node->setBgColor(QColor(Qt::red).lighter(150));
    n.push_back(node);
    node->setId(n.size()-1);
    return n.size()-1;
}

void DisplayGraph::on_timerEvent()
{
    if(curIteration >= ui->spnMaxIterations->value() ||
         curDelta <= 0)
    {
        timer->stop();
        return;
    }

    optimalDistance= ui->spnOptimalDistance->value();

    springBasedLayoutIteration(curDelta);

    curDelta -= ui->spnDeltaDecrement->value() * 1e-6;
    curIteration++;
    updateWidget();
}


qreal DisplayGraph::springBasedLayoutIteration(qreal delta)
{
    forces.fill(QPointF(0,0),n.size());

    /*
    qDebug() << "Edge Info:";
    QMultiMap<int, int>::iterator i = edgeInfo.begin();
    while (i != edgeInfo.end() ) {
        qDebug() << i.value() ;
        ++i;
    }*/

    // Calculate attractive forces between adjacent nodes
    foreach(DisplayGraphNode * n1, n)
    {
        QMultiMap<DisplayGraphNode*, DisplayGraphNode*>::iterator i = edgeInfoGi.find(n1);
        while (i != edgeInfoGi.end() && i.key() == n1)
        {
            DisplayGraphNode * n2 = *i;

            qreal dist= optimalDistance + n1->boundingRect().width()/2 +
                                          n2->boundingRect().width()/2;

            QPointF force (attractiveForce(n1->pos(),n2->pos(),dist) );
            forces[n1->getId()] += force;
            forces[n2->getId()] -= force;

            ++i;
        }
    }


    // Calculate repulsive forces for near nodes
    foreach(DisplayGraphNode * n1, n)
    {
        QRectF rect(0,0,10*optimalDistance,10*optimalDistance);
        rect.moveCenter(n1->pos());
        QList<QGraphicsItem*> nearItems = scene->items(rect,Qt::IntersectsItemBoundingRect);
        qDebug() << "Inner Loop: " << nearItems.size();

        foreach(QGraphicsItem * n2,nearItems)
        {
            DisplayGraphNode * dispN1 = n1;
            DisplayGraphNode * dispN2 = dynamic_cast<DisplayGraphNode*>(n2);

            // edges are also found by scene->item()
            if( !dispN2)
                continue;

            qreal dist= optimalDistance + n1->boundingRect().width()/2 +
                                          n2->boundingRect().width()/2;

            QPointF repForce(repulsiveForce(n1->pos(),n2->pos(),dist));

            forces[dispN1->getId()] += repForce;
            forces[dispN2->getId()] -= repForce;
        }
    }

    qreal max=0;
    for(int i=0; i<n.size(); i++)
    {
        if(n[i]->isMouseHold())
            continue;

        qreal dx = forces[i].x() * delta;
        qreal dy = forces[i].y() * delta;

        max=qMax(max,dx);
        max=qMax(max,dy);

        dx = qBound(-optimalDistance,dx,optimalDistance);
        dy = qBound(-optimalDistance,dy,optimalDistance);

        n[i]->moveBy(dx,dy);
    }

    return max;
}



QPointF DisplayGraph::repulsiveForce (const QPointF & n1, const QPointF & n2, qreal optDist)
{
    if(n1 == n2)
    {
        qDebug() << "GraphLayout RepForce Warning";
        return QPointF(0,0);
    }


    QPointF v (n1-n2);
    QLineF l (n1,n2);

    QPointF res =( optDist *optDist / l.length() ) * v;
    return  res;
}

QPointF DisplayGraph::attractiveForce(const QPointF & n1, const QPointF & n2, qreal optDist)
{
    if(n1 == n2)
    {
        qDebug() << "GraphLayout AttrForce Warning";
        return QPointF(0,0);
    }

    QPointF v( n2-n1);
    qreal normSq = v.x()*v.x() + v.y() * v.y() ;
    QPointF res =( normSq  / optDist) * v;
    return res;
}


void DisplayGraph::circleLayout()
{
    int nodeCount = n.size();
    qreal radius = optimalDistance/10 * nodeCount /2;

    for(int i=0; i<nodeCount ; i++)
    {
        qreal angle = ( static_cast<qreal>(i)/nodeCount ) * (2* M_PI);
        QPointF newPos ( cos(angle),sin(angle) );
        newPos *= radius;
        n[i]->setPos(newPos);
    }
}





DisplayGraph * DisplayGraph::buildCallGraphForFunction(SgIncidenceDirectedGraph * cg,
                                                       SgNode * funcNode,
                                                       int depth,
                                                       QGraphicsScene * sc,
                                                       QObject * par)
{

    if(AstDisplayInfo::getType(funcNode) != AstDisplayInfo::FUNCTION)
        return NULL;


    DisplayGraph * g = new DisplayGraph(sc,par);

    QString funcName = AstDisplayInfo::getShortNodeNameDesc(funcNode);

    typedef rose_graph_string_integer_hash_multimap StringNodeMap;
    StringNodeMap & stringNodeMap = cg->get_string_to_node_index_multimap();
    StringNodeMap::iterator it = stringNodeMap.find(funcName.toStdString());

    if(it == stringNodeMap.end())
        return NULL;


    int nodeIndex= it->second;

    qDebug() << getNodeLabelFromId(cg,nodeIndex);

    DisplayGraphNode * curDisplayNode = new DisplayGraphNode(getNodeLabelFromId(cg,nodeIndex));
    curDisplayNode->setBgColor(QColor(Qt::red).lighter(130));
    g->addNode(curDisplayNode);


    QMap<int,DisplayGraphNode*>  addedNodes;
    addedNodes.insert(nodeIndex,curDisplayNode);

    // call visit for all out-nodes
    EdgeMap & edges =  cg->get_node_index_to_edge_multimap_edgesOut ();
    EdgeMap::iterator edgeIt = edges.find(nodeIndex);
    for(; edgeIt != edges.end() && edgeIt->first == nodeIndex; ++edgeIt )
    {
        SgDirectedGraphEdge * edge = isSgDirectedGraphEdge(edgeIt->second);
        QString edgeLabel( edge->get_name().c_str() );
        int newNodeIndex = edge->get_to()->get_index();

        if(addedNodes.contains(newNodeIndex) )
            g->addEdge(curDisplayNode, addedNodes[newNodeIndex]);
        else
            buildCgVisit(cg,g,newNodeIndex,nodeIndex,curDisplayNode,edgeLabel,addedNodes,depth);
    }



    g->circleLayout();
    return g;

}


void DisplayGraph::buildCgVisit(SgIncidenceDirectedGraph * cg,
                                DisplayGraph *g,
                                int curNodeIndex,
                                int lastNodeIndex,
                                DisplayGraphNode * lastDisplayNode,
                                const QString & edgeCaption,
                                QMap<int,DisplayGraphNode*> & addedNodes,
                                int curDepth)
{
    if(curDepth==0)
        return;

    if(addedNodes.contains(curNodeIndex))
        return;

    // Add node to display graph
    DisplayGraphNode * curDisplayNode = new DisplayGraphNode(getNodeLabelFromId(cg,curNodeIndex));
    g->addNode(curDisplayNode);



    addedNodes.insert(curNodeIndex,curDisplayNode);
    qDebug() << "Added" << curNodeIndex << getNodeLabelFromId(cg,curNodeIndex);

    // add edge from last to this node
    g->addEdge(lastDisplayNode,curDisplayNode);

    // call recursively for all adjacent nodes
    EdgeMap & edges =  cg->get_node_index_to_edge_multimap_edgesOut ();
    EdgeMap::iterator edgeIt = edges.find(curNodeIndex);
    for(; edgeIt != edges.end() && edgeIt->first == curNodeIndex; ++edgeIt )
    {
        SgDirectedGraphEdge * edge = isSgDirectedGraphEdge(edgeIt->second);
        QString newEdgeCaption( edge->get_name().c_str() );
        int newNodeIndex = edge->get_to()->get_index();


        QMap<int,DisplayGraphNode*>::const_iterator i = addedNodes.constBegin();
        while (i != addedNodes.constEnd()) {
            qDebug() << i.key() << ": " << i.value()->getDisplayName() ;
            ++i;
        }

        if(addedNodes.contains(newNodeIndex) )
            g->addEdge(curDisplayNode, addedNodes[newNodeIndex]);
        else
            buildCgVisit(cg,g,newNodeIndex,curNodeIndex,curDisplayNode,newEdgeCaption,addedNodes,curDepth-1);
    }

}

QString DisplayGraph::getNodeLabelFromId(SgIncidenceDirectedGraph * cg, int nodeId)
{
    NodeMap & nodes = cg->get_node_index_to_node_map ();
    NodeMap::iterator it = nodes.find(nodeId);
    Q_ASSERT(it != nodes.end());

    return it->second->get_name().c_str();
}

QString DisplayGraph::getEdgeLabelFromId(SgIncidenceDirectedGraph * cg, int fromNode, int toNode)
{
    EdgeMap & edges =  cg->get_node_index_to_edge_multimap_edgesOut ();
    EdgeMap::iterator edgeIt = edges.find(fromNode);
    for(; edgeIt != edges.end() && edgeIt->first==fromNode; ++edgeIt )
    {
        SgDirectedGraphEdge * edge = isSgDirectedGraphEdge(edgeIt->second);
        Q_ASSERT(edge);

        if(edge->get_to()->get_index() == toNode)
            return  edge->get_name().c_str();
    }
    Q_ASSERT(false);
    return "";
}




DisplayGraph * DisplayGraph::generateCallGraph(QGraphicsScene * sc,
                                               SgIncidenceDirectedGraph * cg,
                                               rose_graph_integer_node_hash_map & nodeMap,
                                               rose_graph_integer_edge_hash_multimap & edgeMap,
                                               QObject * par)
{
    DisplayGraph * g = new DisplayGraph(sc,par);

    NodeMap & nodes = cg->get_node_index_to_node_map ();


    for( NodeMap::iterator it = nodes.begin();  it != nodes.end(); ++it )
    {
        QString name  ( it->second->get_name ().c_str() );
        DisplayGraphNode * n = new DisplayGraphNode(name, sc);

        g->addNode(n);
    }

    EdgeMap & edges =  cg->get_node_index_to_edge_multimap_edgesOut ();
    for( EdgeMap::iterator it= edges.begin(); it != edges.end(); ++it )
    {
        SgDirectedGraphEdge * edge = isSgDirectedGraphEdge(it->second);
        Q_ASSERT(edge);
        QString edgeLabel( edge->get_name().c_str());
        g->addEdge(edge->get_from()->get_index(),edge->get_to()->get_index(),edgeLabel);
    }

    qDebug() << "BuiltCallGraph with " << nodes.size() << "nodes and " << edges.size() << "edges";
    g->circleLayout();

    return g;
}


DisplayGraph * DisplayGraph::generateTestGraph(QGraphicsScene * sc,QObject * par)
{
    DisplayGraph * g = new DisplayGraph(sc,par);

    // add some nodes
    for(int i=0; i < 6; i++)
    {
        DisplayGraphNode * n = new DisplayGraphNode( QString("%1").arg(i),sc );
        //n->setPos(i,0);
        g->addNode(n);
    }


    // ..and edges
    g->addEdge(0,1);
    g->addEdge(1,0);
    g->addEdge(0,2);

    g->addEdge(1,3);

    g->addEdge(3,4);
    g->addEdge(4,5);
    g->addEdge(3,5);
    g->addEdge(2,4);


    int n1=g->addNode( new DisplayGraphNode("Node1",sc) ) ;
    int n2=g->addNode( new DisplayGraphNode("LongNode2",sc));
    int n3=g->addNode( new DisplayGraphNode("LongLongNode3",sc));

    int gn=g->addGravityNode();

    g->addEdge(n1,n2);
    g->addEdge(0,n1);
    g->addEdge(2,n2);
    g->addInvisibleEdge(gn,n1);
    g->addInvisibleEdge(gn,n2);
    g->addInvisibleEdge(gn,n3);


    g->circleLayout();
    return g;
}


DisplayGraph * DisplayGraph::generateLargeTestGraph(QGraphicsScene * sc,QObject * par)
{
    DisplayGraph * g = new DisplayGraph(sc,par);

    qDebug() << "Generating Graph";

    const int NODE_COUNT = 200;
    const int EDGE_COUNT = 50;


    // add some nodes
    for(int i=0; i < NODE_COUNT; i++)
    {
        DisplayGraphNode * n = new DisplayGraphNode( QString("%1").arg(i),sc );
        n->setPos(i,0);
        g->addNode(n);
        if(i>0)
            g->addEdge(i,i-1);
    }

    /*
    const int CLUSTER_SIZE = 50;
    int lastClusterNode=-1;
    for(int i = 0; i < NODE_COUNT / CLUSTER_SIZE; i++)
    {
        int clusterNode = g->addGravityNode();
        for(int j= i*CLUSTER_SIZE; j < (i+1)*CLUSTER_SIZE ;j++)
            g->addInvisibleEdge(clusterNode,j);


        if(i>0)
            g->addInvisibleEdge(clusterNode,lastClusterNode);

        lastClusterNode=clusterNode;
    }
    */


    int edgeCounter=0;
    while(edgeCounter < EDGE_COUNT)
    {
        int i1 = (double) rand()/RAND_MAX * NODE_COUNT;
        int i2 = (double) rand()/RAND_MAX * NODE_COUNT;

        if(! g->isAdjacentTo(i1,i2))
        {
            g->addEdge(i1,i2);
            edgeCounter++;
        }
    }


    qDebug() << "Done";
    g->circleLayout();

    return g;
}



void DisplayGraph::generateCgSubGraph(SgIncidenceDirectedGraph * cg,
                                      int nodeId,
                                      rose_graph_integer_node_hash_map & nodeMapOut,
                                      rose_graph_integer_edge_hash_multimap & edgeMapOut,
                                      int curDepth)
{

    NodeMap & nodes = cg->get_node_index_to_node_map ();
    Q_ASSERT( nodes.find(nodeId) != nodes.end());

    pair<NodeMap::iterator,bool> res;
    res = nodeMapOut.insert(make_pair<int,SgGraphNode*>(nodeId,nodes[nodeId] ));
    if(!res.second) // node already exists
        return;


    if(curDepth==0)
        return;


    // call recursively for all adjacent nodes
    EdgeMap & edges =  cg->get_node_index_to_edge_multimap_edgesOut ();
    EdgeMap::iterator edgeIt = edges.find(nodeId);
    for(; edgeIt != edges.end() && edgeIt->first == nodeId; ++edgeIt )
    {
        SgDirectedGraphEdge * edge = isSgDirectedGraphEdge(edgeIt->second);
        Q_ASSERT(edge);

        // Add edge
        int newNodeIndex = edge->get_to()->get_index();
        edgeMapOut.insert(*edgeIt);

        generateCgSubGraph(cg,newNodeIndex,nodeMapOut,edgeMapOut,curDepth-1);
    }
}





