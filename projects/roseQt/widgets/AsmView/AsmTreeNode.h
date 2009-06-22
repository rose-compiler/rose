
#ifndef ASMTREENODE_H
#define ASMTREENODE_H


#include "ItemTreeNode.h"
#include "AstDisplayInfo.h"

#include <QIcon>

class SgNode;
class SgAsmFunctionDeclaration;
class SgAsmInstruction;

class AstFilterInterface;


class AsmTreeRootNode : public ItemTreeNode
{
    public:
        /// Creates an (invisible) root node, which provides the header information
        /// @param diffModel true when nodes should store two AsmFiles (diff mode)
        AsmTreeRootNode(bool diffModel=false);

        bool isDiff() { return diff; }

        virtual QStringList sectionHeader() const;
    protected:
        bool diff;

};

class AsmTreeFunctionNode : public ItemTreeNode
{
    public:
        AsmTreeFunctionNode(SgAsmFunctionDeclaration * fd,
                            SgAsmFunctionDeclaration * fd2=0);
        virtual ~AsmTreeFunctionNode() {}

        QString getName() const { return name; }

        virtual QVariant data(int role, int column=0) const;

        bool isFirstColumnSpanned () const    { return true; }

    protected:
        SgAsmFunctionDeclaration * declNode1;
        SgAsmFunctionDeclaration * declNode2;
        QString name;

};

class AsmTreeInstructionNode : public ItemTreeNode
{
    public:
        AsmTreeInstructionNode(SgAsmInstruction * inst1,
                               SgAsmInstruction * inst2 = NULL);

        virtual ~AsmTreeInstructionNode() {}

        QString getUnparsedLine() const { return operands; }
        long getAddress()         const { return address;      }
        QString getRawBytes()     const { return rawBytes;     }
        QString getComment()      const { return comment;      }

        void setComment(const QString & c) { comment = c; }

        virtual QVariant data(int role, int column=0) const;

    protected:
        //QVariant dataSingleInstr(int role, int column, SgAsmInstruction * instNode) const;

        SgAsmInstruction * instNode1;
        SgAsmInstruction * instNode2;

        QString mnemonic;
        QString operands;

        QString comment;
        QString rawBytes;
        long    address;
};


class AstFilterInterface;

ItemTreeNode *  buildAsmTree(SgNode * node, AstFilterInterface * filter);

ItemTreeNode * buildAsmTreeDiff(SgNode * node1, SgNode * node2,
                                AstFilterInterface * filter1, AstFilterInterface * filter2);

#endif
