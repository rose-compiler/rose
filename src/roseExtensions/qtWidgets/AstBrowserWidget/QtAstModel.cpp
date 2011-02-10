#include "sage3basic.h"

#include "QtAstModel.h"

#include <QDebug>
#include <QList>

#include "ItemTreeNode.h"
#include "ItemModelHelper.h"

#include "AstDisplayInfo.h"

// ------------------ ModelNode ------------------------

/**
 * \brief Node for a tree consisting of DisplayInformation of an Ast
 *
 *  Supports the on-demand building of the tree
 */
class QtAstModel::ModelNode : public ItemTreeNode
{
        public:
                //Constructor only for RootElement!
                ModelNode(SgNode * proj);
                virtual ~ModelNode() {}

                SgNode * sgNode() const                         { return sg; }
                const QString & getDispName() const { return dispName; }


                QtAstModel::ModelNode * addChild(const QString & name,SgNode * s);

                virtual QVariant data(int role, int column=0) const;

                /// returns if this node is buildup
                bool isBuildUp() const  { return buildUp; }

                /// Call this function when you added all childnodes
                void buildUpFinished()  { buildUp=true; }

                virtual void deleteChildren();

        protected:

                ModelNode() {}

                /// Icon which is displayed in the first column
                QIcon icon;

                /// Associated SgNode
                SgNode * sg;


                /* this member is shown in the first column of the view
                 * it usually is some description obtained by AstDisplayInfo,
                 * but if there is none available it's set to the successorName
                 * see SgNode function get_traversalSuccessorNamesContainer() */
                QString dispName;

                /// Sets the dispName, and icon according to type of SgNode,
                /// if no description is found in AstDisplayInfo dispName is not changed
                /// dispName is set to the successorName (out of ROSE traversal) as default
                void setInfo(SgNode * node);

                // the tree structure is build up "on demand" from the rose AST
                // in the row() function, the children are added and finalized is set to true
                // when the node is visited again, the children are not added anymore
                bool buildUp;
};

QtAstModel::ModelNode::ModelNode(SgNode * s)
{
        sg=s;


        if(s)
                setInfo(s);
        else
                dispName=tr("Empty");


        buildUp=false;
}


void QtAstModel::ModelNode::deleteChildren()
{
        qDeleteAll(children);
        buildUp=false;
        children.clear();
}

QVariant QtAstModel::ModelNode::data(int role, int column) const
{
    if( role== Qt::DisplayRole)
    {
        switch(column)
        {
            case 0 : return dispName;
            case 1 : return sg ? QString( sg->class_name().c_str() ) : QString("NULL");
            default: return QVariant();
        }
    }
    else if( role==SgNodeRole)
    {
        return sg ? QVariant::fromValue<SgNode*>(sg) : QVariant();
    }
    else if (role == Qt::DecorationRole && column ==0)
        return icon;


    return QVariant();
}



QtAstModel::ModelNode * QtAstModel::ModelNode::addChild(const QString & name,SgNode * s)
{
        // when the tree is buildUp then all children have already been added
        Q_ASSERT(!buildUp);

        ModelNode * newChild = new ModelNode();
        ItemTreeNode::addChild(newChild);

        newChild->sg=s;
        newChild->dispName=name;
        newChild->buildUp=false;

        newChild->setInfo(s);

        return newChild;
}

void QtAstModel::ModelNode::setInfo(SgNode * s)
{
    icon = AstDisplayInfo::nodeIcon(s);

    QString newName = AstDisplayInfo::getShortNodeNameDesc(s);
    if(! newName.isEmpty())
        dispName=newName;

}

// ------------------ ASTTreeModel ------------------------

QtAstModel::QtAstModel(SgNode * node, QObject * p)
        : QAbstractItemModel(p),filter(NULL)
{
        treeRoot = new ModelNode(node);
}

QtAstModel::~QtAstModel()
{
        delete treeRoot;
        if(filter)
                delete filter;
}


void QtAstModel::setFilter(AstFilterInterface * f)
{
        if(filter)
                delete filter;

        filter=f;

        treeRoot->deleteChildren();
        reset();
}

void QtAstModel::setNode(SgNode * node)
{
        delete treeRoot;
        treeRoot = new ModelNode(node);
        reset();
}

Qt::ItemFlags QtAstModel::flags (const QModelIndex & i) const
{
        if(!i.isValid())
                return 0;

        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QtAstModel::data  (const QModelIndex & ind, int role) const
{
    if(!ind.isValid())
        return QVariant();

    ModelNode * node = static_cast<ModelNode *>(ind.internalPointer());
    Q_ASSERT(node);

    return node->data(role,ind.column());
}

QVariant QtAstModel::headerData (int section, Qt::Orientation orientation, int role) const
{
        if(role == Qt::DisplayRole && orientation==Qt::Horizontal)
        {
                switch(section)
                {
                        case 0:  return QString("Title");
                        case 1:  return QString("SgClassName");
                        default: return QVariant();
                }
        }
        return QVariant();
}

int  QtAstModel::columnCount (const QModelIndex & par) const
{
        return 2;
}

int  QtAstModel::rowCount(const QModelIndex & par) const
{
        //Root of the tree, one row..
        if(! par.isValid())
                return 1;


        //Only the first column has children
        if(par.column()>=1)
                return 0;


        ModelNode * node = static_cast<ModelNode *>(par.internalPointer());
        Q_ASSERT(node != NULL);

        if (!node->isBuildUp())
        {
                // build up the tree
                if(node->sgNode() != NULL) //test if it's a NULL leaf -> no children to buildup
                {
                        std::vector< SgNode * >  successorPointer = node->sgNode()->get_traversalSuccessorContainer();
                        std::vector< std::string > successorNames = node->sgNode()->get_traversalSuccessorNamesContainer();

                        Q_ASSERT(successorPointer.size() == successorNames.size());
                        for(unsigned int i=0; i < successorPointer.size(); i++)
                        {
                                if(filter!=NULL)
                                {
                                        if(! filter->displayNode(successorPointer[i]))
                                                continue;

                                }

                                node->addChild(successorNames[i].c_str(),successorPointer[i]);
                        }
                }

                node->buildUpFinished();
        }

        return node->childrenCount();
}

QModelIndex QtAstModel::index ( int row, int column, const QModelIndex & par) const
{
        if(row<0 || column<0)
                return QModelIndex();

        if(row>= rowCount(par) || column>=columnCount(par))
                return QModelIndex();

        if(!par.isValid() )
        {
                Q_ASSERT(treeRoot!=NULL);
                return createIndex(row,column,treeRoot);
        }
        else
        {
                Q_ASSERT(par.internalPointer()!=NULL);

                ModelNode * parentNode = static_cast<ModelNode *>(par.internalPointer());
                Q_ASSERT(parentNode != NULL);


                Q_ASSERT(parentNode->isBuildUp());
                Q_ASSERT(parentNode->childrenCount() > row);

                return createIndex(row,column,parentNode->child(row));
        }
}


QModelIndex QtAstModel::parent ( const QModelIndex & ind ) const
{
        if(! ind.isValid())
                return QModelIndex();


        ModelNode * node = static_cast<ModelNode *>(ind.internalPointer());
        Q_ASSERT(node != NULL);

        ItemTreeNode * parentNode = node->getParent();
        if(parentNode==NULL)
        {
                Q_ASSERT(node==treeRoot);
                return QModelIndex();
        }

        return createIndex(parentNode->getRow(),0,parentNode);
}

SgNode * QtAstModel::getNodeFromIndex(const QModelIndex & ind)
{
        ModelNode * ret = static_cast<ModelNode *>(ind.internalPointer());
        Q_ASSERT(ret!=NULL);
        return ret->sgNode();
}

