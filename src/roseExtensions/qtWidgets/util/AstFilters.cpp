#include "rose.h"

#include "AstFilters.h"

bool AstFilterFileByName::displayNode(SgNode * node) const
{
    SgLocatedNode* sgLocNode = isSgLocatedNode(node);
    if(sgLocNode==NULL)
        return true;

    // check if filename matches
    Sg_File_Info* fi = sgLocNode->get_file_info();
    return (filename == QString(fi->get_filenameString().c_str()) );
}

bool AstFilterFileById::displayNode(SgNode * node) const
{
    SgLocatedNode* sgLocNode = isSgLocatedNode(node);
    if(sgLocNode==NULL)
        return true;

    if(isSgGlobal(node))
        return true;


    Sg_File_Info* fi = sgLocNode->get_file_info();
    return (fileId == fi->get_file_id() );
}

bool AstFilterMetricAttributeByThreshold::displayNode( SgNode *node ) const
{
    if( info.eval( node, name )  > double(threshold)/100.0 )
        return true;
    
    return false;
}
