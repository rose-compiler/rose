#ifndef BASTNODE_H
#define BASTNODE_H

#include "util/ItemTreeNode.h"
#include "util/AstDisplayInfo.h"

#include <QIcon>

class SgNode;
class AstFilterInterface;


class BAstNode : public ItemTreeNode
{

    public:



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
        const QString & getCaption() const     { return caption; }
        const QString & getTooltip() const     { return tooltip; }


        QIcon getIcon() const                  { return icon; }

        AstDisplayInfo::NodeType getType() const { return type; }


        SgNode * sgNode() const { return sg; }


        /// Overwritten Data for returning Item-Data
        virtual QVariant data(int role, int column) const;


        static BAstNode * generate(SgNode * sgRoot,AstFilterInterface * filter);

    protected:
        static void generationVisit(SgNode * node,
                                    BAstNode * bAstNode,
                                    int fileId,
                                    AstFilterInterface * filter);

        /// Sorts the children of a node by type
        /// first files, than namespaces ...
        void sortByType();

        /// removes double namespace entries
        /// if two adjacent children are namespaces and have same name
        /// delete the second (call sortByType first!)
        void mergeNamespaces();

        static bool lessThanByType   (ItemTreeNode * n1, ItemTreeNode * n2);
        static bool lessThanByCaption(ItemTreeNode * n1, ItemTreeNode * n2);

        AstDisplayInfo::NodeType type;
        QString caption;
        QString tooltip;
        SgNode * sg;
        QIcon icon;
};





#endif
