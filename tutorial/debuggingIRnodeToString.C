// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

int
main ( int argc, char* argv[] )
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     if (SgProject::get_verbose() > 0)
          printf ("In preprocessor.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // AST diagnostic tests
     AstTests::runAllTests(const_cast<SgProject*>(project));

  // test statistics
     if (project->get_verbose() > 1)
        {
          cout << AstNodeStatistics::traversalStatistics(project);
          cout << AstNodeStatistics::IRnodeUsageStatistics();
        }

     if (project->get_verbose() > 0)
          printf ("Generate the pdf output of the SAGE III AST \n");
     generatePDF ( *project );

     if (project->get_verbose() > 0)
          printf ("Generate the DOT output of the SAGE III AST \n");
     generateDOT ( *project );

     Rose_STL_Container<SgNode*> nodeList;
  // nodeList = NodeQuery::querySubTree (project,V_SgType,NodeQuery::ExtractTypes);
     nodeList = NodeQuery::querySubTree (project,V_SgForStatement);
     printf ("\nnodeList.size() = %zu \n",nodeList.size());

     Rose_STL_Container<SgNode*>::iterator i = nodeList.begin();
     while (i != nodeList.end())
        {
          printf ("Query node = %p = %s = %s \n",*i,(*i)->sage_class_name(),(*i)->unparseToString().c_str());
          i++;
        }

     return 0;
   }
