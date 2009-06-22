#include "TreeLayoutGenerator.h"
#include <QDebug>
#include <cmath>
#include <limits>

// For each node additional Layout info is needed
// and stored as a hash indexed by node-pointer
struct TreeLayoutGenerator::AdditionalNodeInfo
{
        AdditionalNodeInfo() :
            threadPointer(NULL), threadOffset(0), relPosition(0)
        {
        }

        /// Pointer to threaded node, or NULL for no thread
        DisplayTreeNode * threadPointer;

        /// the distance between threadedNode and this node
        double threadOffset;

        /// relative position of this node
        double relPosition;
};

/// Structure is used to keep track of "extreme" nodes on lowest level of subtrees
/// extreme means left or rightmost in current subtree
class TreeLayoutGenerator::ExtremeInfo
{
    public:
        ExtremeInfo(DisplayTreeNode * node_, double offset_, int layer_) :
            node(node_),
            offset(offset_),
            layer(layer_)
        {}



        ExtremeInfo(int layer_,DisplayTreeNode * leaf) :
            node(leaf),
            offset(0),
            layer(layer_)
        {
        }

        ExtremeInfo(const ExtremeInfo & o) :
            node(o.node),
            offset(o.offset),
            layer(o.layer)
        {}


        void incrOffset(double incr)   { offset += incr; }
        void decrOffset(double decr)   { offset -= decr; }

        double getOffset()      const   { return offset; }
        DisplayTreeNode * getNode() const   { return node;}

        /// Compares this and other, the result is stored in this
        void merge(const ExtremeInfo & other, bool left)
        {
            if(other.layer < layer)
                return;

            double myOffset   = offset + node->boundingRect().width()/2;
            double otherOffset= other.offset + other.node->boundingRect().width()/2;

            bool replace=false;
            if(left && otherOffset< myOffset)
                replace=true;
            else if(!left && otherOffset>myOffset)
                replace=true;

            if(replace)
                *this=other;
        }

    protected:

        ExtremeInfo() :
            node(NULL),
            offset(0),
            layer(-1)
        {}

        /// Link to the "extreme" node
        DisplayTreeNode * node;

        /// Offset of the node in current subtree
        double offset;

        /// Layer of this node, needed for merging ExtremeInfos
        /// because the Node has to be on the lowest subtree (i.e. where layer is maximal)
        int layer;
};


TreeLayoutGenerator::TreeLayoutGenerator() :
    minNodeDistance(15),
    extraDistSubtrees(0),
    layerHeight(60),
    layerChildFactor(0)
{
}

void TreeLayoutGenerator::layoutTree(DisplayTreeNode * root)
{
    Q_ASSERT(nodeInfo.size()==0);
    qDebug() << "PosCalc Traversal";
    posCalcTraversal(root, 1);
    qDebug() << "Layout Traversal";
    layoutTraversal(root);
    //qDebug() << "Paint Thread Traversal";
    //paintThreadTraversal(root);

    qDebug() << "Deleting additional info";
    foreach (AdditionalNodeInfo * ni, nodeInfo)
        delete ni;

    nodeInfo.clear();

    qDebug() << "Done";

}

TreeLayoutGenerator::ExtInfoPair TreeLayoutGenerator::posCalcTraversal(DisplayTreeNode * node, int layer)
{
    // base case for leafs
    if (node->childrenCount() == 0)
    {
        nodeInfo.insert(node, new AdditionalNodeInfo());
        // this node is both, the left- and rightmost node
        return qMakePair(ExtremeInfo(layer,node),ExtremeInfo(layer,node));
    }

    // Visit children (postorder traversal)
    QList<ExtremeInfo> extInfoListLeft;
    QList<ExtremeInfo> extInfoListRight;


    for(int i=0; i < node->childrenCount(); i++)
    {
        DisplayTreeNode * subNode = node->getChild(i);
        ExtInfoPair ret= posCalcTraversal(subNode,layer+1);
        extInfoListLeft.push_back(ret.first);
        extInfoListRight.push_back(ret.second);
    }

    // merge subtrees
    AdditionalNodeInfo * curNodeInfo = new AdditionalNodeInfo();
    nodeInfo[node] = curNodeInfo;

    Q_ASSERT(curNodeInfo!=NULL);

    double offsetSum = 0;

    //TODO don't copy here, use new interface
    QList<DisplayTreeNode*> childList;
    for(int i=0; i< node->childrenCount(); i++)
        childList.push_back(node->getChild(i));


    // The first is positioned at relPos=0, the second at previosPos+needeDistance
    // after positioning all children, they are repositioned such that parent is centered
    nodeInfo[childList[0]]->relPosition = 0;
    //qDebug() << "Processing" << node->getDisplayName();

    // Merging the children
    // the ExtremeNodeInfo structs have to be updated after every merge
    // and the merging result is stored at index 0 in the list
    // caution: if you merge child 2 and 3, the leftmost node may be in child 0 or 1
    for (int i = 1; i < childList.size(); i++)
    {

        double dist = calcSubtreeDistance(childList[i-1], childList[i],
                                          extInfoListLeft[0],extInfoListRight[i]);
        offsetSum += dist;
        nodeInfo[childList[i]]->relPosition = offsetSum;
        extInfoListLeft[i].incrOffset(offsetSum);
        extInfoListRight[i].incrOffset(offsetSum);

        extInfoListLeft[0].merge(extInfoListLeft[i],true);
        //extInfoListRight[0].merge(extInfoListRight[i],false);
    }




    // Center the children below parent,
    double center = offsetSum / 2;
    for (int i = 0; i < childList.size(); i++)
    {
        nodeInfo[childList[i]]->relPosition -= center;
    }
    extInfoListLeft[0].decrOffset (center);
    extInfoListRight[0].decrOffset(center);


    for (int i = 1; i < childList.size(); i++)
    {
        extInfoListRight[0].merge(extInfoListRight[i],false);
    }

    return qMakePair(extInfoListLeft[0],extInfoListRight[0]);
}

DisplayTreeNode * TreeLayoutGenerator::advanceLeft(DisplayTreeNode * left, double & leftPos,
                                               bool & threadUsed)
{
    DisplayTreeNode * newLeft;

    if (left->childrenCount() == 0)
    {
        Q_ASSERT(nodeInfo.contains(left));
        newLeft = nodeInfo[left]->threadPointer;
        if (newLeft)
        {
            threadUsed = true;
            leftPos += nodeInfo[left]->threadOffset;
        }
    }
    else
    {
        newLeft = left->getLastChild();
        leftPos += nodeInfo[newLeft]->relPosition;
    }

    return newLeft;
}

DisplayTreeNode * TreeLayoutGenerator::advanceRight(DisplayTreeNode * right, double & rightPos,
                                                bool & threadUsed)
{
    DisplayTreeNode * newRight;

    if (right->childrenCount() == 0)
    {
        Q_ASSERT(nodeInfo.contains(right));

        newRight = nodeInfo[right]->threadPointer;
        if (newRight)
        {
            threadUsed = true;
            rightPos += nodeInfo[right]->threadOffset;
        }
    }
    else
    {
        newRight = right->getFirstChild();
        rightPos += nodeInfo[newRight]->relPosition;
    }
    return newRight;
}

double TreeLayoutGenerator::calcSubtreeDistance(DisplayTreeNode * left, DisplayTreeNode * right,
                                                const ExtremeInfo & extInfoL,
                                                const ExtremeInfo & extInfoR)
{
    double curOffset = minNodeDistance;

    double leftPos = 0;
    double rightPos = 0;

    bool threadUsed = false;
    bool firstRun = true;
    //qDebug() << "Merging" << left->getDisplayName() << "and" << right->getDisplayName();

    //scan along the outline of the two subtrees and compute how far they have
    //to be pushed apart
    while (left != NULL && right != NULL)
    {
        Q_ASSERT(nodeInfo.contains(left));
        Q_ASSERT(nodeInfo.contains(right));

        //Adjust the distance if necessary
        double neededDistance = minNodeDistance;
        neededDistance += left->boundingRect().width() / 2;
        neededDistance += right->boundingRect().width() / 2;
        if (threadUsed)
            neededDistance += extraDistSubtrees;

        double curDist = rightPos - leftPos + curOffset;
        if (curDist < neededDistance)
            curOffset += neededDistance - curDist;

        //go one layer deeper
        DisplayTreeNode * newLeft = advanceLeft(left, leftPos, threadUsed);
        DisplayTreeNode * newRight = advanceRight(right, rightPos, threadUsed);


#if 1
        //check if threading is required
        if (newLeft && !newRight)
        {
            // Thread from rightmost node of right subtree to the newLeft
            // i.e. to the leftmost node on next layer of left tree
            nodeInfo[extInfoR.getNode()]->threadPointer = newLeft;
            nodeInfo[extInfoR.getNode()]->threadOffset =  -(extInfoR.getOffset() + curOffset - leftPos);
        }
        if (!newLeft && newRight)
        {
             nodeInfo[extInfoL.getNode()]->threadPointer = newRight;
             nodeInfo[extInfoL.getNode()]->threadOffset = rightPos + curOffset - extInfoL.getOffset() ;
        }


#else
        if (newLeft && !newRight)
        {
            DisplayTreeNode * n = NULL;
            if (!firstRun)
            {
                n = right->getParent()->getChildren().last();
                rightPos -= nodeInfo[right]->relPosition;
                rightPos += nodeInfo[n]->relPosition;
            }
            else
            {
                n = right;
            }

            nodeInfo[n]->threadPointer = newLeft;
            nodeInfo[n]->threadOffset = -(rightPos + curOffset - leftPos);
        }
        if (!newLeft && newRight)
        {
            DisplayTreeNode * n = NULL;
            if (!firstRun)
            {
                n = left->getParent()->getChildren().first();
                leftPos -= nodeInfo[left]->relPosition;
                leftPos += nodeInfo[n]->relPosition;
            }
            else
                n = left;

            nodeInfo[n]->threadPointer = newRight;
            nodeInfo[n]->threadOffset = rightPos + curOffset - leftPos;
        }
#endif

        left = newLeft;
        right = newRight;
        firstRun=false;
    }
    return curOffset;
}

void TreeLayoutGenerator::layoutTraversal(DisplayTreeNode * node)
{
    DisplayTreeNode * parent = node->getParent();
    QPointF parPos = parent ? parent->pos() : QPointF();

    // if parent has many children, the layer is made higher (more space for lines)
    double addLayerOffset = parent ? parent->childrenCount() : 0;

    parPos.setY(parPos.y() + layerHeight + layerChildFactor * addLayerOffset);
    parPos.setX(parPos.x() + nodeInfo[node]->relPosition);
    node->setPos(parPos);


    for(int i=0; i < node->childrenCount(); i++)
        layoutTraversal(node->getChild(i));
}

#include <QPen>
#include "DisplayEdge.h"

void TreeLayoutGenerator::paintThreadTraversal(DisplayTreeNode * node)
{
    //Paint threads for debugging

    DisplayTreeNode * from = node;
    DisplayTreeNode * to = nodeInfo[node]->threadPointer;

    if (to != NULL)
    {
        Q_ASSERT(from!=NULL);
        DisplayEdge * e = new DisplayEdge(from, to);
        e->setWidth(1);
        e->setColor(Qt::red);
        e->setEdgeLabel(QString("%1").arg(nodeInfo[node]->threadOffset));
        e->setPaintMode(DisplayEdge::STRAIGHT);
        from->registerAdditionalEdge(e);
        to->registerAdditionalEdge(e);
    }

    for(int i=0; i < node->childrenCount(); i++)
        paintThreadTraversal(node->getChild(i));
}

