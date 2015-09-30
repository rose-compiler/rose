#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include "rewrite.h"

// Extra header for customizing the rewrite mechanism
#include "rewriteTemplateImpl.h"

// Use new rewrite mechanism
#define USE_REWRITE_MECHANISM 1

// Notice that only the names of the evaluate functions change
// along with the derivation of the attributes from an AST_Rewrite nested class
#if USE_REWRITE_MECHANISM
#define EVALUATE_INHERITED_ATTRIBUTE_FUNCTION evaluateRewriteInheritedAttribute
#define EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION evaluateRewriteSynthesizedAttribute
#else
#define EVALUATE_INHERITED_ATTRIBUTE_FUNCTION evaluateInheritedAttribute
#define EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION evaluateSynthesizedAttribute
#endif

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class MyInheritedAttribute
#if USE_REWRITE_MECHANISM
// : public AST_Rewrite::InheritedAttribute
#endif
   {
     public:
          MyInheritedAttribute () {};
#if 0
#if USE_REWRITE_MECHANISM
      //! Specific constructors are required
          MyInheritedAttribute ( SgNode* astNode )
             : HighLevelRewrite::InheritedAttribute(astNode) {};
          MyInheritedAttribute ( const MyInheritedAttribute & X, SgNode* astNode )
             : HighLevelRewrite::InheritedAttribute (X,astNode) {};
#else
       // Note that any constructor is allowed
          MyInheritedAttribute () {};
#endif
#endif
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class MySynthesizedAttribute
#if USE_REWRITE_MECHANISM
   : public HighLevelRewrite::SynthesizedAttribute
#endif
   {
     public:
          MySynthesizedAttribute( SgNode* node )
             : HighLevelRewrite::SynthesizedAttribute(node) {};

  // private:
          MySynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
#if USE_REWRITE_MECHANISM
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class MyTraversal
   : public HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttribute,MySynthesizedAttribute>
#else
 /*! Any AST processing class may be used but the conversion 
    is trivial if SgTopDownBottomUpProcessing is used.
  */
class MyTraversal
   : public SgTopDownBottomUpProcessing<MyInheritedAttribute,MySynthesizedAttribute>
#endif
   {
     public:
#if 0
#if USE_REWRITE_MECHANISM
          MyTraversal ( SgProject & project )
             : HighLevelRewrite::RewriteTreeTraversal<MyInheritedAttribute,MySynthesizedAttribute>(project) {};
#else
          MyTraversal ( SgProject & project ) {};
#endif
#endif

       // Functions required by the tree traversal mechanism
          MyInheritedAttribute EVALUATE_INHERITED_ATTRIBUTE_FUNCTION (
             SgNode* astNode,
             MyInheritedAttribute inheritedAttribute );

          MySynthesizedAttribute EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION (
             SgNode* astNode,
             MyInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Functions required by the tree traversal mechanism
MyInheritedAttribute
MyTraversal::EVALUATE_INHERITED_ATTRIBUTE_FUNCTION (
     SgNode* astNode,
     MyInheritedAttribute inheritedAttribute )
   {
     MyInheritedAttribute returnAttribute;

#if 0
#if USE_REWRITE_MECHANISM
  // Note that we have to use a particular constructor 
  // (to pass on context information about source code position).
  // This allows the Rewrite mechanism to position new source code 
  // relative to the current position using a simple interface.
     MyInheritedAttribute returnAttribute(inheritedAttribute,astNode);
#else
  // Note that any constructor will do
     MyInheritedAttribute returnAttribute;
#endif
#endif

     return returnAttribute;
   }

MySynthesizedAttribute
MyTraversal::EVALUATE_SYNTHESIZED_ATTRIBUTE_FUNCTION (
     SgNode* astNode,
     MyInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
#if USE_REWRITE_MECHANISM
     MySynthesizedAttribute returnAttribute(astNode);
#else
  // Note that any constructor will do
     MySynthesizedAttribute returnAttribute;
#endif

     return returnAttribute;
   }

int
main( int argc, char * argv[] )
   {
  // This test code demonstrates the differences between an ordinary 
  // SgTopDownBottomUpProcessing traversal and a traversal using the 
  // AST Rewrite Mechanism.

     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!
     if (argc == 1)
        {
       // Print usage and exit with exit status == 1
          rose::usage (1);
        }

  // Build the project object which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* sageProject = frontend(argc,argv); 

  // Warnings from EDG processing are OK but not errors
     ROSE_ASSERT (sageProject->get_frontendErrorCode() <= 3);

     cout << "EDG/SAGE Processing DONE! (manipulate with ROSE ...) " << endl;

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     AstPDFGeneration pdftest;
     pdftest.generateInputFiles(sageProject);

  // Build the inherited attribute
  // MyInheritedAttribute inheritedAttribute (sageProject);
     MyInheritedAttribute inheritedAttribute;

  // The traversal uses the AST rewrite mechanism which requires the SgProject object to retrive the
  // command line for compilation of the intermeditate files (from strings to AST fragments) before
  // patching them into the application's AST.
  // MyTraversal myTraversal(sageProject);
     MyTraversal myTraversal;

  // Call the traversal starting at the sageProject node of the AST
     myTraversal.traverseInputFiles(sageProject,inheritedAttribute);

  // Generate the final C++ source code from the potentially modified SAGE AST
     sageProject->unparse();

     cout << "Generation of final source code (unparsing) DONE! (compile ...) " << endl;

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code) to generate
  // an object file.
     int finalCombinedExitStatus = sageProject->compileOutput();
     printf ("Program Compiled Normally (exit status = %d)! \n\n\n\n",finalCombinedExitStatus);
     return finalCombinedExitStatus;
   }
















