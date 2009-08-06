#ifndef BASTNODE_H
#define BASTNODE_H

#include "ItemTreeNode.h"
#include "AstDisplayInfo.h"

#include <QIcon>

class SgNode;
class AstFilterInterface;

/**
 * \brief A treenode of a Beautified AST
 *
 *  This class represents the display information of a tree node in a Beautified AST
 *  A beautified AST shows only function, classes, namespace etc.
 *   the "beautified" ast's are ment to shown to a user which is not familiar with ROSE datastructures
 */
class BAstNode : public ItemTreeNode
{

    public:
        /// Generates a new tree
        static ItemTreeNode * generate(SgNode * sgRoot,AstFilterInterface * filter, ItemTreeNode * root = NULL);



        BAstNode(AstDisplayInfo::NodeType type,
                 const QString & caption,
                 const QString & tooltip,
                 const QIcon & icon,
                 SgNode * sg);

        BAstNode(AstDisplayInfo::NodeType type,
                 const QString & caption,
                 SgNode * sg);

        BAstNode(SgNode * node);

        virtual ~BAstNode();

        void setCaption(const QString & cap)   { caption=cap; }

        AstDisplayInfo::NodeType getType() const { return type; }


        SgNode * sgNode() const { return sg; }


        /// Overwritten Data for returning Item-Data
        virtual QVariant data(int role, int column) const;


        virtual QStringList sectionHeader() const;

    protected:
        static void generationVisit(SgNode * node,
                                    ItemTreeNode * bAstNode,
                                    int fileId,
                                    AstFilterInterface * filter);

        /// Sorts the children of a node by type
        /// first files, than namespaces ...
        static void sortByType(ItemTreeNode * node);

        /// removes double namespace entries
        /// if two adjacent children are namespaces and have same name
        /// delete the second (call sortByType first!)
        static void mergeNamespaces(ItemTreeNode * root);

        static bool lessThanByType   (ItemTreeNode * n1, ItemTreeNode * n2);
        static bool lessThanByCaption(ItemTreeNode * n1, ItemTreeNode * n2);

        AstDisplayInfo::NodeType type;
        QString caption;
        QString tooltip;
        SgNode * sg;
        QIcon icon;

    private:
        // returns the function declaration of the sgNode
        // if sgNode is a SgFunctionDefinition
        // otherwise returns itself again
        SgNode *getFunctionDeclaration( SgNode *sgNode );
};

#endif
