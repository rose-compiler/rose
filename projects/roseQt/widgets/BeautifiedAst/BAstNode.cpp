#include <rose.h>
#include "BAstNode.h"
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
    icon(QIcon()),
    sg(sgNode)
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


QVariant BAstNode::data(int role, int column) const
{
    if(role == Qt::DisplayRole)
        return caption;
    else
        return QVariant();
}


BAstNode * BAstNode::generate(SgNode * node,AstFilterInterface * filter)
{
    BAstNode * root = new BAstNode(ROOT,QObject::tr("Root"),node);

    SgProject * project = isSgProject(node);



    if(project)
    {
        for(int i=0; i < project->numberOfFiles(); i++)
        {
            Sg_File_Info * fi = (*project)[i]->get_file_info();
            int mainFileId (fi->get_file_id());

            std::map< int, std::string > map = fi->get_fileidtoname_map();

            QFileInfo qFi;
            qFi.setFile(fi->get_filenameString().c_str());


            BAstNode * mainFileNode = new BAstNode(SRC_FILE,
                                                   qFi.fileName(),
                                                   QObject::tr("Compiled Source-file"),
                                                   QIcon(":/util/NodeIcons/sourcefile.gif"),
                                                   node);
            root->addChild(mainFileNode);


            if( isSgBinaryFile(project->get_traversalSuccessorByIndex(0))) //binary ast
            {
                generationVisit(project,mainFileNode,-1,filter);
            }
            else
            {
                BAstNode * includeFileTopNode = new BAstNode(INCLUDE_ROOT,
                                                             QObject::tr("Include Files"),
                                                             QObject::tr("All included Files"),
                                                             QIcon(":/util/NodeIcons/includes.gif"),
                                                             node);

                mainFileNode->addChild(includeFileTopNode);

                typedef std::map<int,std::string>::iterator MapIter;
                for( MapIter iter = map.begin(); iter != map.end(); ++iter )
                {
                    if(iter->first == mainFileId)
                        continue;

                    qFi.setFile(iter->second.c_str());
                    BAstNode * inclFileNode = new BAstNode(INCLUDE_FILE,
                                                           qFi.fileName(),
                                                           QObject::tr("Included file"),
                                                           QIcon(":/util/NodeIcons/include.gif"),
                                                           NULL);
                    includeFileTopNode->addChild(inclFileNode);

                    generationVisit(project,inclFileNode,iter->first,filter);
                }
                generationVisit(project,mainFileNode,mainFileId,filter);
            }
        }

    }
    else
    {
        generationVisit(node,root,-1,filter);
    }

    root->sortByType();
    root->mergeNamespaces();
    return root;
}


void BAstNode::generationVisit(SgNode * node, BAstNode * bAstNode,
                               int fileId,    AstFilterInterface * filter)
{
    if(node==NULL)
        return;
    if(filter && ! filter->displayNode(node) )
        return;

    BAstNode * newBAstNode=bAstNode;

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
    Q_ASSERT( bn1 && bn2);

    return static_cast<int>(bn1->getType()) <  static_cast<int>(bn2->getType());
}
bool BAstNode::lessThanByCaption(ItemTreeNode * n1, ItemTreeNode * n2)
{
    BAstNode * bn1 = dynamic_cast<BAstNode*>(n1);
    BAstNode * bn2 = dynamic_cast<BAstNode*>(n2);
    Q_ASSERT( bn1 && bn2);

    return bn1->caption  <  bn2->caption;
}



void BAstNode::mergeNamespaces()
{
    if(children.isEmpty())
        return;

    QList<ItemTreeNode*> newChildList;

    newChildList.push_back(children.first());

    for(int i=1; i<children.size(); i++)
    {
        BAstNode * lastNode = dynamic_cast<BAstNode*>(newChildList.back());
        BAstNode * curNode  = dynamic_cast<BAstNode*>(children[i]);

        if(curNode->getType() == NAMESPACE &&
           lastNode->getType() == NAMESPACE &&
           curNode->caption == lastNode->caption )
        {
            qDebug() << "Skipped NameSpace " << curNode->caption;

            QList<ItemTreeNode * > grandChildList = curNode->children;

            foreach(ItemTreeNode * itNode, grandChildList)
                dynamic_cast<BAstNode*>(itNode)->parent=lastNode;

            // append(QList) is supported since Qt4.5, so use foreach instead..
            // lastNode->children.append(grandChildList);
            foreach(ItemTreeNode* n,grandChildList)
                lastNode->children.append(n);
        }
        else
        {
            newChildList.push_back(curNode);
        }
    }

    children=newChildList;

    foreach(ItemTreeNode * node, children)
        dynamic_cast<BAstNode*>(node)->mergeNamespaces();

}

void BAstNode::sortByType()
{
    qSort(children.begin(),children.end(), BAstNode::lessThanByCaption );

    qStableSort(children.begin(),children.end(), BAstNode::lessThanByType );

    foreach(ItemTreeNode * node, children)
        dynamic_cast<BAstNode*>(node)->sortByType();
}




