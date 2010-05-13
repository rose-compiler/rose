#include "sage3basic.h"
#include "AsmUnparser_compat.h"
#include "ItemTreeNode.h"
#include "AsmTreeNode.h"

#include "AstFilters.h"
#include "ItemModelHelper.h"

#include <QApplication>
#include <QPalette>
#include <QFont>
#include <QLinearGradient>


#include <QDebug>

// ---------------------- Root Node ------------------------------


AsmTreeRootNode::AsmTreeRootNode(bool d)
    : diff(d)
{

}

QStringList AsmTreeRootNode::sectionHeader() const
{
    static QStringList diff_list =
        QStringList() << QObject::tr("Address 1")
                      << QObject::tr("Inst 1")
                      << QObject::tr("Args 1")
                      << QObject::tr("comment 1")
                      << ""
                      << QObject::tr("Address 2")
                      << QObject::tr("Inst 2")
                      << QObject::tr("Args 2")
                      << QObject::tr("comment 2");

   static QStringList normal_list =
       QStringList() << QObject::tr("Address")
                     << QObject::tr("Inst")
                     << QObject::tr("Args")
                     << QObject::tr("comment");

    return diff ? diff_list : normal_list;
}


// -------------------- Function Node ------------------------------


AsmTreeFunctionNode::AsmTreeFunctionNode(SgAsmFunctionDeclaration * fd1,
                                         SgAsmFunctionDeclaration * fd2)
    : declNode1(fd1), declNode2(fd2)
{
    Q_ASSERT(declNode1 || declNode2 );

    if(declNode1 && declNode2)
        Q_ASSERT(fd1->get_name() == fd2->get_name()); //the function names have to match

    SgAsmFunctionDeclaration *valNode = declNode1 ? declNode1 : declNode2;

    name=StringUtility::demangledName(valNode->get_name()).c_str();
}


QVariant AsmTreeFunctionNode::data(int role,int column) const
{
    //Function name in column 0, rest empty
    if(role == Qt::DisplayRole && column ==0)
        return name;

    if(role == Qt::BackgroundColorRole)
        return Qt::black;

    if(role == Qt::ForegroundRole)
        return Qt::white;


    if(role == SgNodeRole)
    {
        AsmTreeRootNode * root = dynamic_cast<AsmTreeRootNode*> (getParent());
        Q_ASSERT(root);
        int columnCount = root->sectionHeader().size();
        if(root->isDiff())
            columnCount = (columnCount-1)/2;


        if(column<columnCount)
            return QVariant::fromValue<SgNode*>(declNode1);
        else
            return QVariant::fromValue<SgNode*>(declNode2);
    }

    if(role == Qt::FontRole)
    {
        QFont f;
        f.setBold(true);
        f.setPointSize(f.pointSize()+1);
        return f;
    }

    return QVariant();
}




// -------------------- Instruction Node ------------------------------



AsmTreeInstructionNode::AsmTreeInstructionNode(SgAsmInstruction * inst1, SgAsmInstruction * inst2)
    : instNode1(inst1), instNode2(inst2)
{
    SgAsmInstruction * valInst = instNode1;
    if(!valInst)
        valInst = instNode2;

    Q_ASSERT(valInst); //at least one of the instructions has to be non-null


    if(instNode1 && instNode2)
    {
        Q_ASSERT(instNode1->get_mnemonic() == instNode2->get_mnemonic() );
        //should test argument for equality too
    }

    mnemonic = valInst->get_mnemonic().c_str();

    SgAsmOperandList * ops = valInst->get_operandList();
    SgAsmExpressionPtrList& opsList = ops->get_operands();

    operands=" ";
    for (SgAsmExpressionPtrList::iterator it = opsList.begin(); it!=opsList.end();++it)
    {
        operands.append(unparseExpression(*it).c_str());
        if(it != opsList.end() -1)
            operands.append(", ");
    }


    comment = valInst->get_comment().c_str();


    SgUnsignedCharList rawByteList = valInst->get_raw_bytes();

    for(SgUnsignedCharList::iterator i = rawByteList .begin();
        i != rawByteList.end(); ++i)
    {
        rawBytes.append( QString("0x%1 ").arg(*i,0,16));
    }

    address = valInst->get_address();
}



QVariant AsmTreeInstructionNode::data(int role,int column) const
{
    //the separator column when diff is used
    if(role== Qt::BackgroundRole && column ==4)
        return QBrush(QColor(Qt::red));

    AsmTreeRootNode * rootNode = dynamic_cast<AsmTreeRootNode*>(getParent()->getParent());
    Q_ASSERT(rootNode);

    //subtract divider column, then half
    int normalColumnCount = (rootNode->sectionHeader().size() -1) /2;

    bool leftPart = true;

    //divider column belongs to left part
    if(rootNode->isDiff() && column > normalColumnCount)
        leftPart=false;

    // Left Part
    if( leftPart && !instNode1)
        return QVariant();


    if(!leftPart)
    {
        if(!instNode2)
            return QVariant();

        Q_ASSERT(column!=4);
        column = (column-1)-normalColumnCount;
    }

    if(leftPart && instNode1 || !leftPart && instNode2)
    {
        //Function name in column 0, rest empty
        if(role == Qt::DisplayRole)
        {
            switch(column)
            {
                case 0:  return QString("%1").arg(address,0,16);
                case 1:  return mnemonic;
                case 2:  return operands;
                //case 2:  return rawBytes;
                case 3:  return comment;
                default: return QVariant();
            }
        }
    }

    if( role == Qt::ForegroundRole )
    {
        switch(column)
        {
            //case 0 : return QColor(Qt::red);              //red addresses
            case 1 : return QColor(Qt::blue);               //blue mnemonics
            case 2 : return QColor(Qt::green).darker(150);  //green operands
            case 3 : return QColor(Qt::gray);               //gray comments
            default: return QVariant();
        }
    }


    if(role == SgNodeRole)
        return QVariant::fromValue<SgNode*>(leftPart ? instNode1 : instNode2);

    return QVariant();
}


// -------------------- Functions to build up the Asm Tree ------------------------



void buildAsmTreeVisit(SgNode * sgNode, ItemTreeNode * itemNode,AstFilterInterface * filter)
{
    if(sgNode == NULL)
        return;

    if(filter && !filter->displayNode(sgNode))
        return;


    ItemTreeNode * newNode = itemNode;

    SgAsmFunctionDeclaration * sgFunc = isSgAsmFunctionDeclaration(sgNode);
    SgAsmInstruction * sgInst = isSgAsmInstruction(sgNode);

    if(sgInst) // Instruction
    {
        newNode = new AsmTreeInstructionNode(sgInst);
        itemNode->addChild(newNode);
    }
    else if (sgFunc) // Function Declaration
    {
        newNode = new AsmTreeFunctionNode(sgFunc);
        itemNode->addChild(newNode);
    }


    // Traverse children
    for(unsigned int i=0; i< sgNode->get_numberOfTraversalSuccessors(); i++)
        buildAsmTreeVisit(sgNode->get_traversalSuccessorByIndex(i),newNode,filter);

}




ItemTreeNode * buildAsmTree(SgNode * node, AstFilterInterface * filter)
{
    // Check for valid node
    if(node == NULL || node->get_numberOfTraversalSuccessors() == 0)
        return NULL;


    ItemTreeNode * rootNode = new AsmTreeRootNode();
    buildAsmTreeVisit(node,rootNode,filter);

    return rootNode;
}



#include "LCS.h"

using namespace std;


Rose_STL_Container<SgNode *>  querySubTreeFiltered(SgNode * tree, VariantT type,AstFilterInterface * filter )
{
    if(tree==NULL)
        return Rose_STL_Container<SgNode*> ();

    if(filter==NULL)
        return NodeQuery::querySubTree(tree,type);

    //TODO write own traversal -> faster
    Rose_STL_Container<SgNode *> all = NodeQuery::querySubTree(tree,type);

    Rose_STL_Container<SgNode *> filtered;

    typedef Rose_STL_Container<SgNode*>::iterator RContainterIter;
    for(RContainterIter i= all.begin(); i != all.end(); ++i)
        if(filter->displayNode(*i))
            filtered.push_back(*i);

    return filtered;

}


ItemTreeNode * buildAsmTreeDiff(SgNode * node1, SgNode * node2,
                                AstFilterInterface * filter1, AstFilterInterface * filter2)
{
    Rose_STL_Container<SgNode *> funcs1 = querySubTreeFiltered(node1,V_SgAsmFunctionDeclaration,filter1);
    Rose_STL_Container<SgNode *> funcs2 = querySubTreeFiltered(node2,V_SgAsmFunctionDeclaration,filter2);


    ItemTreeNode * rootNode = new AsmTreeRootNode(true);

    vector<pair<SgNode*,SgNode*> > funcDiff;
    LCS::getDiff(funcs1,funcs2,funcDiff);

    typedef vector<pair<SgNode*,SgNode*> >::iterator SgPairIter;

    for(SgPairIter i= funcDiff.begin(); i != funcDiff.end(); ++i)
    {
        SgAsmFunctionDeclaration * leftFunc  = isSgAsmFunctionDeclaration(i->first);
        SgAsmFunctionDeclaration * rightFunc = isSgAsmFunctionDeclaration(i->second);

        AsmTreeFunctionNode * funcNode = new AsmTreeFunctionNode(leftFunc,rightFunc);
        rootNode->addChild(funcNode);

        Rose_STL_Container<SgNode *> inst1 = querySubTreeFiltered(leftFunc, V_SgAsmInstruction,filter1);
        Rose_STL_Container<SgNode *> inst2 = querySubTreeFiltered(rightFunc,V_SgAsmInstruction,filter2);

        vector<pair<SgNode*,SgNode*> > instDiff;
        LCS::getDiff(inst1,inst2,instDiff);

        for(SgPairIter j = instDiff.begin(); j != instDiff.end(); ++j)
        {
            SgAsmInstruction * inst1 = isSgAsmInstruction(j->first);
            SgAsmInstruction * inst2 = isSgAsmInstruction(j->second);
            AsmTreeInstructionNode * instNode = new AsmTreeInstructionNode(inst1,inst2);

            funcNode->addChild(instNode);
        }

    }

    return rootNode;
}














