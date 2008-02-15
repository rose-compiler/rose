#include <merge.h>

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
       // Only traverse the new part of the AST
          typedef vector<pair<SgNode**,string> > DataMemberMapType;
          DataMemberMapType dataMemberMap = node->returnDataMemberReferenceToPointers();

          DataMemberMapType::iterator i = dataMemberMap.begin();
          while (i != dataMemberMap.end())
             {
            // Ignore the parent pointer since it will be reset differently if required
               SgNode** pointerToKey = i->first;
            // SgNode* key           = *(i->first);
            // string & debugString  = i->second;

               *pointerToKey = NULL;

               i++;
             }
        }
   }

