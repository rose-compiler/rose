
#include "rose.h"

#include <iostream>

#include "AsmFunctions.h"


using namespace std;

void annotate( SgNode *node, SgNode *binaryNode )
{
   AsmFunctions functions( binaryNode );

   typedef Rose_STL_Container<SgNode *>::const_iterator const_iterator;

   Rose_STL_Container<SgNode *> asmFunctions =
      NodeQuery::querySubTree( node, V_SgFunctionDeclaration );

   for( const_iterator i = asmFunctions.begin(); i != asmFunctions.end(); ++i )
   {
      SgFunctionDeclaration *fun( isSgFunctionDeclaration( *i ) );
      assert( fun );

      /*cout << "Source Function: " << fun->get_name() << endl;
      cout << "                 " << fun->get_asm_name() << endl;*/
   }
}

int main( int argc, char **argv )
{
   std::vector<string> argvList( argv, argv+argc );
   char **argvSource( new char *[argc-1] );

   argvSource[0] = new char [strlen(argv[0] )];
   strcpy( argvSource[0], argv[0] );

   argvSource[1] = new char [strlen(argv[1] )];
   strcpy( argvSource[1], argv[1] );

   char **argvBinary( new char *[argc-1] );

   argvBinary[0] = new char [strlen(argv[0] )];
   strcpy( argvBinary[0], argv[0] );

   argvBinary[1] = new char [strlen(argv[2] )];
   strcpy( argvBinary[1], argv[2] );




   cout << argc << " " << argc-3 << endl;

   for( int i = 2; i < argc-1; ++i )
   {
      cout << ".." << endl;
      size_t len( strlen( argv[i+1] ) );

      argvSource[i] = new char[len];
      argvBinary[i] = new char[len];

      strcpy( argvSource[i], argv[i+1] );
      strcpy( argvBinary[i], argv[i+1] );
   }
   //argvSource[argc-2] = NULL;
   //argvBinary[argc-2] = NULL;


   cout << "Calling frontend for binary file ..."<<endl;
   SgProject *binaryProject = frontend( argc-1, argvBinary );
   cout << "Calling frontend for source file ..."<<endl;
   SgProject *sourceProject = frontend( argc-1, argvSource );

   annotate( sourceProject, binaryProject );


   return 0;
}
