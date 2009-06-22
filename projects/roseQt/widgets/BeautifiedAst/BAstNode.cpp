#include <rose.h>
#include "BAstNode.h"
#include "ItemModelHelper.h"
#include <QFileInfo>
#include <QtAlgorithms>

#include <QDebug>

#include "AstFilters.h"


using namespace AstDisplayInfo;

BAstNode::BAstNode(AstDisplayInfo::NodeType t,
                   const QString & cap,
                   const QString & tt,
                   const QIcon & ico,
                   SgNode * sgNode) :
    type(t),
    caption(cap),
    tooltip(tt),
    sg(sgNode),
    icon(ico)
{
}

BAstNode::BAstNode(AstDisplayInfo::NodeType t,
                   const QString & cap,
                   SgNode * sgNode) :
    type(t),
    caption(cap),
    tooltip(""),
    sg(sgNode),
    icon(QIcon())
{
}


BAstNode::BAstNode(SgNode * sgNode)
    : sg(sgNode)
{
    type    = AstDisplayInfo::getType(sg);
    caption = AstDisplayInfo::getShortNodeNameDesc(sg);
    tooltip = AstDisplayInfo::getShortNodeTypeDesc(sg);
    icon    = AstDisplayInfo::nodeIcon(sg);
}


BAstNode::~BAstNode()
{
}


QStringList BAstNode::sectionHeader() const
{
    return QStringList() << QObject::tr("Element");
}

QVariant BAstNode::data(int role, int column) const
{
    if(role == Qt::DisplayRole)
        return caption;
    else if(role == Qt::DecorationRole)
        return icon;
    else if(role == Qt::ToolTipRole)
        return tooltip;
    else if(role == SgNodeRole)
        return QVariant::fromValue(sg);
    else
        return QVariant();
}


ItemTreeNode * BAstNode::generate(SgNode * node,AstFilterInterface * filter, ItemTreeNode * root)
{
    if(node==NULL)
        return NULL;

    if(root == NULL)
        root = new BAstNode(ROOT,QObject::tr("Root"),node);

    SgProject * project = isSgProject(node);
    SgFile * file = isSgFile(node);


    if(file)
    {
        Sg_File_Info * fi =file->get_file_info();
        int mainFileId (fi->get_file_id());

        std::map< int, std::string > map = fi->get_fileidtoname_map();

        if( isSgBinaryFile(file) )//binary ast
        {
            generationVisit(file,root,-1,filter);
        }
        else if (isSgSourceFile(file) )
        {
            BAstNode * includeFileTopNode = new BAstNode(INCLUDE_ROOT,
                                                         QObject::tr("Include Files"),
                                                         QObject::tr("All included Files"),
                                                         QIcon(":/util/NodeIcons/includes.gif"),
                                                         node);

            root->addChild(includeFileTopNode);

            typedef std::map<int,std::string>::iterator MapIter;
            for( MapIter iter = map.begin(); iter != map.end(); ++iter )
            {
                if(iter->first == mainFileId)
                    continue;

                QFileInfo qFi(iter->second.c_str());
                if(qFi.fileName().isEmpty())
                    continue;

                BAstNode * inclFileNode = new BAstNode(INCLUDE_FILE,
                                                       qFi.fileName(),
                                                       QObject::tr("Included file"),
                                                       QIcon(":/util/NodeIcons/include.gif"),
                                                       NULL);
                includeFileTopNode->addChild(inclFileNode);

                generationVisit(file,inclFileNode,iter->first,filter);
            }
            generationVisit(file,root,mainFileId,filter);
        }
    }
    else if(project)
    {
        for(int i=0; i < project->numberOfFiles(); i++)
        {
            Sg_File_Info * fi = (*project)[i]->get_file_info();

            std::map< int, std::string > map = fi->get_fileidtoname_map();

            QFileInfo qFi;
            qFi.setFile(fi->get_filenameString().c_str());


            BAstNode * mainFileNode = new BAstNode(SRC_FILE,
                                                   qFi.fileName(),
                                                   QObject::tr("Compiled Source-file"),
                                                   QIcon(":/util/NodeIcons/sourcefile.gif"),
                                                   node);
            root->addChild(mainFileNode);

            generate(&project->get_file(i),filter, mainFileNode);
        }

    }
    else
    {
        generationVisit(node,root,-1,filter);
    }

    sortByType(root);
    mergeNamespaces(root);
    return root;
}


void BAstNode::generationVisit(SgNode * node, ItemTreeNode * bAstNode,
                               int fileId,    AstFilterInterface * filter)
{
    if(node==NULL)
        return;
    if(filter && ! filter->displayNode(node) )
        return;

    ItemTreeNode * newBAstNode = bAstNode;

    SgLocatedNode* sgLocNode = isSgLocatedNode(node);

    // fileId has to match, or, if fileid==-1 accept all nodes
    if( (fileId == -1 ||  (sgLocNode && sgLocNode->get_file_info()->get_file_id() == fileId ))  &&
        AstDisplayInfo::getType(node) != AstDisplayInfo::UNKNOWN )
    {
        if(! AstDisplayInfo::getShortNodeNameDesc(node).isEmpty())
        {
            newBAstNode = new BAstNode(node);
            bAstNode->addChild(newBAstNode);
        }
    }

    for(unsigned int i=0; i< node->get_numberOfTraversalSuccessors(); i++)
        generationVisit(node->get_traversalSuccessorByIndex(i),newBAstNode,fileId,filter);

}

bool BAstNode::lessThanByType(ItemTreeNode * n1, ItemTreeNode * n2)
{
    BAstNode * bn1 = dynamic_cast<BAstNode*>(n1);
    BAstNode * bn2 = dynamic_cast<BAstNode*>(n2);
    Q_ASSERT( bn1 && bn2); //tried to sort a subtree which doesn't consist of BAstNodes

    return static_cast<int>(bn1->getType()) <  static_cast<int>(bn2->getType());
}
bool BAstNode::lessThanByCaption(ItemTreeNode * n1, ItemTreeNode * n2)
{
    BAstNode * bn1 = dynamic_cast<BAstNode*>(n1);
    BAstNode * bn2 = dynamic_cast<BAstNode*>(n2);
    Q_ASSERT( bn1 && bn2); //tried to sort a subtree which doesn't consist of BAstNodes

    return bn1->caption  <  bn2->caption;
}


void BAstNode::sortByType(ItemTreeNode * node)
{
    node->sortChildren(BAstNode::lessThanByCaption );
    node->sortChildrenStable(BAstNode::lessThanByType );

    for(int i=0; i < node->childrenCount(); i++)
        sortByType(node->child(i));
}

void BAstNode::mergeNamespaces(ItemTreeNode * root)
{
    if(root->childrenCount() == 0)
        return;

    QList<ItemTreeNode*> oldChildList;
    root->takeAllChildren(oldChildList);

    QList<ItemTreeNode*> newChildList;

    newChildList.push_back(oldChildList[0]);

    for(int i=1; i < oldChildList.size() ; i++)
    {
        BAstNode * lastNode = dynamic_cast<BAstNode*>(newChildList.back());
        BAstNode * curNode  = dynamic_cast<BAstNode*>(oldChildList[i]);

        if(!lastNode || !curNode)
        {
            qWarning() << "Tried to call mergeNamespaces on a subtree which does not consist of BAstNodes";
            return;
        }

        if(curNode->getType() == NAMESPACE &&
           lastNode->getType() == NAMESPACE &&
           curNode->caption == lastNode->caption )
        {
            // take all children from current node, move them to lastNode
            // then delete curNode
            QList<ItemTreeNode* > curNodeChildren;
            curNode->takeAllChildren(curNodeChildren);
            lastNode->addChildren(curNodeChildren);

            // can be safely deleted, because the children have been detached from root
            // by takeAllChildren
            delete curNode;
        }
        else
        {
            newChildList.push_back(curNode);
        }
    }

    root->addChildren(newChildList);

    for(int i=1; i < root->childrenCount(); i++)
        mergeNamespaces( root->child(i));

}





