#ifndef DISPLAYNODE_H
#define DISPLAYNODE_H


#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QList>
#include <QFont>


class SgNode;
class SgGraphicsScene;

class DisplayEdge;

/**
 * \brief Class for drawing a Node on a QGraphicsScene
 * each node may have a caption and an SgNode associated to it
 */
class DisplayNode : public QGraphicsItem
{
    public:
        /// Each user defined QGraphicsItem needs a unique constant
        enum { DisplayNodeType = UserType + 1 };


        /// The Node is deleted by the scene, if scene is not null
        DisplayNode(QGraphicsScene * scene=NULL);
        DisplayNode(const QString & caption, QGraphicsScene * scene = NULL);

        virtual ~DisplayNode() {}

        /// Display Name is the String which is printed in the node
        virtual const QString & getDisplayName()       { return caption; }
        virtual void setDisplayName(const QString& n)  { caption=n;      }


        /// Background color, when not selected
        virtual void   setBgColor         (const QColor & c);
        virtual void   setBgColorSelected (const QColor & c);

        virtual QColor getBgColor()         const { return bgColor; }
        virtual QColor getBgColorSelected() const { return bgColorSelected; }


        /// Returns the Sage Node which corresponds to this DisplayNode
        virtual SgNode * getSgNode()                   { return sg; }
        virtual void setSgNode(SgNode * sgNode)        { sg = sgNode; }

        /// Functions required by QGraphicsScene
        virtual QRectF       boundingRect() const;
        virtual QPainterPath shape()        const;
        virtual int          type()         const      { return DisplayNodeType; }



        /// Displays this node on another scene, the scene of the edges is not changed!
        /// if scene is NULL the node is removed from current scene,
        /// and the ownership is passes to the caller
        virtual void setScene(QGraphicsScene * scene);

        /// Custom Paint Event
        virtual void paint   (QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget);

        virtual bool isMouseHold() const { return mouseHold; }


    protected:
        virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );


        void init();

        /// Needed to listen on selection-changes
        /// node is drawn differently when selected)
        virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

        /// GraphicsScene where this node is rendered, or NULL if in no scene
        /// the scene takes ownership of nodes
        QGraphicsScene * scene;

        /// Text which is rendered inside of node
        QString caption;

        /// SgNode associated with this DisplayNode
        /// not used internally
        SgNode * sg;

        /// The font used for drawing the caption
        QFont textFont;

        /// Background color if node is not selected
        QColor bgColor;
        /// Background color when node is selected
        QColor bgColorSelected;

        /// True when a mouse button is pressed over this node
        bool mouseHold;
};


/**
 * \brief A DisplayNode used to manage a tree
 *
 * A DisplayTreeNode is a DisplayGraphNode which has only outputEdges (children)
 * and a dedicated inputEdge (parent)
 * - The memory handling is done by the parent node (when parent is deleted the whole
 *   subtree, edges and additionalEdges are deleted)
 * - each node owns the edges to the childs, parentEdge is deleted by parent
 */
class DisplayTreeNode : public DisplayNode
{
        friend class DisplayTreeGenerator;

        public:
                DisplayTreeNode(QGraphicsScene * scene=NULL);
                virtual ~DisplayTreeNode();


                DisplayTreeNode * getParent();
                int childrenCount() const            { return childEdges.size(); }
                DisplayTreeNode * getChild (int i);
                DisplayTreeNode * getFirstChild();
                DisplayTreeNode * getLastChild();


                //Deletes Children in range [from,to)
                //'to' is exclusive
                void deleteChildren(int from, int to);


                /// Inserts a child with a given displayName in the tree
                DisplayTreeNode * addChild(const QString & dispName);

                /// Adds an existing DisplayTreeNode (and with it a subtree)
                /// to the tree (DisplayTreeNode takes ownership of c)
                void addChild(DisplayTreeNode * c);

                /// Overloaded to set recursively the scene in the children
                virtual void setScene(QGraphicsScene * s);

                /// Additional edges for displaying can be added here
                /// takes ownership of the edge
                /// f.e. for debug purposes
                void registerAdditionalEdge(DisplayEdge *e);

                /// Generates a new Tree out of two separate subtrees
                /// @param scene the scene of the merged tree, the subtrees are moved to that scene
                /// @param name  displayName of the new root node
                /// @param n1    left subtree
                /// @param n2    right subtree
                static DisplayTreeNode * mergeTrees(QGraphicsScene * scene,
                                                    const QString & name,
                                                    DisplayTreeNode * n1,
                                                    DisplayTreeNode * n2);


                /// If n successing children have no subchildren and have same caption
        /// delete them, and create node with caption "n x oldcaption"
                /// useful f.e. in an Sage-AST when a lot nodes with the same caption
                /// appear after each other
                static void simplifyTree(DisplayTreeNode * node);

        protected:
                QVariant itemChange(GraphicsItemChange change, const QVariant &value);

                DisplayTreeNode * addChild(SgNode * sgNode);

                /// Child Edges go from parent to child
                QList<DisplayEdge* > childEdges;
                /// Additional edges, difference to childEdges: target is not deleted when node is deleted
                /// each node deletes its outgoing additionalEdges
        QList<DisplayEdge* > additionalEdges;

        /// Source node of parentEdge is the parent, destination is this node
                DisplayEdge * parentEdge;
};


#include "AstFilters.h"

/**
 * \brief Class to build up a tree out of DisplayTreeNode 's
 *
 * has no real members - functions should be static functions of DisplayTreeNode (TODO - refactor)
 */
class DisplayTreeGenerator
{
        public:
                DisplayTreeNode * generateTree(SgNode * sgRoot, AstFilterInterface * filter=NULL);

        protected:
                void visit(DisplayTreeNode * parent, SgNode * sgNode, AstFilterInterface * filter);

                DisplayTreeNode * treeRoot;
};


#endif
