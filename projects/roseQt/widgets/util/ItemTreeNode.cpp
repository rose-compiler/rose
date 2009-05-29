#include "ItemTreeNode.h"

#include <QList>


ItemTreeNode::ItemTreeNode()
	: parent(NULL)
{}

ItemTreeNode::~ItemTreeNode()
{
	qDeleteAll(children);
}

int ItemTreeNode::addChild(ItemTreeNode * c)
{
	children.push_back(c);
	c->parent=this;
	return children.size()-1;
}

int ItemTreeNode::getRow()
{
	//assume that there is only one node on highest level (one root)
	if(parent==NULL)
		return 0;

	return parent->children.indexOf(this);
}

QVariant ItemTreeNode::data(int role, int column) const
{
	return QString("NoData");
}




