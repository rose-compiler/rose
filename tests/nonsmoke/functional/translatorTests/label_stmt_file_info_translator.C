// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

int main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Run internal consistency tests on AST
     AstTests::runAllTests(project);

     // Query for all label statements
     Rose_STL_Container<SgNode*> label_list;
     label_list = NodeQuery::querySubTree (project, 
                                     V_SgLabelStatement);
     
     // For each label statment, print file info
     Rose_STL_Container<SgNode*>::iterator j;
     for (j = label_list.begin(); j != label_list.end(); j++)
     {
        SgLabelStatement* label_stmt = isSgLabelStatement(*j);
        ROSE_ASSERT(label_stmt);

        printf("%d:%s \n", label_stmt->get_file_info()->get_line(), label_stmt->get_file_info()->get_filename());
     }
 
   }

