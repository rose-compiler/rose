// Author: Markus Schordan
// $Id: AstTextAttributesHandling.C,v 1.5 2008/01/08 02:56:39 dquinlan Exp $

#ifndef ASTTEXTATTRIBUTEHANDLING_C
#define ASTTEXTATTRIBUTEHANDLING_C
#include "rose.h"
#include <string>
#include "AstTextAttributesHandling.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/13/2007): This is a slow test and is tested separately in the 
// tests of the AST (see directory ROSE/tests/CompileTests/unparseToString_tests
// so we would like to avoid mixing that testing with the attribute testing.
#define UNPARSE_SOURCE_TEST 0


AstTextAttributesHandling::AstTextAttributesHandling(SgProject* p) 
  : projectnode(p) {
}

AstTextAttributesHandling::~AstTextAttributesHandling() { 
}

void
AstTextAttributesHandling::setAll() {
  mode=M_set;
  traverseInputFiles(projectnode,preorder);
}

void
AstTextAttributesHandling::removeAll() {
  mode=M_remove;
  traverseInputFiles(projectnode,preorder);
}

void 
AstTextAttributesHandling::visit(SgNode* node)
   {
     ROSE_ASSERT (node != NULL);
#if 0
     printf ("In AstTextAttributesHandling::visit(%p): node = %s \n",node,node->sage_class_name());
#endif

  // DQ (2/20/2007): Don't add attributes to expliclt share IR nodes (from the AST merge).  These IR nodes are 
  // explicitly marked as shared and a different from SgType IR nodes which can be shared have not Sg_File_Info 
  // object and so cannot be explicitly makred when they are shared.
     bool markedAsShared = false;
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL && fileInfo->isShared() == true)
          markedAsShared = true;

  // Only apply this test to IR nodes that are not marked as shared (sharedIR nodes would fail the test by design)
  // if (!dynamic_cast<SgType*>(node) || !dynamic_cast<SgTypedefSeq*>(node))
     if ( (!dynamic_cast<SgType*>(node) || !dynamic_cast<SgTypedefSeq*>(node)) && (markedAsShared == false) )
        {
          switch(mode)
             {
               case M_set:
                  {
                 // DQ (1/2/2006): Use the new attribute interface
                 // printf ("In AstTextAttributesHandling::visit() set: using new attribute interface \n");

                 // DQ (4/10/2006): We only test this on statements (since not all IR nodes have the attribute mechanism defined)
                 // SgStatement* statement = dynamic_cast<SgStatement*>(node);
                 // if (statement != NULL)

                    node->addNewAttribute("test1", new AstTextAttribute("value1") );
                    node->addNewAttribute("test2", new AstTextAttribute("value2: newline follows here:\nThis is the 2nd line of value 2.\nThis is the 3rd line of value 2.") );
                    node->addNewAttribute("test3", new AstTextAttribute("value3") );

#if UNPARSE_SOURCE_TEST
                    if (dynamic_cast<SgStatement*>(node))
                       {
                         node->addNewAttribute("source", new AstTextAttribute( node->unparseToString() ) );
                      // node->addNewAttribute("source", new AstTextAttribute( "node->unparseToString()" ) );
                       }
                      else
                       {
                         node->addNewAttribute("source", new AstTextAttribute("-- undefined --") );
                       }
#endif
                    break;
                  }

               case M_remove:
                  {
                 // DQ (1/2/2006): Use the new attribute interface
                 // printf ("In AstTextAttributesHandling::visit() remove: using new attribute interface \n");

                 // DQ (4/10/2006): We only test this on statements (since not allIR nodes have the attribute mechanism defined)
                 // SgStatement* statement = dynamic_cast<SgStatement*>(node);
                 // if (statement != NULL)

                    node->removeAttribute("test1");
                    node->removeAttribute("test2");
                    node->removeAttribute("test3");
#if UNPARSE_SOURCE_TEST
                    node->removeAttribute("source");
#endif
                    break;
                  }

               default:
                    cerr << "error: undefined mode in AstTextAttributesHandling." << endl;

                 // DQ (9/5/2006): Added assertion to make default case fail!
                    ROSE_ASSERT(false);
             }
        }
   }

#endif
