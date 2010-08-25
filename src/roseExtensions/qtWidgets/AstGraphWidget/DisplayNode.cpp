#include "sage3basic.h"
#include "DisplayNode.h"

#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>
#include <QDebug>

#include "DisplayEdge.h"


// ---------------------- DisplayNode -------------------------------


DisplayNode::DisplayNode(QGraphicsScene * sc)
    : scene(0),caption("Root"),sg(NULL)
{
    setScene(sc);
    init();
}

DisplayNode::DisplayNode(const QString & cap, QGraphicsScene * sc)
    : scene(0),caption(cap),sg(NULL)
{
    setScene(sc);
    init();
}



void DisplayNode::init()
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setZValue(1);

    mouseHold=false;
#if QT_VERSION >= 0x040400
    setCacheMode(DeviceCoordinateCache);
#endif
    setScene(scene);

    bgColor = QColor(Qt::blue).lighter(150);
    bgColorSelected = Qt::green;
}


void DisplayNode::setBgColor(const QColor & c)
{
    bgColor = c;
    update();
}

void DisplayNode::setBgColorSelected(const QColor &c)
{
    bgColorSelected=c;
    update();
}


QRectF DisplayNode::boundingRect() const
{
    static const int MARGIN = 10;
    QFontMetrics fi (textFont);
    float width=fi.width(caption)+MARGIN;
    float height=fi.height()+MARGIN;

    return QRectF(-width/2,-height/2,width,height);
}

QPainterPath DisplayNode::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}


void DisplayNode::paint(QPainter * painter,const QStyleOptionGraphicsItem * , QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::black);
    pen.setWidth(2);
    painter->setPen(pen);

    QBrush brush( isSelected() ? bgColorSelected : bgColor );
    painter->setBrush(brush);

#if QT_VERSION >= 0x040400
    painter->drawRoundedRect(boundingRect(),20,20,Qt::RelativeSize);
#else
    painter->drawRect(boundingRect());
#endif

    painter->drawText(boundingRect(),Qt::AlignCenter,caption);
}


void DisplayNode::setScene(QGraphicsScene * s)
{
    if(s==scene)
        return;

    if(s==NULL)
    {
        if(scene) scene->removeItem(this);
        scene=NULL;
        return;
    }

    scene=s;
    scene->addItem(this);
}

QVariant DisplayNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemSelectedChange)
        update();

    return QGraphicsItem::itemChange(change, value);
}


void DisplayNode::mousePressEvent ( QGraphicsSceneMouseEvent * ev )
{
    mouseHold=true;
    QGraphicsItem::mousePressEvent(ev);
}

void DisplayNode::mouseReleaseEvent ( QGraphicsSceneMouseEvent * ev )
{
    mouseHold=false;
    QGraphicsItem::mouseReleaseEvent(ev);
}

// ---------------------- DisplayTreeNode -------------------------------


DisplayTreeNode::DisplayTreeNode(QGraphicsScene * sc)
	: parentEdge(NULL)
{
}

DisplayTreeNode::~DisplayTreeNode()
{
    // Deletion of subtree
	foreach(DisplayEdge * e, childEdges)
	{
	    // the child node is deleted (and with it the whole subtree)
	    delete e->destNode();
	    // deletes the edge itself
	    delete e;
	}

   //Only outgoing edges are deleted to prevent double frees
    foreach(DisplayEdge * e, additionalEdges)
        if(e->sourceNode()==this)
            delete e;
}

DisplayTreeNode * DisplayTreeNode::addChild(SgNode * sgNode)
{
	DisplayTreeNode * child = addChild(sgNode->class_name().c_str());
	child->setSgNode(sgNode);

	return child;
}

DisplayTreeNode * DisplayTreeNode::addChild(const QString & dispName)
{
	DisplayTreeNode * child = new DisplayTreeNode(scene);
	child->setDisplayName(dispName);

	addChild(child);

	return child;
}

void DisplayTreeNode::addChild(DisplayTreeNode * child)
{
    DisplayEdge * newEdge = new DisplayEdge(this,child);

    if(scene) scene->addItem(newEdge);

    childEdges.push_back(newEdge);

    child->parentEdge = newEdge;
	child->setScene(scene);
}


DisplayTreeNode * DisplayTreeNode::getChild(int id)
{
    DisplayTreeNode * res;
    res = dynamic_cast<DisplayTreeNode*>(childEdges[id]->destNode() );

    Q_ASSERT(res); // the tree should only contain tree-nodes
    return res;
}

DisplayTreeNode * DisplayTreeNode::getFirstChild()
{
    Q_ASSERT(childrenCount() > 0);
    return getChild(0);
}

DisplayTreeNode * DisplayTreeNode::getLastChild()
{
    Q_ASSERT(childrenCount() > 0);
    return getChild(childrenCount()-1);
}





DisplayTreeNode * DisplayTreeNode::getParent()
{
    if(!parentEdge)
        return NULL;


    DisplayTreeNode * res;
    res = dynamic_cast<DisplayTreeNode*>(parentEdge->sourceNode() );

    Q_ASSERT(res); // the tree should only contain tree-nodes
    return res;
}

void DisplayTreeNode::registerAdditionalEdge(DisplayEdge *e)
{
    if(scene) scene->addItem(e);

	additionalEdges.push_back(e);
}


void DisplayTreeNode::setScene(QGraphicsScene * sc)
{
    for(int i=0; i< childrenCount(); i++)
        getChild(i)->setScene(sc);

    if(scene && !sc)
    {
        foreach(DisplayEdge * e, childEdges)
            scene->removeItem(e);

        foreach(QGraphicsItem * e,additionalEdges)
            scene->removeItem(e);

        return;
    }

    if(sc)
    {
        foreach(DisplayEdge * e, childEdges)
            sc->addItem(e);

        foreach(DisplayEdge * e,additionalEdges)
            sc->addItem(e);
    }

    DisplayNode::setScene(sc);
}


DisplayTreeNode * DisplayTreeNode::mergeTrees(QGraphicsScene * sc,
                                              const QString & name,
                                              DisplayTreeNode * n1,
                                              DisplayTreeNode * n2)
{
	DisplayTreeNode * newNode= new DisplayTreeNode(sc);
	newNode->setDisplayName(name);

	n1->setScene(sc);
	n2->setScene(sc);

	newNode->addChild(n1);
	newNode->addChild(n2);
	return newNode;
}


QVariant DisplayTreeNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionHasChanged)
    {
        foreach(DisplayEdge * edge, childEdges)
            edge->adjust();

        foreach(DisplayEdge * edge,additionalEdges)
            edge->adjust();

        if(parentEdge)
        {
            parentEdge->adjust();

            // child can not move above parent
            //if(pos().y() <  getParent()->pos().y())
            //    setPos( QPointF(pos().x(),getParent()->pos().y()) );

            /*            DisplayTreeNode * par = getParent();
            if(pos().y() <  par->pos().y())
                setPos( QPointF(pos().x(),par->pos().y()) );*/
        }
    }

    return DisplayNode::itemChange(change, value);
}

void DisplayTreeNode::deleteChildren(int from, int to)
{
	typedef QList<DisplayEdge*>::iterator ChildIter;

    ChildIter begin = childEdges.begin() + from;
    ChildIter end   = childEdges.begin() + to;

    for(ChildIter remIter=begin; remIter<end; ++remIter )
    {
        // delete node
        delete (*remIter)->destNode();
        // delete edge
        delete *remIter;
    }
    childEdges.erase(begin,end);

}

void DisplayTreeNode::simplifyTree(DisplayTreeNode * node)
{
	// If n successing children have no subchildren and have same caption
	// delete them, and create node with caption "n x oldcaption"


	int startIndex = -1;
	QString curName;
	bool inSequence=false;



	typedef QList<DisplayTreeNode *>::iterator ListIter;

	for(int i=0; i< node->childrenCount(); i++)
	{
		DisplayTreeNode * child = node->getChild(i);

		//End of Sequence
		if(inSequence && (child->childrenCount() > 0 ||
                                  child->getDisplayName() != curName) )
                {
			if (i - startIndex > 1)
			{
			    node->getChild(startIndex)->caption=QString("%1 x ").arg(i-startIndex);
			    node->getChild(startIndex)->caption.append(curName);
				Q_ASSERT(node->getChild(startIndex)->childrenCount()==0);

				// children[startIndex] is not deleted, but overwritten
				node->deleteChildren(startIndex +1, i);

				i -=  i - (startIndex-1);
			}
			inSequence=false;
		}

		//Start of Sequence
		if(!inSequence && child->childrenCount() == 0)
		{
			inSequence=true;
			startIndex=i;
			curName=child->getDisplayName();
		}
	}

	// Traverse...
	for(int i=0; i< node->childrenCount(); i++)
	    simplifyTree(node->getChild(i));
}


// ---------------- Tree Generator ------------------

DisplayTreeNode * DisplayTreeGenerator::generateTree(SgNode *sgRoot, AstFilterInterface * filter)
{
	treeRoot=new DisplayTreeNode();
	treeRoot->sg=sgRoot;
	treeRoot->caption = sgRoot->class_name().c_str();
	treeRoot->parentEdge=NULL;

	for(unsigned int i=0; i< sgRoot->get_numberOfTraversalSuccessors(); i++)
		visit(treeRoot,sgRoot->get_traversalSuccessorByIndex(i),filter);

	Q_ASSERT(treeRoot);
	return treeRoot;
}


void DisplayTreeGenerator::visit(DisplayTreeNode * parent, SgNode * sgNode, AstFilterInterface * filter)
{
	if(filter && !filter->displayNode(sgNode))
			return;

	if(sgNode==NULL)
		return;

	DisplayTreeNode * dispNode = parent->addChild(sgNode);

	for(unsigned int i=0; i< sgNode->get_numberOfTraversalSuccessors(); i++)
		visit(dispNode,sgNode->get_traversalSuccessorByIndex(i),filter);
}













