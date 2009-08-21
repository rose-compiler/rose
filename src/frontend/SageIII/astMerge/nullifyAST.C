// #include <merge.h>
#include "rose.h"

using namespace std;

void
NullTree()
   {
  // This is a test code (distroys AST)
     NullTreeMemoryPoolTraversal t;
     t.traverseMemoryPool();
   }

void
NullTreeMemoryPoolTraversal::visit ( SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
  // printf ("FixupTraversal::visit: node = %s \n",node->class_name().c_str());

  // skip sgSymbol IR nodes since they seem to cause problems
     SgSymbol*     symbol     = isSgSymbol(node);
     SgStatement*  statement  = isSgStatement(node);
     Sg_File_Info* fileInfo   = isSg_File_Info(node);
     SgTypedefSeq* typedefSeq = isSgTypedefSeq(node);
     SgType*       type       = isSgType(node);
     SgFile*       file       = isSgFile(node);
     SgProject*    project    = isSgProject(node);
     SgFunctionParameterTypeList* functionParameterTypeList = isSgFunctionParameterTypeList(node);
  // SgSymbolTable* symbolTable = isSgSymbolTable(node);

     bool nullifyIRchildren = false;
     nullifyIRchildren = (functionParameterTypeList != NULL) || (project != NULL) || (file != NULL) || 
                         (symbol != NULL) || (statement != NULL) || (fileInfo != NULL) || 
                         (typedefSeq != NULL) | (type != NULL);

  // The memory pool based traversal used in the graphing of the AST fails if we reset the symbol table entries
  // nullifyIRchildren = nullifyIRchildren || (symbolTable != NULL);

     if (nullifyIRchildren == true)
        {
          struct Nullifier: public SimpleReferenceToPointerHandler {
            virtual void operator()(SgNode*& n, const SgName&, bool /* traverse */) {n = NULL;}
          };
          Nullifier nf;
          node->processDataMemberReferenceToPointers(&nf);
        }
   }

