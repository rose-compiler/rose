#include "DisplayGraphNode.h"
#include "DisplayEdge.h"

#include <QGraphicsScene>

#include <QDebug>

// ---------------------- DisplayGraphNode -------------------------------

DisplayGraphNode::DisplayGraphNode(QGraphicsScene * sc)
    : DisplayNode(sc)
{
}

DisplayGraphNode::DisplayGraphNode(const QString & caption,QGraphicsScene * sc)
    : DisplayNode(caption,sc)
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
    //TODO optimize
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

DisplayGraph::DisplayGraph(QObject * par)
    : QObject(par),
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



void DisplayGraph::addEdge(DisplayGraphNode * n1, DisplayGraphNode * n2)
{
    DisplayEdge * e = new DisplayEdge(n1,n2);
    e->setPaintMode(DisplayEdge::STRAIGHT);
    DisplayGraphNode::addEdge(e);
}

void DisplayGraph::addEdge ( int i1, int i2)
{
    addEdge(n[i1],n[i2]);
}

void DisplayGraph::addNode(DisplayGraphNode * node)
{
    n.push_back(node);
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


void DisplayGraph::springBasedLayoutIteration(qreal delta)
{
    for(int i=0; i < n.size(); i++)
    {
        QPointF randComp =  QPointF(qrand()/(double)RAND_MAX,qrand()/(double)RAND_MAX) -QPointF(0.5,0.5);
        n[i]->setPos(n[i]->pos() + ui->spnRandomFactor->value() * randComp );

        QPointF force(0,0);
        for(int j=0; j<n.size(); j++)
        {
            if(i==j)
                continue;

            if(n[i]->isMouseHold())
                continue;

            if(n[i]->isAdjacentTo(n[j]))
                force += attractiveForce(n[i]->pos(),n[j]->pos());

            force += repulsiveForce(n[i]->pos(),n[j]->pos());
        }

        //qDebug() << "Force" << force;
        n[i]->setPos(n[i]->pos() + delta * force );
        //qDebug() << "Pos for" << i << n[i]->pos() + delta * force;
    }
}

QPointF DisplayGraph::repulsiveForce (const QPointF & n1, const QPointF & n2)
{
    //static const qreal OPT_SQ = OPTIMAL_DISTANCE*OPTIMAL_DISTANCE;
    if(n1 == n2)
    {
        qDebug() << "GraphLayout RepForce Warning";
        return QPointF(0,0);
    }


    QLineF l (n1,n2);
    QPointF v (n1-n2);
    QPointF res =( optimalDistance *optimalDistance / l.length() ) * v;
    return  res;
}

QPointF DisplayGraph::attractiveForce(const QPointF & n1, const QPointF & n2)
{
    if(n1 == n2)
    {
        qDebug() << "GraphLayout AttrForce Warning";
        return QPointF(0,0);
    }

    QPointF v( n2-n1);
    QPointF res =( (v.x()*v.x() + v.y() * v.y())  / optimalDistance) * v;
    return res;
}



DisplayGraph * DisplayGraph::generateTestGraph(QGraphicsScene * sc,QObject * par)
{
    DisplayGraph * g = new DisplayGraph(par);

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

    return g;
}




