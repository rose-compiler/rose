#ifndef FIXUP_FRIEND_TEMPLATE_DECLARATIONS_H
#define FIXUP_FRIEND_TEMPLATE_DECLARATIONS_H

/*! \brief This traversal uses the Memory Pool traversal to fixup the friend specifications on template declarations.

    There appears to be no way to tell if a template declaration for a function is marked as friend except that the 
    scopes are different.  So we use this information to explicitly mark friends.
 */

#include "Cxx_Grammar.h"

class FixupFriendTemplateDeclarations : public ROSE_VisitTraversal
   {
     public:
     virtual ~FixupFriendTemplateDeclarations() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

void fixupFriendTemplateDeclarations();

/*! \brief This traversal uses the Memory Pool traversal to fixup the friend specifications on all declarations.

    This allows instantiated template member function which we marked as friends to be marked consistantly.
    It also checks that all the other possible friend declarations are correctly makred.
 */
class FixupFriendDeclarations : public ROSE_VisitTraversal
   {
     public:
     virtual ~FixupFriendDeclarations() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

void fixupFriendDeclarations();


// endif for FIXUP_FRIEND_TEMPLATE_DECLARATIONS_H
#endif
