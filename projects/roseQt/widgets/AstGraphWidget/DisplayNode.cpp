#include "rose.h"
#include "DisplayNode.h"

#include <QGraphicsScene>
#include <QFontMetrics>
#include <QPainter>
#include <QDebug>

#include "DisplayEdge.h"

DisplayNode::DisplayNode(QGraphicsScene * sc)
	: scene(sc), parentEdge(NULL), parent(NULL),sg(NULL)
{
	 setFlag(ItemIsMovable);
	 setFlag(ItemIsSelectable);

#if QT_VERSION >= 0x040400
     setCacheMode(DeviceCoordinateCache);
#endif


	 setZValue(1);

	 caption="Root";

	 setScene(sc);

}

DisplayNode::~DisplayNode()
{
	//qDebug() << "Destroying" << caption;

	//Only outgoing edges are deleted to prevent double frees
	foreach(DisplayEdge * e, additionalEdges)
		if(e->sourceNode()==this)
			delete e;


	qDeleteAll(children);
	qDeleteAll(edges);
}

QRectF DisplayNode::boundingRect() const
{
	return getRect();
}

QPainterPath DisplayNode::shape() const
{
	QPainterPath path;
	path.addRect(getRect());
	return path;
}

void DisplayNode::paint(QPainter * painter,const QStyleOptionGraphicsItem * , QWidget*)
{
	painter->setRenderHint(QPainter::Antialiasing, true);

	QPen pen(Qt::black);
	pen.setWidth(2);
	painter->setPen(pen);


	QColor color=Qt::blue;
	if(isSelected())
		color = Qt::green;
	else
		color = color.lighter(150);

	QBrush brush(color);
	painter->setBrush(brush);

#if QT_VERSION >= 0x040400
	painter->drawRoundedRect(getRect(),20,20,Qt::RelativeSize);
#else
	painter->drawRect(getRect());
#endif

	painter->drawText(getRect(),Qt::AlignCenter,caption);


}


QRectF DisplayNode::getRect() const
{
	static const int MARGIN = 10;
	QFontMetrics fi (textFont);
	float width=fi.width(caption)+MARGIN;
	float height=fi.height()+MARGIN;

	return QRectF(-width/2,-height/2,width,height);
}


DisplayNode * DisplayNode::addChild(SgNode * sgNode)
{
	DisplayNode * child = addChild(sgNode->class_name().c_str());
	child->sg=sgNode;

	return child;
}

DisplayNode * DisplayNode::addChild(const QString & dispName)
{
	DisplayNode * child=new DisplayNode(scene);
	child->caption=dispName;

	addChild(child);

	return child;
}

void DisplayNode::addChild(DisplayNode * child)
{
	children.push_back(child);
	child->parent=this;
	child->setScene(scene);

	// Create and link edge
	DisplayEdge * edge= new DisplayEdge(this,child);
	child->parentEdge=edge;
	if(scene) scene->addItem(edge);
	edges.push_back(edge);
}


void DisplayNode::registerAdditionalEdge(DisplayEdge *e)
{
	scene->addItem(e);
	additionalEdges.push_back(e);
}


void DisplayNode::setScene(QGraphicsScene * s)
{
    foreach(DisplayNode * child,children)
		child->setScene(s);

    if(s==scene)
        return;

    scene=s;

    if(scene==NULL)
        return;

	scene->addItem(this);

	foreach(QGraphicsItem * e,edges)
		scene->addItem(e);

	foreach(QGraphicsItem * e,additionalEdges)
		scene->addItem(e);

}


DisplayNode * DisplayNode::mergeTrees(QGraphicsScene * sc,
									   const QString & name,
									   DisplayNode * n1,
									   DisplayNode * n2)
{
	DisplayNode * newNode= new DisplayNode(sc);
	newNode->setDisplayName(name);

	n1->setScene(sc);
	n2->setScene(sc);

	newNode->addChild(n1);
	newNode->addChild(n2);
	return newNode;
}


QVariant DisplayNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
		case ItemPositionHasChanged:
				foreach(DisplayEdge * edge, edges)
					edge->adjust();

				foreach(DisplayEdge * edge,additionalEdges)
					edge->adjust();

				if(parent)
				{
					parentEdge->adjust();

					if(pos().y() < parent->pos().y())
						setPos( QPointF(pos().x(),parent->pos().y()) );
				}
			break;

		case ItemSelectedChange:
			update();
			break;
		default:
			break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void DisplayNode::deleteChildren(int from, int to)
{
	typedef QList<DisplayNode*>::iterator       ChildIter;
	typedef QList<DisplayEdge*>::iterator EdgeIter;

	{
		ChildIter begin = children.begin() + from;
		ChildIter end   = children.begin() + to;

		for(ChildIter remIter=begin; remIter<end; ++remIter )
			delete *remIter;
		children.erase(begin,end);
	}

	{
		EdgeIter begin = edges.begin() + from;
		EdgeIter end   = edges.begin() + to;
		for(EdgeIter remIter=begin; remIter<end; ++remIter )
			delete *remIter;
		edges.erase(begin,end);
	}
}

void DisplayNode::simplifyTree(DisplayNode * node)
{
	// If n successing children have no subchildren and have same caption
	// delete them, and create node with caption "n x oldcaption"


	int startIndex = -1;
	QString curName;
	bool inSequence=false;



	typedef QList<DisplayNode *>::iterator ListIter;

	QList<DisplayNode*> & children = node->children;



	for(int i=0; i<children.size(); i++)
	{
		DisplayNode * child = children[i];

		//End of Sequence
		if(inSequence && (child->childrenCount() > 0 ||
						  child->getDisplayName() != curName) )
		{
			if (i - startIndex > 1)
			{
				children[startIndex]->caption=QString("%1 x ").arg(i-startIndex);
				children[startIndex]->caption.append(curName);
				Q_ASSERT(children[startIndex]->childrenCount()==0);

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
	foreach(DisplayNode * child, children)
		simplifyTree(child);
}


// ---------------- Tree Generator ------------------

DisplayNode * DisplayTreeGenerator::generateTree(SgNode *sgRoot, AstFilterInterface * filter)
{
	treeRoot=new DisplayNode();
	treeRoot->sg=sgRoot;
	treeRoot->caption = sgRoot->class_name().c_str();
	treeRoot->parent=NULL;

	for(unsigned int i=0; i< sgRoot->get_numberOfTraversalSuccessors(); i++)
		visit(treeRoot,sgRoot->get_traversalSuccessorByIndex(i),filter);

	Q_ASSERT(treeRoot);
	return treeRoot;
}


void DisplayTreeGenerator::visit(DisplayNode * parent, SgNode * sgNode, AstFilterInterface * filter)
{
	if(filter && !filter->displayNode(sgNode))
			return;

	if(sgNode==NULL)
		return;

	DisplayNode * dispNode = parent->addChild(sgNode);

	for(unsigned int i=0; i< sgNode->get_numberOfTraversalSuccessors(); i++)
		visit(dispNode,sgNode->get_traversalSuccessorByIndex(i),filter);
}




