#include "rose.h"

#include "rose_aterm_api.h"

ATerm convertNodeToAterm(SgNode* project)
   {
  // This function likely does not make sense because we will not be converting ROSE AST's to Aterms via a serialization.

  // I think that we need an envelope data structure to hold the SgNode* and represent the AST.

  // ROSE_ASSERT(!"convertNodeToAterm(): not implemented!");

     printf ("Inside of convertNodeToAterm(project = %p = %s) \n",project,project->class_name().c_str());

  // Note that this is a C data structure, so we have some likitations on what we can do.
  // We have specifically added a SgNode* data member to this data structure so that we
  // can build them as ATerm objects and permit the C ATerm API to pass them around 
  // between ATerm API functions (and macros).  We might have to represent all of the 
  // macros as proper functions at some point, but this is more complex since as macros
  // the the ATerm API exploits some liberties that would not be available to functions
  // (because of the type system).  At least this is my current understanding.

  // Allocating these on the heap is not enough, we have to properly initialize them as ATerms.
  // In the case of a SgProject IR node this should be a node with a short list of children.
  // The children should not be interpreted, only the subtree root SgProject* should be setup 
  // as a ATerm. Children would be interpreted when they are referenced (as seperate ATerm objects).

  // ATerm tmp_aterm;
  // ATerm tmp_aterm = new _ATerm;
  // convertNodeToAterm(isSgSourceFile(n)->get_globalScope()));
     ROSE_ASSERT(isSgSourceFile(project) != NULL);
     SgGlobal* globalScope = isSgSourceFile(project)->get_globalScope();

     printf ("Building a the child Aterm for the global scope = %p \n",globalScope);

  // Let this be a signature of an unevaluated (unwrapped) child node.
     ATerm wrappedChild = ATmake("wrappedChild(<int>)",99);

     printf ("Constructed the wrappedChild = %p \n",wrappedChild);

  // wrappedChild->aterm.rose_IR_node = globalScope;

     printf ("Assgined the global scope into the wrappedChild = %p \n",wrappedChild);

     ATerm tmp_aterm = ATmake("SourceFile(<term>)",wrappedChild);

  // We have to make a proper term from this ROSE AST IR node.  To this extent we are likely to 
  // serialize and de-serialize the AST (but only on a node by node basis).  In some cases this 
  // will not be required, but when we return an ATerm we have to build something.
  // So likely we need to make a ATerm here that will at least hold the SgNode* and be able to
  // be queried using the ATerm API (including macros that reference specific parts of the ATerm 
  // data structure.

  // I think this should be a AT_APPL == 1, type of applications of function names to terms



     printf ("Assign project pointer to tmp_aterm %p \n",tmp_aterm);
  // tmp_aterm->aterm.rose_IR_node = project;

  // atermType will be one of: AT_APPL, AT_INT, AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
     int atermType = ATgetType(tmp_aterm);

     printf ("ATgetType(tmp_term) = %d \n",atermType);

  // The value of 7 is an error state, I think.
     ROSE_ASSERT(atermType != 7);

     printf ("Returning tmp_aterm = %p \n",tmp_aterm);
     return tmp_aterm;
   }



