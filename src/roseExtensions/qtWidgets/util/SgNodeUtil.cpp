
#include "rose.h"

#include "SgNodeUtil.h"
#include "AsmToSourceMapper.h"

#include <iostream>
#include <cassert>
#include <string>

using namespace std;

SgFile *getSgFile( SgNode *node )
{
    if( isSgProject( node ) )
    {
        return NULL;
    }

    // traverse up until we find a SgFile
    SgNode *parent( node );
    while( !isSgFile( parent ) && parent )
    {
        parent = parent->get_parent();
    }

    assert( parent );
    return isSgFile( parent );
}

bool isBinaryNode( SgNode *node )
{
    return isSgBinaryFile( getSgFile( node ) ) ? true : false;
}

bool isSourceNode( SgNode *node )
{
    return isSgSourceFile( getSgFile( node ) ) ? true : false;
}

template< typename LinkType >
static std::vector<SgNode *> getLinkedNodes( SgNode *node )
{
    SgNodeVector res;

    AstAttributeMechanism *attrMech( node->get_attributeMechanism() );
    if( !attrMech ) return res;

    // iterate over AstAttributes to find our link
    for( AstAttributeMechanism::iterator i( attrMech->begin() );
         i != attrMech->end();
         ++i )
    {
        LinkType *attr( dynamic_cast<LinkType *>( i->second ) );

        if( attr )
        {
            for( typename LinkType::iterator j( attr->begin() ); j != attr->end(); ++j )
            {
                res.push_back( j->first );
            }
        }
    }

    return res;
}

SgNodeVector getLinkedBinaryNodes( SgNode *node )
{
    return getLinkedNodes<AstBinaryNodeLink>( node );
}

SgNodeVector getLinkedSourceNodes( SgNode *node )
{
    return getLinkedNodes<AstSourceNodeLink>( node );
}

bool isAncestor( SgNode *parent, SgNode *child )
{
    if( !child || !parent ) return false;

    SgNode *tmp( child );
    while( ( tmp = tmp->get_parent() ) )
    {
        if( tmp == parent )
            return true;
    }

    return false;
}

void deleteFileFromProject(SgFile * file, SgProject * proj)
{
    if(!file)
        return;

    SgFilePtrList & fileList = proj->get_fileList();
    SgFilePtrList::iterator it =  find(fileList.begin(), fileList.end(),file);
    assert(it != fileList.end());

    proj->get_fileList().erase(it);

    delete file;
}


bool findNextInstruction(SgNode * node, rose_addr_t & out)
{
    SgAsmInstruction * inst = isSgAsmInstruction(node);
    if(!inst)
        return false;

    string mn = inst->get_mnemonic();
    if(mn != "jmp" &&
       mn != "call" &&
       mn != "je" &&
       mn != "jle" &&
       mn != "jge" &&
       mn != "jl")
    {
        return false;
    }

    // Fixed expression:
    SgAsmExpressionPtrList & expList = inst->get_operandList()->get_operands();

    if (expList.size() != 1)
        return false;

    SgAsmQuadWordValueExpression * val = isSgAsmQuadWordValueExpression(expList.front());
    if(!val)
        return false;

    out = val->get_value();
    return true;
}

SgProject * getProjectOf(SgNode * node)
{
    SgNode * curNode =node;

    SgProject * proj =0;
    while(! (proj=isSgProject(curNode)) && curNode )
        curNode=curNode->get_parent();


    return proj;
}




