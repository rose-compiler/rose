
#ifndef ITEMTREENODE_H
#define ITEMTREENODE_H

#include <QList>
#include <QVariant>


/**
 * Can be used to build a QT-TreeModel
 * to subclass from QAbstractTreeModel
 */
class ItemTreeNode
{
	public:
		// Constructor to use for root-node
		// to construct tree use addChild instead
		ItemTreeNode();
		virtual ~ItemTreeNode();

		virtual QVariant data(int role, int column=0) const;

		virtual ItemTreeNode * getParent() const 	{ return parent; }

		virtual Qt::ItemFlags flags (int column) const { return Qt::ItemIsEnabled | Qt::ItemIsSelectable; }

		virtual ItemTreeNode * child(int id) { return children[id]; }
		virtual int childrenCount() const    { return children.size(); }

		virtual int addChild(ItemTreeNode * c);

		/// Returns the row of this Node
		/// goes to the parent and returns the id it has in its children list
		virtual int getRow();


	protected:
		ItemTreeNode * parent;
		QList<ItemTreeNode*> children;
};





#endif
