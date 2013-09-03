#ifndef FIXUP_TYPE_REFERENCES_H
#define FIXUP_TYPE_REFERENCES_H

// This post-processing pass fixes up reference to types that didn't exist 
// as parts of the AST were being constructed and so we built type
// wrappers (reusing a SgModifierType IR node to hold the reference.

// this pos-processing pass fixes up all of these references (which
// are easy to find by design).  However, it leave the SgModifierType
// IR nodes in the AST (which is a bit inelegant).  this may be fixed
// up later to be mroe refined.  Initially this is an effort to test 
// this solution as a way to get past what appears to be a fundamental
// problem in the use of the new EDG within ROSE.  There might be 
// other solutions as well.  Some are elaborated upon in the comments 
// in edgRose.C in the function convert_type() in case tk_typeref.


class FixupTypeReferencesOnMemoryPool
// : public SgSimpleProcessing
   : public ROSE_VisitTraversal
   {
     public:
      //! Function to support traversal of types (where islands can hide)
       // void visitType ( SgType* typeNode );

      //! Required traversal function
          void visit (SgNode* node);

       // This avoids a warning by g++
         virtual ~FixupTypeReferencesOnMemoryPool(); 
   };


void fixupTypeReferences();

// endif for FIXUP_TYPE_REFERENCES_H
#endif
