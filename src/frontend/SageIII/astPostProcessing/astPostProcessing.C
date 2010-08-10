// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AstFixup.h"
#include "astPostProcessing.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// tps (01/14/2009): Had to define this locally as it is not part of sage3 but rose.h
#include "AstDiagnostics.h"

// DQ (8/20/2005): Make this local so that it can't be called externally!
void postProcessingSupport (SgNode* node);

// DQ (5/22/2005): Added function with better name, since none of the fixes are really
// temporary any more.
void AstPostProcessing (SgNode* node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST post-processing:");

  // printf ("Inside of AstPostProcessing(node = %p) \n",node);

  // DQ (3/17/2007): This should be empty
     if (SgNode::get_globalMangledNameMap().size() != 0)
        {
          if (SgProject::get_verbose() > 0)
             {
               printf("AstPostProcessing(): found a node with globalMangledNameMap size not equal to 0: SgNode = %s =%s ", node->sage_class_name(),SageInterface::get_name(node).c_str());
               printf ("SgNode::get_globalMangledNameMap().size() != 0 size = %zu (clearing mangled name cache) \n",SgNode::get_globalMangledNameMap().size());
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

       // Test for a binary executable, object file, etc.
          case V_SgBinaryComposite:
             {
               SgBinaryComposite* file = isSgBinaryComposite(node);
               ROSE_ASSERT(file != NULL);

               printf ("AstPostProcessing of SgBinaryFile \n");
               ROSE_ASSERT(false);

               break;
             }

          default:
             {
            // list general post-processing fixup here ...
               postProcessingSupport (node);
             }
        }

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

  // JJW (12/5/2008): Turn off C and C++ postprocessing steps when the new EDG
  // interface is being used (it should produce correct, complete ASTs on its
  // own and do its own fixups)
#ifdef ROSE_USE_NEW_EDG_INTERFACE

  // Only do AST post-processing for C/C++
     bool doPostprocessing = (SageInterface::is_Fortran_language() == true) || (SageInterface::is_PHP_language() == true);

  // If this is C or C++ then we are using the new EDG translation and althrough fewer 
  // fixups should be required, some are still required.
     if (doPostprocessing == false)
        {
          printf ("Postprocessing AST build using new EDG/Sage Translation Interface. \n");

       // Reset and test and parent pointers so that it matches our definition 
       // of the AST (as defined by the AST traversal mechanism).
          topLevelResetParentPointer (node);

       // Another 2nd step to make sure that parents of even IR nodes not traversed 
       // can be set properly.
          resetParentPointersInMemoryPool();

       // DQ (6/27/2005): fixup the defining and non-defining declarations referenced at each SgDeclarationStatement
       // This is a more sophisticated fixup than that done by fixupDeclarations. See test2009_09.C for an example
       // of a non-defining declaration appearing before a defining declaration and requiring a fixup of the
       // non-defining declaration reference to the defining declaration.
          fixupAstDefiningAndNondefiningDeclarations(node);

       // Fixup the symbol tables (in each scope) and the global function type 
       // symbol table. This is less important for C, but required for C++.
       // But since the new EDG interface has to handle C and C++ we don't
       // setup the global function type table there to be uniform.
          fixupAstSymbolTables(node);

       // DQ (4/14/2010): Added support for symbol aliases for C++
       // This is the support for C++ "using declarations" which uses symbol aliases in the symbol table to provide 
       // correct visability of symbols included from alternative scopes (e.g. namespaces).
          fixupAstSymbolTablesToSupportAliasedSymbols(node);

       // This resets the isModified flag on each IR node so that we can record 
       // where transformations are done in the AST.  If any transformations on
       // the AST are done, even just building it, this step should be the final
       // step.
          checkIsModifiedFlag(node);

          printf ("DONE: Postprocessing AST build using new EDG/Sage Translation Interface. \n");

          return;
        }
#endif // ROSE_USE_NEW_EDG_INTERFACE -- do postprocessing unconditionally when the old EDG interface is used

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
  // TimingPerformance timer ("AST Fixup: time (sec) = ");

     if ( SgProject::get_verbose() >= AST_POST_PROCESSING_VERBOSE_LEVEL )
          cout << "/* AST Postprocessing started. */" << endl;

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
     resetParentPointersInMemoryPool();

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

  // reset the names of template class declarations
     resetTemplateNames(node);

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
     if (SageInterface::is_Fortran_language() == false)
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

#if 0
  // DQ (6/27/2005): fixup the defining and non-defining declarations referenced at each SgDeclarationStatement
     fixupAstDefiningAndNondefiningDeclarations(node);
#endif

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
     fixupDeclarations(node);

  // DQ (3/17/2007): This should be empty
     ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

  // DQ (2/12/2006): Moved to trail marking templates (as a test)
  // DQ (6/27/2005): fixup the defining and non-defining declarations referenced at each SgDeclarationStatement
  // This is a more sophisticated fixup than that done by fixupDeclarations.
     fixupAstDefiningAndNondefiningDeclarations(node);

#if 0
     ROSE_ASSERT(saved_declaration != NULL);
     printf ("saved_declaration = %p saved_declaration->get_definingDeclaration() = %p saved_declaration->get_firstNondefiningDeclaration() = %p \n",
          saved_declaration,saved_declaration->get_definingDeclaration(),saved_declaration->get_firstNondefiningDeclaration());
     ROSE_ASSERT(saved_declaration->get_definingDeclaration() != saved_declaration->get_firstNondefiningDeclaration());     
#endif

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
#if 1
     markBackendSpecificFunctionsAsCompilerGenerated(node);
#else
     printf ("Warning: Skipped marking of backend specific functions ... \n");
#endif

  // DQ (5/24/2006): Added this test to figure out where Symbol parent pointers are being reset to NULL
  // TestParentPointersOfSymbols::test();

  // DQ (5/24/2006): reset the remaining parents in IR nodes missed by the AST based traversals
     resetParentPointersInMemoryPool();

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

  // DQ (2/21/2010): This normalizes an EDG trick (well documented) that replaces "__PRETTY_FUNCTION__" variable 
  // references with variable given the name of the function where the "__PRETTY_FUNCTION__" variable references 
  // was found. This is only seen when compiling ROSE using ROSE and was a mysterious property of ROSE for a long 
  // time until it was identified.  This fixup traversal changes the name back to "__PRETTY_FUNCTION__" to make
  // the code generated using ROSE when compiling ROSE source code the same as if GNU processed it (e.g. using CPP).
     fixupPrettyFunctionVariables(node);

#if 0
  // DQ (1/22/2008): Use this for the Fortran code to get more accurate source position information.

  // DQ (4/16/2007): comment out to test how function declaration prototypes are reset or set wrong.
  // DQ (11/1/2006): fixup source code position information for AST IR nodes.
     fixupSourcePositionInformation(node);
#endif

#if 0
  // DQ (11/10/2007): Moved computation of hidden list from astPostProcessing.C to unparseFile so that 
  // it will be called AFTER any transformations and immediately before code generation where it is 
  // really required.  This part of a fix for Liao's outliner, but should be useful for numerous 
  // transformations.
  // DQ (8/6/2007): Only compute the hidden lists if working with C++ code!
     if (SageInterface::is_Cxx_language() == true)
        {
       // DQ (5/22/2007): Moved from SgProject::parse() function to here so that propagateHiddenListData() could be called afterward.
       // DQ (5/8/2007): Now build the hidden lists for types and declarations (Robert Preissl's work)
          Hidden_List_Computation::buildHiddenTypeAndDeclarationLists(node);

       // DQ (6/5/2007): We actually need this now since the hidden lists are not pushed to lower scopes where they are required.
       // DQ (5/22/2007): Added support for passing hidden list information about types, declarations and elaborated types to child scopes.
          propagateHiddenListData(node);
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

#if 0
     ROSE_MemoryUsage memoryUsage1;
     printf ("Test 1: memory_usage = %f \n",memoryUsage1.getMemoryUsageMegabytes());
#endif

  // DQ (4/14/2010): Added support for symbol aliases for C++
  // This is the support for C++ "using declarations" which uses symbol aliases in the symbol table to provide 
  // correct visability of symbols included from alternative scopes (e.g. namespaces).
     fixupAstSymbolTablesToSupportAliasedSymbols(node);

#if 0
     ROSE_MemoryUsage memoryUsage2;
     printf ("Test 2: memory_usage = %f \n",memoryUsage2.getMemoryUsageMegabytes());
#endif

  // DQ (6/24/2010): To support merge, we want to normalize the typedef lists for each type so that 
  // the names of the types will evaluate to be the same (and merge appropriately).
     normalizeTypedefSequenceLists();

#if 0
     ROSE_MemoryUsage memoryUsage3;
     printf ("Test 3: memory_usage = %f \n",memoryUsage3.getMemoryUsageMegabytes());
#endif

  // DQ (3/7/2010): Identify the fragments of the AST that are disconnected.
  // Moved from astConsistancy tests (since it deletes nodes not connected to the AST).
  // TestForDisconnectedAST::test(node);

  // DQ (5/22/2005): Nearly all AST fixup should be done before this closing step
  // QY: check the isModified flag
  // CheckIsModifiedFlagSupport(node); 
     checkIsModifiedFlag(node);

#if 0
     ROSE_MemoryUsage memoryUsage4;
     printf ("Test 4: memory_usage = %f \n",memoryUsage4.getMemoryUsageMegabytes());
#endif

  // ROSE_ASSERT(saved_declaration->get_definingDeclaration() != saved_declaration->get_firstNondefiningDeclaration());

  // DQ (3/17/2007): This should be empty
  // ROSE_ASSERT(SgNode::get_globalMangledNameMap().size() == 0);

  // This is used for both of the fillowing tests.
     SgSourceFile* sourceFile = isSgSourceFile(node);

#if 1
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
#endif

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
               printf ("WARNING: no statements in global scope for file = %s \n",sourceFile->getFileName().c_str());
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
                              printf ("WARNING: no statements in global scope for file = %s \n",(*fileI)->getFileName().c_str());
                            }
                       }
                  }
             }
        }

     if ( SgProject::get_verbose() >= AST_POST_PROCESSING_VERBOSE_LEVEL )
        cout << "/* AST Postprocessing finished */" << endl;



  // DQ (5/3/2010): Added support for binary analysis specific post-processing.
     SgProject* project = isSgProject(node);
     if (project != NULL)
        {
#if 0
       // printf ("In postProcessingSupport(): project->get_exit_after_parser() = %s \n",project->get_exit_after_parser() ? "true" : "false");
       // printf ("In postProcessingSupport(): project->get_binary_only()       = %s \n",project->get_binary_only() ? "true" : "false");
          if (project->get_binary_only() == true)
             {
               printf ("Inside of postProcessingSupport(): Processing binary project \n");
            // ROSE_ASSERT(false);

            // addEdgesInAST();
             }
#endif
        }

   }
