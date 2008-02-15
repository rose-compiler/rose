#ifndef FIXUP_CONSTRUCTOR_PREINITIALIZATION_LISTS_H
#define FIXUP_CONSTRUCTOR_PREINITIALIZATION_LISTS_H

/*! \brief This traversal uses the Memory Pool traversal to fixup names of template classes that appear in constructor preinitialization lists.

 */
class ResetContructorInitilizerLists : public ROSE_VisitTraversal
   {
     public:
     virtual ~ResetContructorInitilizerLists() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

void resetContructorInitilizerLists();

// endif for FIXUP_CONSTRUCTOR_PREINITIALIZATION_LISTS_H
#endif
