// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Example of how to do an unsafe unparse of a string instead of a subtree in the AST.
#include "rose.h"

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
     if (SgForStatement* forStmt = isSgForStatement(n))
        {
          printf ("Found a for loop ... \n");
		  ROSE_ASSERT(forStmt != NULL);
		  ROSE_ASSERT(forStmt->get_file_info() != NULL);
		  if( forStmt->getAttachedPreprocessingInfo() == NULL)
			forStmt->getAttachedPreprocessingInfo() = new AttachedPreprocessingInfoType();

          forStmt->getAttachedPreprocessingInfo()->push_back(new PreprocessingInfo(PreprocessingInfo::LineReplacement,
          	                   "/* REPLACED FOR LOOP WITH IF */ if (0);",forStmt->get_file_info()->get_filenameString(),1,1,1,PreprocessingInfo::before));
        }
     if (SgLocatedNode* forStmt = isSgLocatedNode(n))
        {
          printf ("Found a for loop ... \n");
		  ROSE_ASSERT(forStmt != NULL);
		  ROSE_ASSERT(forStmt->get_file_info() != NULL);
		  if( forStmt->getAttachedPreprocessingInfo() == NULL)
			forStmt->getAttachedPreprocessingInfo() = new AttachedPreprocessingInfoType();

          forStmt->getAttachedPreprocessingInfo()->push_back(new PreprocessingInfo(PreprocessingInfo::LineReplacement,
          	                   "/* REPLACED FOR LOOP WITH IF */ if (0);",forStmt->get_file_info()->get_filenameString(),1,1,1,PreprocessingInfo::before));
        }

   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     if (SgProject::get_verbose() > 0)
          printf ("In visitorTraversal.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);

	 return backend(project);

   }

