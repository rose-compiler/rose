#include "PropertyTreeModel.h"

#include <QBrush>
#include <QFont>
#include <QColor>
#include <QApplication>
#include <QPalette>
#include <QDebug>

#include "ItemTreeNode.h"

// ------------------ Header Node  -----------------------------


class PropertyTreeModel::ItemTreeHeaderNode : public ItemTreeNode
{
	public:
		ItemTreeHeaderNode(const QString & title, int sectionId);
		virtual ~ItemTreeHeaderNode() {}


		virtual QVariant data(int role, int column=0) const;

		int getSectionId() const { return sectionId; }

	        virtual QStringList sectionHeader() const;

	        bool isFirstColumnSpanned () const    { return true;}
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
QStringList PropertyTreeModel::ItemTreeHeaderNode::sectionHeader() const
{
    return QStringList() << "Property" << "Value";
}






// ------------------ PropValue Node  -----------------------------


class PropertyTreeModel::ItemTreePropValueNode : public PropertyValueNode
{
    public:
        ItemTreePropValueNode(const QString & p, const QVariant & v);

        virtual QVariant data(int role, int column=0) const;
        int getSectionId() const;
};
PropertyTreeModel::ItemTreePropValueNode::ItemTreePropValueNode(const QString & p, const QVariant & v)
    : PropertyValueNode(p,v)
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
    if(role == Qt::UserRole)
        return getSectionId();
    else
        return PropertyValueNode::data(role,column);
}







// --------------- Implementation of the AbstractItemModel -----------------------


PropertyTreeModel::PropertyTreeModel(QObject * p)
	: ItemTreeModel(p)
{
	treeRoot = new ItemTreeHeaderNode("root",-1);
	setRoot(treeRoot);
}

PropertyTreeModel::~PropertyTreeModel()
{
}


void PropertyTreeModel::clear()
{
	treeRoot=new ItemTreeHeaderNode("root",-1);
    setRoot(treeRoot);
}

int PropertyTreeModel::addSection(const QString & sectionName)
{
	int curRowCount = rowCount();

	int id= treeRoot->addChild(new ItemTreeHeaderNode(sectionName,curRowCount));

	return id;
}

QModelIndex PropertyTreeModel::addEntryToSection(int sectionId, const QString & pr, const QVariant & value)
{
	Q_ASSERT(sectionId>=0);
	Q_ASSERT(sectionId< treeRoot->childrenCount() );


	int curRowCount = rowCount(index(sectionId,0));

	ItemTreeNode * newNode= new ItemTreePropValueNode(pr,value);
	treeRoot->child(sectionId)->addChild(newNode);

	return createIndex(curRowCount,0,newNode);
}

QModelIndex PropertyTreeModel::addEntry(const QModelIndex & par,
                                        const QString & prop,
                                        const QVariant & val)
{
	int curRowCount = rowCount(par);

	ItemTreeNode * parNode = static_cast<ItemTreeNode*>(par.internalPointer());
	ItemTreeNode * newNode = new ItemTreePropValueNode(prop,val);
	parNode->addChild(newNode);

	return createIndex(curRowCount,0,newNode);
}

