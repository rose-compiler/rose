#if 0
// This test code (below) generates output that we would like to understand better (from the AST consistancy tests).

In TestForParentsMatchingASTStructure::preOrderVisit(): (node->get_parent() != stack.back()): prefix =      nodes parent property does not match traversal parent
traversal parent = 0x2b46bad37010 = SgTypedefDeclaration 
   stack[i]->get_parent() = (nil) 
   stack[i]->get_parent() = 0x2b46ba694010 
   stack[i]->get_parent() = 0x2b46ba694010 = SgProject 
   stack[i]->get_parent() = 0x16bfaf0 
   stack[i]->get_parent() = 0x16bfaf0 = SgFileList 
   stack[i]->get_parent() = 0x2b46ba72d010 
   stack[i]->get_parent() = 0x2b46ba72d010 = SgSourceFile 
   stack[i]->get_parent() = 0x2b46ba78d010 
   stack[i]->get_parent() = 0x2b46ba78d010 = SgGlobal 
   stack[i]->get_parent() = 0x2b46baa1a010 
   stack[i]->get_parent() = 0x2b46baa1a010 = SgNamespaceDeclarationStatement 
   stack[i]->get_parent() = 0x2b46baaab010 
   stack[i]->get_parent() = 0x2b46baaab010 = SgNamespaceDefinitionStatement 
Detected AST parent/child relationship problems after AST post processing:
    AST path (including node) when inconsistency was detected:
        #0    SgProject 0x2b46ba694010; parent=0
        #1    SgFileList 0x16bfaf0; parent=0x2b46ba694010
        #2    SgSourceFile 0x2b46ba72d010; parent=0x16bfaf0
        #3    SgGlobal 0x2b46ba78d010; parent=0x2b46ba72d010
        #4    SgNamespaceDeclarationStatement 0x2b46baa1a010; parent=0x2b46ba78d010
        #5    SgNamespaceDefinitionStatement 0x2b46baaab010; parent=0x2b46baa1a010
        #6    SgTypedefDeclaration 0x2b46bad37010; parent=0x2b46baaab010
        #7    SgClassDeclaration 0x2b46bab33288; parent=0x2b46baaab010

#endif

namespace ABC
   {
  // This declaration causes the output of that is presented above.
     struct XIncludeHistoryNode;

     typedef struct XIncludeHistoryNode
        {
          int *URI;
          struct XIncludeHistoryNode *next;
        } XIncludeHistoryNode;

     class X
        {
          public: 
               XIncludeHistoryNode* x;
        };
   }



