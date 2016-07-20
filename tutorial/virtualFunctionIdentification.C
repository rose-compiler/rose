// Example ROSE Translator: used within ROSE/tutorial

// This example demonstrates the identification of virtual function 
// declarations and virtual function calls.  These are only resolved staticly
// and with no special analysis to support it.

#include "rose.h"

using namespace std;

int
main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Build a list of functions within the AST
     Rose_STL_Container<SgNode*> memberFunctionDeclarationList = NodeQuery::querySubTree (project,V_SgMemberFunctionDeclaration);

     int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = memberFunctionDeclarationList.begin(); i != memberFunctionDeclarationList.end(); i++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(*i);
          ROSE_ASSERT(memberFunctionDeclaration != NULL);

          if (memberFunctionDeclaration->get_functionModifier().isVirtual() == true)
             {
               printf ("Virtual Member Function #%2d name is %s at line %d \n",
                    counter++,memberFunctionDeclaration->get_name().str(),
                    memberFunctionDeclaration->get_file_info()->get_line());
             }
            else
             {
               printf ("Non-virtual Member Function #%2d name is %s at line %d \n",
                    counter++,memberFunctionDeclaration->get_name().str(),
                    memberFunctionDeclaration->get_file_info()->get_line());
             }
        }

     Rose_STL_Container<SgNode*> memberFunctionRefExpList = NodeQuery::querySubTree (project,V_SgMemberFunctionRefExp);
     counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = memberFunctionRefExpList.begin(); i != memberFunctionRefExpList.end(); i++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(*i);
          ROSE_ASSERT(memberFunctionRefExp != NULL);
          SgFunctionSymbol* functionSymbol = memberFunctionRefExp->get_symbol();
          ROSE_ASSERT(functionSymbol != NULL);
          SgDeclarationStatement* declaration = functionSymbol->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declaration);
          ROSE_ASSERT(memberFunctionDeclaration != NULL);

          SgMemberFunctionDeclaration* nondefiningMemberFunctionDeclaration = isSgMemberFunctionDeclaration(memberFunctionDeclaration->get_firstNondefiningDeclaration());
          if (nondefiningMemberFunctionDeclaration->get_functionModifier().isVirtual() == true)
             {
               printf ("Virtual function call #%2d name is %s at line %d MemberDeclaration at line = %d \n",
                    counter++,nondefiningMemberFunctionDeclaration->get_name().str(),
                    memberFunctionRefExp->get_file_info()->get_line(),
                    nondefiningMemberFunctionDeclaration->get_file_info()->get_line());
             }
            else
             {
               printf ("Non-virtual function call #%2d name is %s at line %d MemberDeclaration at line = %d \n",
                    counter++,nondefiningMemberFunctionDeclaration->get_name().str(),
                    memberFunctionRefExp->get_file_info()->get_line(),
                    nondefiningMemberFunctionDeclaration->get_file_info()->get_line());
             }
        }

     return 0;
   }
