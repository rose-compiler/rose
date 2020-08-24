// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "AstFixup.h"
#include "astPostProcessing.h"

// tps (01/14/2009): Had to define this locally as it is not part of sage3 but rose.h
#include "AstDiagnostics.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

// DQ (3/6/2017): Added support for message logging to control output from ROSE tools.
#undef mprintf
#define mprintf Rose::Diagnostics::mfprintf(Rose::ir_node_mlog[Rose::Diagnostics::DEBUG])

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


// DQ (8/20/2005): Make this local so that it can't be called externally!
void postProcessingSupport (SgNode* node);

// DQ (5/22/2005): Added function with better name, since none of the fixes are really
// temporary any more.
void AstPostProcessing (SgNode* node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST post-processing:");

     ROSE_ASSERT(node != NULL);

#if 0
     printf ("+++++++++++++++++++++++++++++++++++++++++++++++ \n");
     printf ("Inside of AstPostProcessing(node = %p = %s) \n",node,node->class_name().c_str());
     printf ("+++++++++++++++++++++++++++++++++++++++++++++++ \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (1/31/2014): We want to enforce this, but for now issue a warning if it is not followed.
  // Later I want to change the function's API to onoy take a SgProject.  Note that this is 
  // related to a performance bug that was fixed by Gergo a few years ago.  The fix could be
  // improved by enforcing that this could not be called at the SgFile level of the hierarchy.
     if (isSgProject(node) == NULL)
        {
       // DQ (5/17/17): Note that this function is called, and this message is output, from the outliner, which is OK but not ideal.
          if ( SgProject::get_verbose() >= 1 )
            printf ("Warning: AstPostProcessing should ideally be called on SgProject (due to repeated memory pool traversals and quadratic \n");
//          printf ("         behavior (over files) when multiple files are specified on the command line): node = %s \n",node->class_name().c_str());
        }
  // DQ (1/31/2014): This is a problem to enforce this for at least (this test program): 
  //      tests/nonsmoke/functional/roseTests/astRewriteTests/testIncludeDirectiveInsertion.C
  // ROSE_ASSERT(isSgProject(node) != NULL);

  // DQ (3/17/2007): This should be empty
     if (SgNode::get_globalMangledNameMap().size() != 0)
        {
          if (SgProject::get_verbose() > 0)
             {
                printf("AstPostProcessing(): found a node with globalMangledNameMap size not equal to 0: SgNode = %s =%s ", node->class_name().c_str(),SageInterface::get_name(node).c_str());
               printf ("SgNode::get_globalMangledNameMap().size() != 0 size = %" PRIuPTR " (clearing mangled name cache) \n",SgNode::get_globalMangledNameMap().size());
             }

          SgNode::clearGlobalMangledNameMap();
        }
     ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

     switch (node->variantT())
        {
          case V_SgProject:
             {
               SgProject* project = isSgProject(node);
               ROSE_ASSERT(project != NULL);

            // GB (8/19/2009): Added this call to perform post-processing on
            // the entire project at once. Conversely, commented out the
            // loop iterating over all files because repeated calls to
            // AstPostProcessing are slow due to repeated memory pool
            // traversals of the same nodes over and over again.
            // Only postprocess the AST if it was generated, and not were we just did the parsing.
            // postProcessingSupport(node);

            // printf ("In AstPostProcessing(): project->get_exit_after_parser() = %s \n",project->get_exit_after_parser() ? "true" : "false");
               if (project->get_exit_after_parser() == false)
                  {
                    postProcessingSupport (node);
                  }
#if 0
               SgFilePtrList::iterator fileListIterator;
               for (fileListIterator = project->get_fileList().begin(); fileListIterator != project->get_fileList().end(); fileListIterator++)
                  {
                 // iterate through the list of current files
                    AstPostProcessing(*fileListIterator);
                  }
#endif

            // printf ("SgProject support not implemented in AstPostProcessing \n");
            // ROSE_ASSERT(false);
               break;
             }

          case V_SgDirectory:
             {
               SgDirectory* directory = isSgDirectory(node);
               ROSE_ASSERT(directory != NULL);

               printf ("SgDirectory support not implemented in AstPostProcessing \n");
               ROSE_ASSERT(false);
               break;
             }

          case V_SgFile:
          case V_SgSourceFile:
             {
               SgFile* file = isSgFile(node);
               ROSE_ASSERT(file != NULL);

            // Only postprocess the AST if it was generated, and not were we just did the parsing.
               if (file->get_exit_after_parser() == false)
                  {
                    postProcessingSupport (node);
                  }
               
               break;
             }

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
       // Test for a binary executable, object file, etc.
          case V_SgBinaryComposite:
             {
               SgBinaryComposite* file = isSgBinaryComposite(node);
               ROSE_ASSERT(file != NULL);

               printf ("Error: AstPostProcessing of SgBinaryFile is not defined \n");
               ROSE_ASSERT(false);

               break;
             }
#endif

          default:
             {
            // list general post-processing fixup here ...
               postProcessingSupport (node);
             }
        }

#if 0
  // DQ (1/12/2015): Save this so that we can check it for where it might be reset to be compiler generated (a bug).
     extern SgFunctionDeclaration* saved_functionDeclaration;
     ROSE_ASSERT(saved_functionDeclaration != NULL);

     printf ("saved_functionDeclaration = %p = %s \n",saved_functionDeclaration,saved_functionDeclaration->class_name().c_str());
     SgFunctionDeclaration* nondefiningDeclaration = isSgFunctionDeclaration(saved_functionDeclaration->get_firstNondefiningDeclaration());
     SgFunctionDeclaration* definingDeclaration    = isSgFunctionDeclaration(saved_functionDeclaration->get_definingDeclaration());
     printf ("saved_functionDeclaration nondefiningDeclaration = %p \n",nondefiningDeclaration);
     printf ("saved_functionDeclaration definingDeclaration    = %p \n",definingDeclaration);

     saved_functionDeclaration->get_startOfConstruct()->display("AstPostProcessing: saved_functionDeclaration source position of the first non-defining declaration that was modified by EDG: START: debug");
     saved_functionDeclaration->get_endOfConstruct()  ->display("AstPostProcessing: saved_functionDeclaration source position of the first non-defining declaration that was modified by EDG: END: debug");
#endif

  // DQ (3/17/2007): Clear the static globalMangledNameMap, likely this is not enough and the mangled name map 
  // should not be used while the names of scopes are being reset (done in the AST post-processing).
     SgNode::clearGlobalMangledNameMap();
   }

// DQ (3/4/2007): part of tempoary support for debugging where a defining and nondefining declaration are the same
// SgDeclarationStatement* saved_declaration;

void postProcessingSupport (SgNode* node)
   {
  // DQ (5/24/2006): Added this test to figue out where Symbol parent pointers are being reset to NULL
  // TestParentPointersOfSymbols::test();

  // DQ (7/25/2005): It is presently an error to call this function with a SgProject 
  // or SgDirectory, since there is no way to compute the SgFile from such IR nodes 
  // (could be multiply defined).
  // ROSE_ASSERT(isSgProject(node) == NULL && isSgDirectory(node) == NULL);
  // GB (8/19/2009): Removed the assertion against calling this function on
  // SgProject and SgDirectory nodes. Nothing below needs to compute a
  // SgFile, as far as I can tell; also, calling the AstPostProcessing just
  // once on an entire project is more efficient than calling it once per
  // file.

#if 0
     printf ("Inside of postProcessingSupport(node = %p = %s) \n",node,node->class_name().c_str());
#endif

#if 0
  // DQ (11/23/2015): Before we do any modifications, check for unique IR nodes in the AST (see test2015_121.C).
#if 1
     printf ("Checking for unique nodes in the AST before AST post-processing: issolating possible multiple references friend function \n");
#endif

     testAstForUniqueNodes(node);

#if 1
     printf ("DONE: Checking for unique nodes in the AST before AST post-processing: issolating possible multiple references friend function \n");
#endif
#endif

  // Only do AST post-processing for C/C++
  // Rasmussen (4/8/2018): Added Ada, Cobol, and Jovial. The logic should probably
  // be inverted to only process C and C++ but I don't understand interactions like OpenMP langauges.
     bool noPostprocessing = (SageInterface::is_Ada_language()     == true) ||
                             (SageInterface::is_Cobol_language()   == true) ||
                             (SageInterface::is_Fortran_language() == true) ||
                             (SageInterface::is_Jovial_language()  == true) ||
                             (SageInterface::is_PHP_language()     == true) ||
                             (SageInterface::is_Python_language()  == true);

  // If this is C or C++ then we are using the new EDG translation and using fewer 
  // fixups should be required, some are still required.
     if (noPostprocessing == false)
        {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("Postprocessing AST build using new EDG/Sage Translation Interface. \n");
#endif

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 1 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

#if 0
       // DQ (8/12/2020): Adding support for debugging access permissions (see Cxx_tests/test2020_28.C).
          printf ("Calling checkAccessPermissions() at START of astPostprocessing \n");
          SageInterface::checkAccessPermissions(node);
#endif

// DQ (10/27/2015): Added test to detect cycles in typedef types.
#define DEBUG_TYPEDEF_CYCLES 0

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
#endif

#if 0
       // DQ (4/26/2013): Debugging code.
          printf ("In postProcessingSupport: Test 1: Calling postProcessingTestFunctionCallArguments() \n");
          postProcessingTestFunctionCallArguments(node);
#endif

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
#endif

       // DQ (5/1/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
       // Liao 11/21/2012. AstPostProcessing() is called within both Frontend and Midend
       // so we have to detect the mode first before asserting no transformation generated file info objects
          if (SageBuilder::SourcePositionClassificationMode != SageBuilder::e_sourcePositionTransformation)
             {
               detectTransformations(node);
             }

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupTypeReferences() \n");
             }

#if 0
       // DQ (10/27/2015): This has been moved to the EDG/ROSE connection (called before memory management of EDG is done).
       // DQ (8/12/2012): reset all of the type references (to intermediately generated types).
          fixupTypeReferences();
#endif

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling topLevelResetParentPointer() \n");
             }

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 2 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

#if 0
       // DQ (8/2/2019): Adding output graph before resetParent traversal (because the AST in each appears to be different, debugging this).
       // Output an optional graph of the AST (just the tree, when active)
          printf ("In astPostprocessing(): Generating a dot file... (SgFile only) \n");
          SgProject* projectNode = isSgProject(node);
          if (projectNode != NULL)
             {
               generateDOT ( *projectNode, "_astPostprocessing");
             }
       // generateAstGraph(project, 2000);
          printf ("DONE: In astPostprocessing(): Generating a dot file... (SgFile only) \n");
#endif
#if 0
          printf ("In astPostprocessing(): Generate the dot output for multiple files (ROSE AST) \n");
       // generateDOT ( *project );
               generateDOTforMultipleFile ( *projectNode, "_astPostprocessing" );
             }
          printf ("DONE: In astPostprocessing(): Generate the dot output of the SAGE III AST \n");
#endif


       // Reset and test and parent pointers so that it matches our definition 
       // of the AST (as defined by the AST traversal mechanism).
          topLevelResetParentPointer(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("DONE: Calling topLevelResetParentPointer() \n");
             }

#if 0
       // DQ (8/2/2019): Testing test2019_501.C for extra non-defining template instantiation in global scope.
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling resetParentPointersInMemoryPool() \n");
             }

       // DQ (8/23/2012): Modified to take a SgNode so that we could compute the global scope for use in setting 
       // parents of template instantiations that have not be placed into the AST but exist in the memory pool.
       // Another 2nd step to make sure that parents of even IR nodes not traversed can be set properly.
       // resetParentPointersInMemoryPool();
          resetParentPointersInMemoryPool(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("DONE: Calling resetParentPointersInMemoryPool() \n");
             }

#if 0
       // DQ (8/2/2019): Testing test2019_501.C for extra non-defining template instantiation in global scope.
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupAstDefiningAndNondefiningDeclarations() \n");
             }

       // DQ (6/27/2005): fixup the defining and non-defining declarations referenced at each SgDeclarationStatement
       // This is a more sophisticated fixup than that done by fixupDeclarations. See test2009_09.C for an example
       // of a non-defining declaration appearing before a defining declaration and requiring a fixup of the
       // non-defining declaration reference to the defining declaration.
          fixupAstDefiningAndNondefiningDeclarations(node);

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 3 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
#endif
          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupAstDeclarationScope() \n");
             }

       // DQ (6/11/2013): This corrects where EDG can set the scope of a friend declaration to be different from the defining declaration.
       // We need it to be a rule in ROSE that the scope of the declarations are consistant between defining and all non-defining declaration).
          fixupAstDeclarationScope(node);

#if DEBUG_TYPEDEF_CYCLES
          printf ("Calling TestAstForCyclesInTypedefs() \n");
          TestAstForCyclesInTypedefs::test();
          printf ("DONE: Calling TestAstForCyclesInTypedefs() \n");
#endif
          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupAstSymbolTables() \n");
             }

       // Fixup the symbol tables (in each scope) and the global function type 
       // symbol table. This is less important for C, but required for C++.
       // But since the new EDG interface has to handle C and C++ we don't
       // setup the global function type table there to be uniform.
          fixupAstSymbolTables(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupAstSymbolTablesToSupportAliasedSymbols() \n");
             }

       // DQ (4/14/2010): Added support for symbol aliases for C++
       // This is the support for C++ "using declarations" which uses symbol aliases in the symbol table to provide 
       // correct visability of symbols included from alternative scopes (e.g. namespaces).
          fixupAstSymbolTablesToSupportAliasedSymbols(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling resetTemplateNames() \n");
             }

       // DQ (2/12/2012): Added support for this, since AST_consistancy expects get_nameResetFromMangledForm() == true.
          resetTemplateNames(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupTemplateInstantiations() \n");
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

       // **********************************************************************
       // DQ (4/29/2012): Added some of the template fixup support for EDG 4.3 work.
       // DQ (6/21/2005): This function now only marks the subtrees of all appropriate declarations as compiler generated.
       // DQ (5/27/2005): mark all template instantiations (which we generate as template specializations) as compiler generated.
       // This is required to make them pass the unparser and the phase where comments are attached.  Some fixup of filenames
       // and line numbers might also be required.
          fixupTemplateInstantiations(node);

#if 0
       // DQ (4/26/2013): Debugging code.
          printf ("In postProcessingSupport: Test 2: Calling postProcessingTestFunctionCallArguments() \n");
          postProcessingTestFunctionCallArguments(node);
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling markTemplateSpecializationsForOutput() \n");
             }

       // DQ (8/19/2005): Mark any template specialization (C++ specializations are template instantiations 
       // that are explicit in the source code).  Such template specializations are marked for output only
       // if they are present in the source file.  This detail could effect handling of header files later on.
       // Have this phase preceed the markTemplateInstantiationsForOutput() since all specializations should 
       // be searched for uses of (references to) instantiated template functions and member functions.
          markTemplateSpecializationsForOutput(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling markTemplateInstantiationsForOutput() \n");
             }

       // DQ (6/21/2005): This function marks template declarations for output by the unparser (it is part of a 
       // fixed point iteration over the AST to force find all templates that are required (EDG at the moment 
       // outputs only though template functions that are required, but this function solves the more general 
       // problem of instantiation of both function and member function templates (and static data, later)).
          markTemplateInstantiationsForOutput(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupFriendTemplateDeclarations() \n");
             }

       // DQ (10/21/2007): Friend template functions were previously not properly marked which caused their generated template 
       // symbols to be added to the wrong symbol tables.  This is a cause of numerous symbol table problems.
          fixupFriendTemplateDeclarations();
       // DQ (4/29/2012): End of new template fixup support for EDG 4.3 work.
       // **********************************************************************

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupSourcePositionConstructs() \n");
             }

       // DQ (5/14/2012): Fixup source code position information for the end of functions to match the largest values in their subtree.
       // DQ (10/27/2007): Setup any endOfConstruct Sg_File_Info objects (report on where they occur)
          fixupSourcePositionConstructs();

#if 0
       // DQ (4/26/2013): Debugging code.
          printf ("In postProcessingSupport: Test 3: Calling postProcessingTestFunctionCallArguments() \n");
          postProcessingTestFunctionCallArguments(node);
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupTemplateArguments() \n");
             }

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 4 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

       // DQ (2/11/2017): Changed API to use SgSimpleProcessing based traversal.
       // DQ (11/27/2016): Fixup template arguments to additionally reference a type that can be unparsed.
       // fixupTemplateArguments();
          fixupTemplateArguments(node);

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 4.1 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

       // DQ (2/12/2012): This is a problem for test2004_35.C (debugging this issue).
       // printf ("Exiting after calling resetTemplateNames() \n");
       // ROSE_ASSERT(false);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling resetConstantFoldedValues() \n");
             }

       // DQ (10/4/2012): Added this pass to support command line option to control use of constant folding 
       // (fixes bug pointed out by Liao).
       // DQ (9/14/2011): Process the AST to remove constant folded values held in the expression trees.
       // This step defines a consistent AST more suitable for analysis since only the constant folded
       // values will be visited.  However, the default should be to save the original expression trees
       // and remove the constant folded values since this represents the original code.

          SgProject* project = isSgProject(node);
#if 0
          printf ("In postProcessingSupport: project = %p \n",project);
          printf (" --- project->get_suppressConstantFoldingPostProcessing() = %s \n",project->get_suppressConstantFoldingPostProcessing() ? "true" : "false");
#endif
          if (project != NULL && project->get_suppressConstantFoldingPostProcessing() == false) 
             {
               resetConstantFoldedValues(node);
             } else if (project != NULL && SgProject::get_verbose() >= 1) {
            mprintf ("In postProcessingSupport: skipping call to resetConstantFoldedValues(): project->get_suppressConstantFoldingPostProcessing() = %s \n",project->get_suppressConstantFoldingPostProcessing() ? "true" : "false");
          } else if (project == NULL) {
            mprintf ("postProcessingSupport should not be called for non SgProject IR nodes \n");
          }

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 4.2 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupSelfReferentialMacrosInAST() \n");
             }

       // DQ (10/5/2012): Fixup known macros that might expand into a recursive mess in the unparsed code.
          fixupSelfReferentialMacrosInAST(node);

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 4.3 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

       // Make sure that frontend-specific and compiler-generated AST nodes are marked as such. These two must run in this
       // order since checkIsCompilerGenerated depends on correct values of compiler-generated flags.
          checkIsFrontendSpecificFlag(node);
          checkIsCompilerGeneratedFlag(node);

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 4.4 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupFileInfoInconsistanties() \n");
             }

       // DQ (11/14/2015): Fixup inconsistancies across the multiple Sg_File_Info obejcts in SgLocatedNode and SgExpression IR nodes.
          fixupFileInfoInconsistanties(node);

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 5 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

#if 1
          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling markSharedDeclarationsForOutputInCodeGeneration() \n");
             }

       // DQ (2/25/2019): Adding support to mark shared defining declarations across multiple files.
          markSharedDeclarationsForOutputInCodeGeneration(node);
#endif
          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling checkIsModifiedFlag() \n");
             }

       // This resets the isModified flag on each IR node so that we can record 
       // where transformations are done in the AST.  If any transformations on
       // the AST are done, even just building it, this step should be the final
       // step.

#if 0
          printf ("In postProcessingSupport(): noPostprocessing == false: calling unsetNodesMarkedAsModified(): node = %p = %s \n",node,node->class_name().c_str());
#endif

       // DQ (4/16/2015): This is replaced with a better implementation.
       // checkIsModifiedFlag(node);
          unsetNodesMarkedAsModified(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling detectTransformations() \n");
             }

       // DQ (5/2/2012): After EDG/ROSE translation, there should be no IR nodes marked as transformations.
       // Liao 11/21/2012. AstPostProcessing() is called within both Frontend and Midend
       // so we have to detect the mode first before asserting no transformation generated file info objects
          if (SageBuilder::SourcePositionClassificationMode != SageBuilder::e_sourcePositionTransformation)
             {
               detectTransformations(node);
             }

#if 0
       // DQ (4/26/2013): Debugging code.
          printf ("In postProcessingSupport: Test 10: Calling postProcessingTestFunctionCallArguments() \n");
          postProcessingTestFunctionCallArguments(node);
#endif

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling fixupFunctionDefaultArguments() \n");
             }

       // DQ (4/24/2013): Detect the correct function declaration to declare the use of default arguments.
       // This can only be a single function and it can't be any function (this is a moderately complex issue).
          fixupFunctionDefaultArguments(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling addPrototypesForTemplateInstantiations() \n");
             }

       // DQ (5/18/2017): Adding missing prototypes.
          addPrototypesForTemplateInstantiations(node);

          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling checkPhysicalSourcePosition() \n");
             }

       // DQ (12/20/2012): We now store the logical and physical source position information.
       // Although they are frequently the same, the use of #line directives causes them to be different.
       // This is part of debugging the physical source position information which is used in the weaving
       // of the comments and CPP directives into the AST.  For this the consistancy check is more helpful
       // if done befor it is used (here), instead of after the comment and CPP directive insertion in the
       // AST Consistancy tests.
          checkPhysicalSourcePosition(node);

#if 0
       // DQ (6/19/2020): The new design does not require this in the AST currently 
       // (and can cause the output of replicated include directives).
       // DQ (5/7/2020): Adding support to insert include directives.
          if (SgProject::get_verbose() > 1)
             {
               printf ("Calling addIncludeDirectives() \n");
             }

          addIncludeDirectives(node);
#endif

#if 0
       // DQ (7/14/2020): DEBUGGING: Check initializers.
          printf ("Test 6 in postProcessingSupport() \n");
          SageInterface::checkForInitializers(node);
#endif

#if 0
       // DQ (8/12/2020): Adding support for debugging access permissions (see Cxx_tests/test2020_28.C).
          printf ("Calling checkAccessPermissions() at END of astPostprocessing \n");
          SageInterface::checkAccessPermissions(node);
#endif

#if 0
       // DQ (8/14/2020): Adding support for debugging symbol visability (see Cxx_tests/test2020_33.C).
          printf ("Calling checkAccessPermissions() at END of astPostprocessing \n");
          SageInterface::checkSymbolTables(node);
#endif

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("DONE: Postprocessing AST build using new EDG/Sage Translation Interface. \n");
#endif
          return;
        }
      else
        {

          ROSE_ASSERT(node != NULL);

       // DQ (7/19/2005): Moved to after parent pointer fixup!        
       // subTemporaryAstFixes(node);

       // DQ (3/11/2006): Fixup NULL pointers left by users when building the AST
       // (note that the AST translation fixes these directly).  This step is
       // provided as a way to make the AST build by users consistant with what 
       // is built elsewhere within ROSE.
          fixupNullPointersInAST(node);

       // DQ (8/9/2005): Some function definitions in Boost are build without 
       // a body (example in test2005_102.C, but it appears to work fine).
          fixupFunctionDefinitions(node);

       // DQ (8/10/2005): correct any template declarations mistakenly marked as compiler-generated
          fixupTemplateDeclarations(node);

       // Output progress comments for these relatively expensive operations on the AST
          if ( SgProject::get_verbose() >= AST_POST_PROCESSING_VERBOSE_LEVEL )
               cout << "/* AST Postprocessing reset parent pointers */" << endl;
     
          topLevelResetParentPointer (node);

       // DQ (6/10/2007): This is called later, but call it now to reset the parents in SgTemplateInstantiationDecl
       // This is required (I think) so that resetTemplateNames() can compute template argument name qualification correctly.
       // See test2005_28.C for where this is required.
       // resetParentPointersInMemoryPool();
          resetParentPointersInMemoryPool(node);

       // Output progress comments for these relatively expensive operations on the AST
          if ( SgProject::get_verbose() >= AST_POST_PROCESSING_VERBOSE_LEVEL )
               cout << "/* AST Postprocessing reset parent pointers (done) */" << endl;

       // DQ (7/19/2005): Moved to after parent pointer fixup!        
       // subTemporaryAstFixes(node);
          removeInitializedNamePtr(node);

       // DQ (3/17/2007): This should be empty
          ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // DQ (12/1/2004): This should be done before the reset of template names (since that operation requires valid scopes!)
       // DQ (11/29/2004): Added to support new explicit scope information on IR nodes
       // initializeExplicitScopeData(node);
          initializeExplicitScopes(node);

       // DQ (5/28/2006): Fixup names in declarations that are inconsistent (e.g. where more than one non-defining declaration exists)
          resetNamesInAST();

       // DQ (3/17/2007): This should be empty
          ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // Output progress comments for these relatively expensive operations on the AST
          if ( SgProject::get_verbose() >= AST_POST_PROCESSING_VERBOSE_LEVEL )
               cout << "/* AST Postprocessing reset template names */" << endl;

       // DQ (5/15/2011): This causes template names to be computed as strings and and without name qualification 
       // if we don't call the name qualification before here. Or we reset the template names after we do the 
       // analysis to support the name qualification.
       // reset the names of template class declarations
          resetTemplateNames(node);

       // DQ (2/12/2012): This is a problem for test2004_35.C (debugging this issue).
       // printf ("Exiting after calling resetTemplateNames() \n");
       // ROSE_ASSERT(false);

       // DQ (3/17/2007): This should be empty
          ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // Output progress comments for these relatively expensive operations on the AST
          if ( SgProject::get_verbose() >= AST_POST_PROCESSING_VERBOSE_LEVEL )
               cout << "/* AST Postprocessing reset template names (done) */" << endl;

       // DQ (6/26/2007): Enum values used before they are defined in a class can have NULL declaration pointers.  This
       // fixup handles this case and traverses just the SgEnumVal objects.
          fixupEnumValues();

       // DQ (4/7/2010): This was commented out to modify Fortran code, but I think it should NOT modify Fortran code.
       // DQ (5/21/2008): This only make since for C and C++ (Error, this DOES apply to Fortran where the "parameter" attribute is used!)
          if (SageInterface::is_Fortran_language() == false && SageInterface::is_Java_language() == false)
             {
            // DQ (3/20/2005): Fixup AST so that GNU g++ compile-able code will be generated
               fixupInClassDataInitialization(node);
             }

       // DQ (3/24/2005): Fixup AST to generate code that works around GNU g++ bugs
          fixupforGnuBackendCompiler(node);

       // DQ (4/19/2005): fixup all definingDeclaration and NondefiningDeclaration pointers in SgDeclarationStatement IR nodes
       // fixupDeclarations(node);

       // DQ (5/20/2005): make the non-defining (forward) declarations added by EDG for static template 
       // specializations added under the "--instantiation local" option match the defining declarations.
          fixupStorageAccessOfForwardTemplateDeclarations(node);


       // DQ (6/21/2005): This function now only marks the subtrees of all appropriate declarations as compiler generated.
       // DQ (5/27/2005): mark all template instantiations (which we generate as template specializations) as compiler generated.
       // This is required to make them pass the unparser and the phase where comments are attached.  Some fixup of filenames
       // and line numbers might also be required.
          fixupTemplateInstantiations(node);

       // DQ (8/19/2005): Mark any template specialization (C++ specializations are template instantiations 
       // that are explicit in the source code).  Such template specializations are marked for output only
       // if they are present in the source file.  This detail could effect handling of header files later on.
       // Have this phase preceed the markTemplateInstantiationsForOutput() since all specializations should 
       // be searched for uses of (references to) instantiated template functions and member functions.
          markTemplateSpecializationsForOutput(node);

       // DQ (6/21/2005): This function marks template declarations for output by the unparser (it is part of a 
       // fixed point iteration over the AST to force find all templates that are required (EDG at the moment 
       // outputs only though template functions that are required, but this function solves the more general 
       // problem of instantiation of both function and member function templates (and static data, later)).
          markTemplateInstantiationsForOutput(node);

       // DQ (3/17/2007): This should be empty
          ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // DQ (3/16/2006): fixup any newly added declarations (see if we can eliminate the first place where this is called, above)
       // fixup all definingDeclaration and NondefiningDeclaration pointers in SgDeclarationStatement IR nodes
       // driscoll6 (6/10/11): this traversal sets p_firstNondefiningDeclaration for defining declarations, which
       // causes justifiable failures in AstConsistencyTests. Until this is resolved, skip this test for Python.
          if (SageInterface::is_Python_language()) {
              //cerr << "warning: python. Skipping fixupDeclarations() in astPostProcessing.C" << endl;
          } else {
              fixupDeclarations(node);
          }

       // DQ (3/17/2007): This should be empty
          ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // DQ (2/12/2006): Moved to trail marking templates (as a test)
       // DQ (6/27/2005): fixup the defining and non-defining declarations referenced at each SgDeclarationStatement
       // This is a more sophisticated fixup than that done by fixupDeclarations.
          fixupAstDefiningAndNondefiningDeclarations(node);

       // DQ (10/21/2007): Friend template functions were previously not properly marked which caused their generated template 
       // symbols to be added to the wrong symbol tables.  This is a cause of numerous symbol table problems.
          fixupFriendTemplateDeclarations();

       // DQ (3/17/2007): This should be the last point at which the globalMangledNameMap is empty
       // The fixupAstSymbolTables will generate calls to function types that will be placed into 
       // the globalMangledNameMap.
          ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // DQ (6/26/2005): The global function type symbol table should be rebuilt (since the names of templates 
       // used in qualified names of types have been reset (in post processing).  Other local symbol tables should
       // be initalized and constructed for any empty scopes (for consistancy, we want all scopes to have a valid 
       // symbol table pointer).
          fixupAstSymbolTables(node);

       // DQ (3/17/2007): At this point the globalMangledNameMap has been used in the symbol table construction. OK.
       // ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // DQ (8/20/2005): Handle backend vendor specific template handling options 
       // (e.g. g++ options: -fno-implicit-templates and -fno-implicit-inline-templates)
          processTemplateHandlingOptions(node);

       // DQ (5/22/2005): relocate compiler generated forward template instantiation declarations to appear 
       // after the template declarations and before first use.
       // relocateCompilerGeneratedTemplateInstantiationDeclarationsInAST(node);

       // DQ (8/27/2005): This disables output of some template instantiations that would result in 
       // "ambiguous template specialization" in g++ (version 3.3.x, 3.4.x, and 4.x).  See test2005_150.C 
       // for more detail.
          markOverloadedTemplateInstantiations(node);

       // DQ (9/5/2005): Need to mark all nodes in any subtree marked as a transformation
          markTransformationsForOutput(node);

       // DQ (3/5/2006): Mark functions that are provided for backend compatability as compiler generated by ROSE
          markBackendSpecificFunctionsAsCompilerGenerated(node);

       // DQ (5/24/2006): Added this test to figure out where Symbol parent pointers are being reset to NULL
       // TestParentPointersOfSymbols::test();

       // DQ (5/24/2006): reset the remaining parents in IR nodes missed by the AST based traversals
       // resetParentPointersInMemoryPool();
          resetParentPointersInMemoryPool(node);

       // DQ (3/17/2007): This should be empty
       // ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // DQ (5/29/2006): Fixup types in declarations that are not shared (e.g. where more than one non-defining declaration exists)
          resetTypesInAST();

       // DQ (3/17/2007): This should be empty
       // ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

       // DQ (3/10/2007): fixup name of any template classes that have been copied incorrectly into SgInitializedName 
       // list in base class constructor preinitialization lists (see test2004_156.C for an example).
          resetContructorInitilizerLists();

       // DQ (10/27/2007): Setup any endOfConstruct Sg_File_Info objects (report on where they occur)
          fixupSourcePositionConstructs();

       // DQ (1/19/2008): This can be called at nearly any point in the ast fixup.
          markLhsValues(node);

#ifndef ROSE_USE_CLANG_FRONTEND
       // DQ (2/21/2010): This normalizes an EDG trick (well documented) that replaces "__PRETTY_FUNCTION__" variable 
       // references with variable given the name of the function where the "__PRETTY_FUNCTION__" variable references 
       // was found. This is only seen when compiling ROSE using ROSE and was a mysterious property of ROSE for a long 
       // time until it was identified.  This fixup traversal changes the name back to "__PRETTY_FUNCTION__" to make
       // the code generated using ROSE when compiling ROSE source code the same as if GNU processed it (e.g. using CPP).
          if (SageInterface::is_Java_language() == false) {
              fixupPrettyFunctionVariables(node);
          }
#endif

       // DQ (11/24/2007): Support for Fortran resolution of array vs. function references.
          if (SageInterface::is_Fortran_language() == true)
             {
            // I think this is not used since I can always figure out if something is an 
            // array reference or a function call.
               fixupFortranReferences(node);

            // DQ (10/3/2008): This bug in OFP is now fixed so no fixup is required.
            // This is the most reliable way to introduce the Fortran "contains" statement.
            // insertFortranContainsStatement(node);
             }

       // DQ (9/26/2008): fixup the handling of use declarations (SgUseStatement).
       // This also will fixup C++ using declarations.
          fixupFortranUseDeclarations(node);

       // DQ (4/14/2010): Added support for symbol aliases for C++
       // This is the support for C++ "using declarations" which uses symbol aliases in the symbol table to provide 
       // correct visability of symbols included from alternative scopes (e.g. namespaces).
          fixupAstSymbolTablesToSupportAliasedSymbols(node);

       // DQ (6/24/2010): To support merge, we want to normalize the typedef lists for each type so that 
       // the names of the types will evaluate to be the same (and merge appropriately).
          normalizeTypedefSequenceLists();

       // Make sure that compiler-generated AST nodes are marked for Sg_File_Info::isCompilerGenerated().
          checkIsCompilerGeneratedFlag(node);

       // DQ (4/16/2015): This is replaced with a better implementation.
       // DQ (5/22/2005): Nearly all AST fixup should be done before this closing step
       // QY: check the isModified flag
       // CheckIsModifiedFlagSupport(node); 
       // checkIsModifiedFlag(node);
          unsetNodesMarkedAsModified(node);

       // This is used for both of the fillowing tests.
          SgSourceFile* sourceFile = isSgSourceFile(node);

       // DQ (9/11/2009): Added support for numbering of statements required to support name qualification.
          if (sourceFile != NULL)
             {
            // DQ (9/11/2009): Added support for numbering of statements required to support name qualification.
            // sourceFile->buildStatementNumbering();
               SgGlobal* globalScope = sourceFile->get_globalScope();
               ROSE_ASSERT(globalScope != NULL);
               globalScope->buildStatementNumbering();
             }
            else 
             {
               if (SgProject* project = isSgProject(node))
                  {
                    SgFilePtrList &files = project->get_fileList();
                    for (SgFilePtrList::iterator fileI = files.begin(); fileI != files.end(); ++fileI)
                       {
                         if ( (sourceFile = isSgSourceFile(*fileI)) )
                            {
                              SgGlobal* globalScope = sourceFile->get_globalScope();
                              ROSE_ASSERT(globalScope != NULL);
                              globalScope->buildStatementNumbering();
                            }
                       }
                  }
             }

       // DQ (4/4/2010): check that the global scope has statements.
       // This was an error for Fortran and it appeared that everything
       // was working when it was not.  It appeared because of a strange
       // error between versions of the OFP support files.  So as a 
       // way to avoid this in the future, we issue a warning for Fortran
       // code that has no statements in the global scope.  It can still
       // be a valid Fortran code (containing only comments).  but this
       // should help avoid our test codes appearing to work when they 
       // don't (in the future). For C/C++ files there should always be
       // something in the global scope (because or ROSE defined functions), 
       // so this test should not be a problem.
          if (sourceFile != NULL)
             {
               SgGlobal* globalScope = sourceFile->get_globalScope();
               ROSE_ASSERT(globalScope != NULL);
               if (globalScope->get_declarations().empty() == true)
                  {
                 // DQ (3/17/2017): Added support to use message streams.
                    mprintf ("WARNING: no statements in global scope for file = %s \n",sourceFile->getFileName().c_str());
                  }
             }
            else 
             {
               if (SgProject* project = isSgProject(node))
                  {
                    SgFilePtrList &files = project->get_fileList();
                    for (SgFilePtrList::iterator fileI = files.begin(); fileI != files.end(); ++fileI)
                       {
                         if ( (sourceFile = isSgSourceFile(*fileI)) )
                            {
                              SgGlobal* globalScope = sourceFile->get_globalScope();
                              ROSE_ASSERT(globalScope != NULL);
                              if (globalScope->get_declarations().empty() == true)
                                 {
                                // DQ (3/17/2017): Added support to use message streams.
                                   mprintf ("WARNING: no statements in global scope for file = %s \n",(*fileI)->getFileName().c_str());
                                 }
                            }
                       }
                  }
             }
        }
   }

