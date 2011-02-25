#include "rose.h"
#include <boost/foreach.hpp>

int main(int argc, char** argv)
   {
     SgProject* project = frontend(argc, argv);

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *project );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);

     backend(project);

     std::vector<SgFunctionDeclaration*> functions = SageInterface::querySubTree<SgFunctionDeclaration>(project,V_SgFunctionDeclaration);
     BOOST_FOREACH(SgFunctionDeclaration* function, functions)
        {
       // Process each function only once
          if (function != function->get_firstNondefiningDeclaration())
               continue;

          function = isSgFunctionDeclaration(function->get_definingDeclaration());
          if (function != NULL)
             {
                printf ("function = %p = %s = %s \n",function,function->class_name().c_str(),function->get_name().str());
               ROSE_ASSERT(function->get_definition() != NULL);
             }
        }
     return 0;
   }
