
#include "rose.h"

#include <iostream>

#include "AsmToSourceMapper.h"


using namespace std;

void annotate( SgNode *node, SgNode *binaryNode )
{
   //AsmFunctions functions( binaryNode );

   typedef Rose_STL_Container<SgNode *>::const_iterator const_iterator;

   Rose_STL_Container<SgNode *> asmFunctions =
      NodeQuery::querySubTree( node, V_SgFunctionDefinition );

   for( const_iterator i = asmFunctions.begin(); i != asmFunctions.end(); ++i )
   {
      SgFunctionDefinition *fun( isSgFunctionDefinition( *i ) );
      assert( fun );

      /*SgFunctionDeclaration *decl( fun->get_declaration() );
      if( decl == NULL ) continue;
      SgDeclarationStatement *defDecl( decl->get_definingDeclaration() );
      if( defDecl == NULL ) continue;
      SgFunctionDeclaration *d( isSgFunctionDeclaration( defDecl ) );
      if( d == NULL ) continue;

      SgFunctionType *fType( d->get_type() );

      SgInitializedNamePtrList &parameter( d->get_args() );

      SgName mangledFunctionName( d->get_mangled_name() );
      //SgName mangledTypes( mangleTypes( parameter.begin(), parameter.end() ) );
      //SgName mangledTypes( parameter.get_mangled_name() );
      SgName mangledTypes( d->get_parameterList()->get_mangled_name() );

      cout << "Source Function: " << decl->get_name().getString() << endl;
      cout << "Source Function: " << d->get_name().getString() << endl;
      cout << joinMangledQualifiers( mangledFunctionName, mangledTypes ).getString() << endl;
      cout << defDecl->class_name() << endl;
      cout << d->get_parameterList()->unparseToString() << endl;
      cout << "                 " << fun->get_qualified_name().str() << endl;
      cout << "                 " << decl->get_qualified_name().str() << endl;
      cout << "                 " << d->get_qualified_name().str() << endl;
      cout << "                 " << fun->get_mangled_name().getString() << endl;
      cout << "                 " << defDecl->get_mangled_name().getString() << endl;
      cout << "                 " << fun->unparseToString() << endl;
      cout << "                 " << fType->get_mangled().getString() << endl;
      cout << "                 " << fType->unparseToString() << endl;
      cout << "-----------------" << endl;*/
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

   //annotate( sourceProject, binaryProject );

   AsmFunctions asmF( isSgBinaryFile( (*binaryProject)[0] ) );
   for( int i( 1 ); i < binaryProject->numberOfFiles(); ++i )
       asmF.addFile( isSgBinaryFile( (*binaryProject)[i] ) );

   for( int i( 0 ); i < sourceProject->numberOfFiles(); ++i )
       asmF.annotate( isSgSourceFile( (*sourceProject)[i] ) );
   //    asmF.annotate( isSgSourceFile( (*sourceProject)[0] ) );

   return 0;
}
