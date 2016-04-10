#include <rose.h>

int main (int argc, char** argv)
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc, argv);

  // Run internal consistency tests on AST
     AstTests::runAllTests(project);

     generateDOT(*project);

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // get class definition of Foo
     Rose_STL_Container<SgNode*> class_definitions = NodeQuery::querySubTree(project, V_SgClassDefinition);
     SgClassDefinition* class_def_foo = isSgClassDefinition(class_definitions.at(0));

#if 0
     printf ("class_definitions.size() = %zu \n",class_definitions.size());
  // printf ("class_def_foo = %p \n",class_def_foo);
     printf ("class_def_foo = %p = %s = %s \n",class_def_foo,class_def_foo->class_name().c_str(),class_def_foo->get_declaration()->get_name().str());
#endif

  // get the defining function declaration of Foo::bar
     SgFunctionDeclaration *func_decl = SageInterface::findFunctionDeclaration( class_def_foo,"bar",NULL,true ) ;

     return 0;
   }
