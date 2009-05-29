#include "PropertyTreeModel.h"

#include <QBrush>
#include <QFont>
#include <QColor>
#include <QApplication>
#include <QPalette>
#include <QDebug>

#include "util/ItemTreeNode.h"

// ------------------ Helper Classes to build the tree -----------------------


class PropertyTreeModel::ItemTreeHeaderNode : public ItemTreeNode
{
	public:
		ItemTreeHeaderNode(const QString & title, int sectionId);
		virtual ~ItemTreeHeaderNode() {}


		virtual QVariant data(int role, int column=0) const;

		int getSectionId() const { return sectionId; }
	protected:
		QString title;
		int sectionId;
};


PropertyTreeModel::ItemTreeHeaderNode::ItemTreeHeaderNode(const QString & t, int sId)
	: title(t),
	  sectionId(sId)
{}

QVariant PropertyTreeModel::ItemTreeHeaderNode::data(int role,int column) const
{
	QFont boldFont;
	boldFont.setBold(true);
	boldFont.setPointSize(boldFont.pointSize()+1);
	switch(role)
	{
		case Qt::DisplayRole:
			return (column==0) ? title : QVariant();
		case Qt::BackgroundRole:
			return QApplication::palette().mid();
		case Qt::FontRole:
			return QVariant( boldFont );
		default:
			return QVariant();
	}
}



class PropertyTreeModel::ItemTreePropValueNode : public ItemTreeNode
{
	public:
		ItemTreePropValueNode(const QString & property, const QVariant & value);

		virtual QVariant data(int role, int column=0) const;

		int getSectionId() const;

	protected:
		QString prop;
		QVariant val;
};



PropertyTreeModel::ItemTreePropValueNode::ItemTreePropValueNode(const QString & p, const QVariant & v)
	: prop(p), val(v)
{
}


int PropertyTreeModel::ItemTreePropValueNode::getSectionId() const
{
	//Assume that parent is a ItemTreeHeader Node
	ItemTreeHeaderNode * p = dynamic_cast<ItemTreeHeaderNode*>(parent);

	if(p==NULL)
	{
		// Item could also be
		ItemTreePropValueNode * n= dynamic_cast<ItemTreePropValueNode*>(parent);
		if(n==NULL)
		{
			//The parent of a PropValueNode should either be a PropValue node or HeaderNode
			qDebug() << "Unexpected Tree Structure";
			return 0;
		}
		return n->getSectionId();
	}
	return p->getSectionId();
}

QVariant PropertyTreeModel::ItemTreePropValueNode::data(int role, int column) const
{
	switch(role)
	{
		case Qt::DisplayRole:
			if      (column==0)  return prop;
			else if (column==1)  return val;
			else                 return QVariant();
		case Qt::UserRole:
			return getSectionId();
		default:
			return QVariant();
	}
}



// --------------- Implementation of the AbstractItemModel -----------------------


PropertyTreeModel::PropertyTreeModel(QObject * p)
	: ItemTreeModel(p)
{
	treeRoot = new ItemTreeNode();
	setRoot(treeRoot);

	headerCaptions() << "Property" << "Value";
}

PropertyTreeModel::~PropertyTreeModel()
{
}


void PropertyTreeModel::clear()
{
	treeRoot=new ItemTreeNode();
    setRoot(treeRoot);
}

int PropertyTreeModel::addSection(const QString & sectionName)
{
	int curRowCount = rowCount();

	beginInsertRows(QModelIndex(), curRowCount,curRowCount);
	int id= treeRoot->addChild(new ItemTreeHeaderNode(sectionName,curRowCount));
	endInsertRows();

	return id;
}

QModelIndex PropertyTreeModel::addEntryToSection(int sectionId, const QString & pr, const QVariant & value)
{
	Q_ASSERT(sectionId>=0);
	Q_ASSERT(sectionId< treeRoot->childrenCount() );


	int curRowCount = rowCount(index(sectionId,0));

	beginInsertRows(index(sectionId,0),curRowCount,curRowCount);
	ItemTreeNode * newNode= new ItemTreePropValueNode(pr,value);
	treeRoot->child(sectionId)->addChild(newNode);
	endInsertRows();

	return createIndex(curRowCount,0,newNode);
}

QModelIndex PropertyTreeModel::addEntry(const QModelIndex & par,
									    const QString & prop,
										const QVariant & val)
{
	int curRowCount = rowCount(par);

	beginInsertRows(par,curRowCount,curRowCount);
	ItemTreeNode * parNode = static_cast<ItemTreeNode*>(par.internalPointer());
	ItemTreeNode * newNode = new ItemTreePropValueNode(prop,val);
	parNode->addChild(newNode);
	endInsertRows();

	return createIndex(curRowCount,0,newNode);
}

