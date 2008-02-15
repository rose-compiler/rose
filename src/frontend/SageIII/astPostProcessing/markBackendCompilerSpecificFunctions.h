#ifndef MARK_BACKEND_COMPILER_SPECIFIC_FUNCTIONS_AS_COMPILER_GENERATED_H
#define MARK_BACKEND_COMPILER_SPECIFIC_FUNCTIONS_AS_COMPILER_GENERATED_H

// DQ (3/5/2006):
// This file declares the function in ROSE which marks back end (vendor compiler)
// specific functions as compiler generated.  Such declarations appear in the file:
// rose_edg_macros_and_functions_required_for_gnu.h and are specific to the use of 
// the GNU gcc and g++ backend.  Other backends can expect to use similarly specific
// declarations.  These are what should have perhaps been accepted by EDG without
// requiring there explicit declaration, but this is likely an oversight in the
// GNU gcc and g++ compatability mode of EDG.

// DQ (3/5/2006):
/*! \brief Mark an backend specific functions as compiler generated.

    This function the function marks back end (vendor compiler)
    specific functions as compiler generated.  Such declarations appear in the file:
    rose_edg_macros_and_functions_required_for_gnu.h and are specific to the use of 
    the GNU gcc and g++ backend.  Other backends can expect to use similarly specific
    declarations.  These are what should have perhaps been accepted by EDG without
    requiring there explicit declaration, but this is likely an oversight in the
    GNU gcc and g++ compatability mode of EDG.
 */
void markBackendSpecificFunctionsAsCompilerGenerated( SgNode* node );

/*! \brief Supporting traversal to mark an backend specific functions as compiler generated.

    This class is a traversal specif to the lower level support of the 
    markBackendSpecificFunctionsAsCompilerGenerated(SgNode*) function.
 */
// DQ (5/8/2006): Implement this using the memory pool traversal so that we will visit every IR node
// class MarkBackendSpecificFunctionsAsCompilerGenerated : public SgSimpleProcessing
class MarkBackendSpecificFunctionsAsCompilerGenerated : public ROSE_VisitTraversal
   {
     public:
          std::string targetFileName;
          Sg_File_Info* targetFile;

          virtual ~MarkBackendSpecificFunctionsAsCompilerGenerated();
          MarkBackendSpecificFunctionsAsCompilerGenerated();

      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for MARK_BACKEND_COMPILER_SPECIFIC_FUNCTIONS_AS_COMPILER_GENERATED_H
#endif
