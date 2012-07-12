#include "rose.h"

#include "UnparseHeadersTransformVisitor.h"


const string UnparseHeadersTransformVisitor::matchEnding = "_rename_me";
const size_t UnparseHeadersTransformVisitor::matchEndingSize = matchEnding.size();
const string UnparseHeadersTransformVisitor::renameEnding = "_renamed";


void UnparseHeadersTransformVisitor::visit(SgNode* node) {
    //Use a pointer to a constant SgVariableDeclaration to be able to call the constant getter variableDeclaration -> get_variables(), 
    //which does not mark the node as modified.
    const SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);
    if (variableDeclaration != NULL) {
        const SgInitializedNamePtrList& nameList = variableDeclaration -> get_variables();
        for (SgInitializedNamePtrList::const_iterator nameListIterator = nameList.begin(); nameListIterator != nameList.end(); nameListIterator++) {
            string originalName = ((*nameListIterator) -> get_name()).getString();
            //Rename any variable, whose name ends with matchEnding.
            if (originalName.size() >= matchEndingSize && originalName.compare(originalName.size() - matchEndingSize, matchEndingSize, matchEnding) == 0) { 
                SageInterface::set_name(*nameListIterator, originalName + renameEnding);
            }
        }
    }	
}
