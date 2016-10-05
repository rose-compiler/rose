// Example ROSE Preprocessor
// used for testing ROSE infrastructure

#include "rose.h"
// #include <iomanip>
// #include "AstConsistencyTests.h"

#include "FunctionNormalization.h"

int
main( int argc, char * argv[] ) {
  SgProject* project = new SgProject(argc, argv);

  project = frontend( argc, argv );
  FunctionCallNormalization traverse;
  traverse.traverseInputFiles( project, postorder );
  generateDOT( *project );
  
  /*
  list<SgNode *> init = NodeQuery::querySubTree( project, V_SgInitializedName );
  for ( list<SgNode *>::iterator i = init.begin(); i != init.end(); i++ )
    {
      SgInitializedName *s = isSgInitializedName( *i );
      ROSE_ASSERT ( s );
      cout << "Node: " << s << "\t";
      cout << s->get_name().str();
      if ( s->get_scope() )
	cout << "\thas scope " << s->get_scope()
             << "\t" << s->get_scope()->sage_class_name() << "\n";
      else
	cout << "\tNO SCOPE SET\n";
    }
  */

  AstTests::runAllTests(project);
  
  return backend(project);
}
