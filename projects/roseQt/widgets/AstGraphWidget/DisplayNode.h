#ifndef DISPLAYNODE_H
#define DISPLAYNODE_H


#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QList>
#include <QFont>


class SgNode;
class SgGraphicsScene;

class DisplayEdge;

class DisplayNode : public QGraphicsItem
{
	friend class DisplayTreeGenerator;

	public:
		DisplayNode(QGraphicsScene * scene=NULL);
		virtual ~DisplayNode();


		/// Display Name is the String which is printed in the node
		const QString & getDisplayName() 		{ return caption; }
		void setDisplayName(const QString& n) 	{ caption=n;      }


		/// Returns the Sage Node which corresponds to this DisplayNode
		SgNode * getSgNode() 					{ return sg; }


		/// Functions required by QGraphicsScene
		QRectF       boundingRect() const;
		QPainterPath shape()        const;

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);



		DisplayNode * getParent() 			     { return parent; }
		int childrenCount() const    			 { return children.size(); }
		const QList<DisplayNode *> getChildren() { return children; }
		DisplayNode * getChild (int i)           { return children[i]; }

		//Deletes Children in range [from,to)
		//to is exclusive!
		void deleteChildren(int from, int to);

		void setScene(QGraphicsScene * scene);

		//Constructor for generating a tree manual for debug purposes
		DisplayNode * addChild(const QString & dispName);
		void addChild(DisplayNode * c);


		/// Each user defined QGraphicsItem needs a unique constant
		enum { DisplayNodeType = UserType + 1 };
		int type() const { return DisplayNodeType; }


		static DisplayNode * mergeTrees(QGraphicsScene * scene,
										const QString & name,
										DisplayNode * n1,
										DisplayNode * n2);

		static void simplifyTree(DisplayNode * node);


		void registerAdditionalEdge(DisplayEdge *e);

	protected:
		QVariant itemChange(GraphicsItemChange change, const QVariant &value);

		// Makes sure that edges start and end on right position
		void adjust(int childId);
		void adjust(DisplayNode * child);

		QRectF getRect() const;

		DisplayNode * addChild(SgNode * sgNode);


		QGraphicsScene * scene;
		QFont textFont;
		QList<DisplayEdge* > edges;
		DisplayEdge * parentEdge;

		QList<DisplayEdge* > additionalEdges;


		QList<DisplayNode* >  children;
		DisplayNode * parent;


		QString caption;
		SgNode * sg;
};


#include "util/AstFilters.h"

class DisplayTreeGenerator
{
	public:
		DisplayNode * generateTree(SgNode * sgRoot, AstFilterInterface * filter=NULL);

	protected:
		void visit(DisplayNode * parent, SgNode * sgNode, AstFilterInterface * filter);

		DisplayNode * treeRoot;

};


#endif
