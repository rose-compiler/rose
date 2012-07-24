////////////OpenMP Checker main///////////////////////
/*
modified by Hongyi Ma
07/ 19 / 2012
*/
#include "ompCheckInterface.h"

using namespace ompCheckerInterface;
using namespace std;

//   MAIN

int main ( int argc, char* argv[] ) {
    printf("hello world, Ma !\n");

    if ( SgProject::get_verbose() > 0 )
        printf ( "In visitorTraversal.C: main() \n" );

    SgProject * project = frontend( argc, argv );
    ROSE_ASSERT ( project != NULL );
    
    // Build the traversal object
    visitorTraversal exampleTraversal;
    
    // Call the traversal function (member function of AstSimpleProcessing)
    // starting at the project node of the AST, using a preorder traversal.
    exampleTraversal.traverseInputFiles( project, preorder );
    
    return 0;
}

