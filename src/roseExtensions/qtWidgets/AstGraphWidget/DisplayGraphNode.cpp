#include "rose.h"
#include "DisplayGraphNode.h"
#include "DisplayEdge.h"

#include "AstDisplayInfo.h"


#include <QGraphicsScene>
#include <QSet>

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

void DisplayGraphNode::deleteAllEdges()
{
    foreach(DisplayEdge *e, outEdges)
    {
        DisplayGraphNode * src = dynamic_cast<DisplayGraphNode*>(e->destNode() );
        src->removeInEdge(e);
        delete e;
    }

    foreach(DisplayEdge *e, inEdges)
    {
        DisplayGraphNode * src = dynamic_cast<DisplayGraphNode*>(e->sourceNode());
        src->deleteOutEdge(e);
    }

    outEdges.clear();
    inEdges.clear();
}

void DisplayGraphNode::deleteOutEdge(DisplayEdge * e)
{
    int i = outEdges.indexOf(e);
    if(i>=0)
    {
        delete outEdges[i];
        outEdges.removeAt(i);
    }
}

void DisplayGraphNode::removeInEdge(DisplayEdge * e)
{
    int i = inEdges.indexOf(e);
    if(i>=0)
        inEdges.removeAt(i);
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
    connect(ui->cmdStopTimer,SIGNAL(clicked()), SLOT(on_cmdStopTimer_clicked()));

    connect(ui->cmdReset,SIGNAL(clicked()), SLOT(on_cmdReset_clicked()));

    connect(timer,SIGNAL(timeout()),SLOT(on_timerEvent()));
}


DisplayGraph::~DisplayGraph()
{
    delete ui;

    qDeleteAll(n);
}

void DisplayGraph::updateWidget()
{
}


void DisplayGraph::on_cmdStartTimer_clicked()
{
    curIteration=0;
    curDelta = ui->spnDelta->value() *1e-4;
    ui->cmdStartTimer->setChecked(true);
    ui->cmdStopTimer->setChecked(false);
    timer->start(ui->spnTimerInterval->value());
}

void DisplayGraph::on_cmdStopTimer_clicked()
{
    ui->cmdStartTimer->setChecked(false);
    ui->cmdStopTimer->setChecked(true);
    timer->stop();
}


void DisplayGraph::on_cmdReset_clicked()
{
    curIteration=0;
    curDelta = ui->spnDelta->value() *1e-4;
    circleLayout();
    updateWidget();
}


void DisplayGraph::deleteNode(DisplayNode * dn)
{
    DisplayGraphNode * node = dynamic_cast<DisplayGraphNode*>(dn);

    int i = n.indexOf(node);
    if(i<0)
    {
        qDebug() << "DisplayGraph::deleteNode: tried to remove not registered node";
        return;
    }

    node->deleteAllEdges();
    delete node;
    n.removeAt(i);

    for(int k=i; k<n.size(); k++)
        n[k]->setId(n[k]->getId()-1);


    QList<DisplayGraphNode*> neighbors = edgeInfo.values(node);
    foreach(DisplayGraphNode * otherNode, neighbors)
    {
        edgeInfo.remove(node,otherNode);
        edgeInfo.remove(otherNode,node);
    }
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
    //edge already exists
    if(edgeInfo.contains(n[i1],n[i2]))
        return;

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
    if(!edgeInfo.contains(n[i1],n[i2]))
        edgeInfo.insert(n[i1],n[i2]);
    if(!edgeInfo.contains(n[i2],n[i1]))
        edgeInfo.insert(n[i2],n[i1]);
}

void DisplayGraph::addNode(DisplayGraphNode * node)
{
    node->setScene(scene);
    node->setPos(qrand()/(double)RAND_MAX , qrand()/(double)RAND_MAX);
    n.push_back(node);
    node->setId(n.size()-1);
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
    if( curDelta <= 0)
    {
        timer->stop();
        return;
    }

    optimalDistance= ui->spnOptimalDistance->value();

    springBasedLayoutIteration(curDelta);

    curIteration++;
    updateWidget();
}



bool DisplayGraph::isAdjacentTo(DisplayGraphNode * n1, DisplayGraphNode * n2) const
{
    //if(n1 >= n2)
    //    qSwap(n1,n2);

    return edgeInfo.contains(n1,n2);
}

qreal DisplayGraph::springBasedLayoutIteration(qreal delta)
{
    static QVector<QPointF> & forces    = forcesList;
    static QVector<QPointF> & oldForces = oldForcesList;

    qSwap(forces,oldForces);

    forces.fill(QPointF(0,0),n.size());

    if(oldForces.size() != n.size())
        oldForces.fill(QPointF(0,0),n.size());


    // Calculate attractive forces between adjacent nodes
    foreach(DisplayGraphNode * n1, n)
    {
        QMultiMap<DisplayGraphNode*, DisplayGraphNode*>::iterator i = edgeInfo.find(n1);
        while (i != edgeInfo.end() && i.key() == n1)
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

        // speed up, if cos(angle between forces) is positive (then cosAngle+1) >1
        // slow down if forces change direction rapidly i.e. cos(angle between forces) near -1
        //qreal cosAngle = getCosBetween(forces[i],oldForces[i])/2 + 1;
        //rotation detection
        //qreal sinAngle = qAbs(getSinBetween(forces[i],oldForces[i]));

        //qDebug() << n[i]->getDisplayName() << "Cos" << cosAngle << "Sin" << sinAngle;

        qreal dx = forces[i].x() * delta;
        qreal dy = forces[i].y() * delta;

        max=qMax(max,dx);
        max=qMax(max,dy);

        dx = qBound(-optimalDistance,dx,optimalDistance);
        dy = qBound(-optimalDistance,dy,optimalDistance);

        //if(qAbs(dx) < 0.2)  dx=0;
        //if(qAbs(dy) < 0.2)  dx=0;

        dx += ((rand()/(double)RAND_MAX) - 0.5 )* ui->spnRandomFactor->value();
        dy += ((rand()/(double)RAND_MAX) - 0.5 )* ui->spnRandomFactor->value();

        n[i]->moveBy(dx,dy);
    }

    return max;
}


qreal DisplayGraph::getCosBetween(QPointF & a, QPointF & b)
{
    qreal normalize = ( a.x() * a.x()  + a.y() * a.y() ) *
                      ( b.x() * b.x()  + b.y() * b.y() );

    if (qAbs(normalize) < 1e-8)
        return 0;

    normalize = sqrt(normalize);

    return ( a.x() * b.x() + a.y() * b.y() ) / normalize;
}


qreal DisplayGraph::getSinBetween(QPointF & a, QPointF & b)
{
    qreal normalize = ( a.x() * a.x()  + a.y() * a.y() ) *
                      ( b.x() * b.x()  + b.y() * b.y() );

    if (qAbs(normalize) < 1e-8)
        return 1;

    normalize = sqrt(normalize);

    return ( a.x() * b.y() - a.y() * b.x() ) / normalize;
}

QPointF DisplayGraph::repulsiveForce (const QPointF & n1, const QPointF & n2, qreal optDist)
{
    if(n1 == n2)
        return QPointF(0,0);



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

// --------------------- DisplayCallGraph ---------------------------------


DisplayCallGraph::DisplayCallGraph(QGraphicsScene * sc,
                                   QObject * par) :
      DisplayGraph(sc,par)
{
    colorNode       = QColor(Qt::blue).lighter(150);;
    colorBorderNode = QColor(Qt::gray).lighter(150);;
}


void DisplayCallGraph::addCgNode(int callGraphNodeId, SgGraphNode * n)
{
    if(!cg)
        return;

    rose_graph_integer_node_hash_map & nodeMap = cg->get_node_index_to_node_map();

    if(callToDisplayNodeMap.contains(callGraphNodeId)) //already added
    {
        qDebug() << "Node already added" << callGraphNodeId;
        return;
    }

    Q_ASSERT(nodeMap.find(callGraphNodeId) != nodeMap.end());
    SgGraphNode  * gn = nodeMap[callGraphNodeId];


    DisplayGraphNode * displayGraphNode =new DisplayGraphNode(gn->get_name().c_str());
    displayGraphNode->setSgNode(n);
    addNode(displayGraphNode);

    qDebug() << "Added Node" << gn->get_name().c_str() << "with id" << callGraphNodeId;

    callToDisplayNodeMap.insert(callGraphNodeId,displayGraphNode);
}

void DisplayCallGraph::addCgEdge(int from, int to)
{
    Q_ASSERT(callToDisplayNodeMap.contains(from));
    Q_ASSERT(callToDisplayNodeMap.contains(to));

    addEdge(callToDisplayNodeMap[from],
            callToDisplayNodeMap[to]); //here edge label could be added

}


void DisplayCallGraph::addFunction(SgNode * node, int depth)
{
    typedef rose_graph_integer_node_hash_map        NodeMap;
    typedef rose_graph_integer_edge_hash_multimap   EdgeMap;
    typedef rose_graph_string_integer_hash_multimap StringNodeMap;


    if(AstDisplayInfo::getType(node) != AstDisplayInfo::FUNCTION)
        return;

    // Find node via function-name string (TODO not unique!!)
    QString funcName = AstDisplayInfo::getShortNodeNameDesc(node);
    StringNodeMap & stringNodeMap = cg->get_string_to_node_index_multimap();
    StringNodeMap::iterator it = stringNodeMap.find(funcName.toStdString());

    if(it == stringNodeMap.end())
        return;

    int nodeIndex= it->second;

    // filter callgraph
    NodeMap filteredNodes;
    EdgeMap filteredEdges;
    QSet<int> visitedNodes;
    generateCgSubGraph(cg,nodeIndex,filteredNodes,filteredEdges,
                       visitedNodes,depth);


    addElements(filteredNodes,filteredEdges);
}

void DisplayCallGraph::clear()
{
    qDeleteAll(n);
    callToDisplayNodeMap.clear();
    cg=NULL;
    edgeInfo.clear();
}

void DisplayCallGraph::setCg(SgIncidenceDirectedGraph * cg_)
{
    // when the callgraph is switched the current display is not valid any more
    if(cg_ != cg && cg !=NULL)
        clear();

    cg=cg_;

    if(cg==NULL)
        return;

    typedef rose_graph_integer_edge_hash_multimap EdgeMap;

    EdgeMap & allEdges = cg->get_node_index_to_edge_multimap();
    EdgeMap & outEdges = cg->get_node_index_to_edge_multimap_edgesOut();
    EdgeMap & inEdges  = cg->get_node_index_to_edge_multimap_edgesIn();


    if(allEdges.size()==0)
    {
        allEdges.insert(outEdges.begin(),outEdges.end());
        allEdges.insert(inEdges.begin(),inEdges.end());
    }

}

void DisplayCallGraph::addElements( rose_graph_integer_node_hash_map & nodes,
                                    rose_graph_integer_edge_hash_multimap & edges)
{
    if(!cg)
        return;

    qDebug() << "Current Nodes";



    for( NodeMap::iterator it = nodes.begin();  it != nodes.end(); ++it )
    {
        addCgNode(it->first,it->second);
    }


    for( EdgeMap::iterator it= edges.begin(); it != edges.end(); ++it )
    {
        SgDirectedGraphEdge * edge = isSgDirectedGraphEdge(it->second);
        Q_ASSERT(edge);
        addCgEdge(edge->get_from()->get_index(),edge->get_to()->get_index());
    }
}


void DisplayCallGraph::deleteNode(DisplayNode * n)
{
    DisplayGraphNode * node = dynamic_cast<DisplayGraphNode*>(n);
    Q_ASSERT(node);

    SgGraphNode* sgGraphNode = isSgGraphNode(node->getSgNode());
    Q_ASSERT(sgGraphNode);

    Q_ASSERT( callToDisplayNodeMap[sgGraphNode->get_index()] == node);
    callToDisplayNodeMap.remove(sgGraphNode->get_index());

    DisplayGraph::deleteNode(n);
}


void DisplayCallGraph::generateCgSubGraph(SgIncidenceDirectedGraph * cg,
                                          int nodeId,
                                          rose_graph_integer_node_hash_map & nodeMapOut,
                                          rose_graph_integer_edge_hash_multimap & edgeMapOut,
                                          QSet<int> & visitedNodes,
                                          int curDepth)
{
    Q_ASSERT(curDepth >= 0);

    if(visitedNodes.contains(nodeId))
        return;

    NodeMap & nodes = cg->get_node_index_to_node_map ();
    Q_ASSERT( nodes.find(nodeId) != nodes.end());

    nodeMapOut.insert(make_pair<int,SgGraphNode*>(nodeId,nodes[nodeId] ));
    visitedNodes.insert(nodeId);


    EdgeMap & edges = cg->get_node_index_to_edge_multimap();

    EdgeMap::iterator edgeIt = edges.find(nodeId);
    for(; edgeIt != edges.end() && edgeIt->first == nodeId; ++edgeIt )
    {
        SgDirectedGraphEdge * edge = isSgDirectedGraphEdge(edgeIt->second);
        Q_ASSERT(edge);

        // Add edge
        int newNodeIndex = edge->get_to()->get_index();
        if(newNodeIndex == nodeId)
            newNodeIndex = edge->get_from()->get_index();

        if(curDepth > 0)
        {
            edgeMapOut.insert(*edgeIt);
            generateCgSubGraph(cg,newNodeIndex,nodeMapOut,edgeMapOut,
                               visitedNodes,curDepth-1);
        }
        else
        {
            if(visitedNodes.contains(newNodeIndex))
                edgeMapOut.insert(*edgeIt);
        }
    }
}






#if 0
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
#endif






