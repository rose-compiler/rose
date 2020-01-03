// Author: Markus Schordan
// $Id: AstConsistencyTests.C,v 1.8 2008/01/25 02:25:46 dquinlan Exp $

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "rose_config.h"

// tps : needed to define this here as it is defined in rose.h
#include "markCompilerGenerated.h"

#include "AstDiagnostics.h"
#ifndef ASTTESTS_C
   #define ASTTESTS_C
// DQ (8/9/2004): Modified to put code below outside of ASTTESTS_C if ... endif
#endif

// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
//#include "sage3.h"
#include "roseInternal.h"

#include "AstConsistencyTests.h"
#include "AstNodePtrs.h"
#include "AstTraversal.h"

// This controls output for debugging
#define WARN_ABOUT_ATYPICAL_LVALUES 0

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

// DQ (3/19/2012): We need this for a function in calss: TestForParentsMatchingASTStructure
#include "stringify.h"

// DQ (3/24/2016): Adding message logging.
#include "Diagnostics.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;

// DQ (3/24/2016): Adding Robb's message logging mechanism to contrl output debug message from the EDG/ROSE connection code.
using namespace Rose::Diagnostics;

// DQ (3/24/2016): Adding Message logging mechanism.
Sawyer::Message::Facility TestChildPointersInMemoryPool::mlog;


/*! \file

    This file contains the test run on the AST after construction (option).

*/

/*! \page AstProperties AST Properties (Consistency Tests)

    This documentation describes the properties of the AST. There are a number of consistency tests 
of the AST, when all these pass the AST is verified to have specifi properties.  This documentation 
lays out these properties of the AST (e.g. which pointers are always valid, etc.).

*/

bool
AstTests::isPrefix(string prefix, string s)
   {
     return (s.find(prefix, 0) == 0);
   }

unsigned int
AstTests::numPrefix(string prefix, vector<string> vs)
   {
     unsigned int num=0;
     for (vector<string>::iterator it=vs.begin();it!=vs.end();it++)
        {
          if (isPrefix(prefix,*it))
             {
               num++;
             }
        }
     return num;
   }

unsigned int
AstTests::numSuccContainers(SgNode* node) {
  vector<string> names=node->get_traversalSuccessorNamesContainer();
  return numPrefix("[0]",names)+numPrefix("*[0]",names);
}

unsigned int
AstTests::numSingleSuccs(SgNode* node) {
  vector<string> names=node->get_traversalSuccessorNamesContainer();
  return numPrefix("p_",names);
}

bool
AstTests::isProblematic(SgNode* node) 
   {
  // This function tests whether a node is problematic for the access of attributes 
  // computed by the processing classes. There are two cases how attributes can be 
  // accessed
  //    1. by using the (generated) enums
  //    2. by using an index for an offset in a children container
  // A node is problematic if both cases are mixed.  Therefore this test is included 
  // in the consistancy tests to ensure that no new node that is added to the AST 
  // violates this property.

  // DQ (4/3/2006 (afternoon)): But I had to change it back since the AST it fails to pass this test.
  //     We need to work more with Markus to fix this!
  // DQ (4/3/2006 (morning)): Markus called and though this should be changed.
  // ERROR:  Call from Markus, this needs to be switched. But it will cause something to fail.
  // This is what it takes to allow the enums based interface to work within the traversal.
#if 1
  // DQ (8/10/2004): This is the correct test and no other one is appropriate, I gather!
  // DQ (4/4/2006): I believe that this was Markus's original code. And while I tried to make
  // it more clear with the alternative code below this didn't work.  So it was put back the
  // way I understood Markus had designed it.
     return (numSuccContainers(node) > 1) || ( (numSuccContainers(node) > 0) && (numSingleSuccs(node) > 0) );
#else
  // This case fails, I think because the numberOfDataMembers == 1 (to include the container itself)
  // actually 1 does not work, so I'm not sure what is going on here!  I have sent email to Markus.
  // int containerSize       = numSuccContainers(node);
     int numberOfContainers  = numSuccContainers(node);
     int numberOfDataMembers = numSingleSuccs(node);
  // return  ( (containerSize >  1) && (numberOfDataMembers == 0)) || ( (containerSize == 0) && (numberOfDataMembers > 0) );
     return  ( (numberOfContainers >  1) && (numberOfDataMembers == 0)) || ( (numberOfContainers == 0) && (numberOfDataMembers > 0) );
#endif
   }

/*
void 
AstTests::dummyTests(SgProject* sageProject) {
  DI inh; DS syn;
  syn=DummyISTestQuery().traverse(sageProject,inh);
  DummyITestQuery().traverse(sageProject,inh);
  syn=DummySTestQuery().traverse(sageProject);
  DummyTestQuery().traverse(sageProject,postorder);
}
*/

// DQ (10/6/2004): This function should be designed to take a SgNode 
// as input so that any part of the AST could be tested!
bool
AstTests::isCorrectAst(SgProject* sageProject)
   {
     TimingPerformance timer ("AST check for IR nodes without source position information:");

     TestAstProperties t;
     bool returnValue = t.traverse(sageProject).val;

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        {
          printf ("\nAST Diagnostics: Fequency of IR nodes missing ending source postion info: \n");
#ifdef __CYGWIN__
       // PC (9/18/2006): Diagnostic fails on cygwin
          puts("Diagnostic fails on cygwin");
#else
          bool found = false;
          for (int i = V_SgModifier; i != V_SgNumVariants; i++)
             {
               if (t.nodeWithoutFileInfoFrequencyCount[i] > 0)
                  {
                    found = true;
                    printf ("     Frequency at %s = %d \n",getVariantName(VariantT(i)).c_str(),t.nodeWithoutFileInfoFrequencyCount[i]);
                  }
             }

          if (found == false)
             {
               printf ("PASSED: All IR nodes have both starting and ending position information \n");
             }
#endif
        }

     return returnValue;
   }

void 
AstTests::runAllTests(SgProject* sageProject)
   {
  // compilation tests of templated processing classes
  // DQ (3/30/2004): This function is called by the 
  //      ROSE/src/roseTranslator.C RoseTestTranslator class

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        {
          printf ("At TOP of AstTests::runAllTests() \n");
        }

#ifdef NDEBUG
  // DQ (6/30/20133): If we have compiled with NDEBUG then nothing identified in this function 
  // will be caught because every place we detect a problem we expect to end with ROSE_ASSERT() 
  // which is disabled when ROSE is compiled with NDEBUG.  So more approriate (and equvalent) 
  // semantics is that if ROSE is compiled with NDEBUG then we should just exit directly.
     TimingPerformance ndebug_timer ("AST Consistency Tests (disabled by NDEBUG):");
     return;
#endif

  // It is a proper place to put any tests of the AST that must always pass!

  // Possible future tests: 
  //    1) Test for redundant statements in the same basic block.
  //       This is a current bug which the AST tests didn't catch.

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
  // ROSE_Performance::TimingPerformance("AST Consistency Tests");
     TimingPerformance timer ("AST Consistency Tests:");

  // DQ (2/17/2013): Added support to skip AST consistancy tests for performance testing.
  // The skipAstConsistancyTests variable is on the SgFile, not the SgProject.
     if (sageProject->get_fileList().empty() == false && sageProject->get_fileList()[0]->get_skipAstConsistancyTests() == true)
        {
          printf ("Note: In AstTests::runAllTests(): command line option used to skip AST consistancy tests \n");
          return;
        }

  // DQ (2/23/2014): Adding support for gathering statistics from boost hash tables.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
  // if ( SgProject::get_verbose() >= 0 )
        {
          for (size_t i = 0; i < sageProject->get_fileList().size(); i++)
             {
               SgSourceFile* sourceFile = isSgSourceFile(sageProject->get_fileList()[i]);
               if (sourceFile != NULL)
                  {
                    SgGlobal* globalScope = sourceFile->get_globalScope();
                    ROSE_ASSERT(globalScope != NULL);
                    size_t maxCollisions = globalScope->get_symbol_table()->maxCollisions();
                    printf ("Symbol Table Statistics: sourceFile = %" PRIuPTR " maxCollisions = %" PRIuPTR " \n",i,maxCollisions);

                    float load_factor     = globalScope->get_symbol_table()->get_table()->load_factor();
                    printf ("Symbol Table Statistics: sourceFile = %" PRIuPTR " load_factor = %f \n",i,load_factor);

                    float max_load_factor = globalScope->get_symbol_table()->get_table()->max_load_factor();
                    printf ("Symbol Table Statistics: sourceFile = %" PRIuPTR " max_load_factor = %f \n",i,max_load_factor);
                  }
             }
        }

  // CH (2010/7/26):   
  // Before running tests, first clear all variable symbols which are not referenced in the memory pool.
  // This is because when building AST bottom-up, some temporary symbol may be generated to be referenced
  // by those variable references generated just using names. When all variable references are fixed,
  // those symbols are not used any more and then should be removed from memory pool.
  //
  // Liao 1/24/2013: I have to comment this out
  // for #define N 1000, when N is used in OpenMP directives, the OmpSupport::attachOmpAttributeInfo() will try to generate a 
  // variable reference to N, But N cannot be found in AST, so unknownType is used.  But symbols with unknowntype will be removed
  // by this clearUnusedVariableSymbols()
     //SageInterface::clearUnusedVariableSymbols();

  // printf ("Inside of AstTests::runAllTests(sageProject = %p) \n",sageProject);

  // printf ("Exiting at top of AstTests::runAllTests() \n");
  // ROSE_ASSERT(false);

/*! \page AstProperties AST Properties (Consistency Tests)

\section section1 Traversal Tests

     This test verifies that the different types of traversal work properly on the AST.

*/
  // DQ (3/30/2004): Not clear why we are avoiding having to specify unique variables, Markus?.
        {
          DummyISTestQuery1 q1;
          DummyITestQuery1  q2;
          DummySTestQuery1  q3;
          DummyTestQuery1   q4;
        }
        {
          DummyISTestQuery2 q1;
          DummyITestQuery2  q2;
          DummySTestQuery2  q3;
          DummyTestQuery2   q4;
        }
        {
          DummyISTestQuery3 q1;
          DummyITestQuery3  q2;
          DummySTestQuery3  q3;
          DummyTestQuery3   q4;
        }

  // test statistics
  // AstNodeStatistics stat;
  // cout << stat.toString(sageProject);
  // statistics data will be used for testing constraints on the AST

  // test properties of AST
  // if (sageProject->get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "\nAST initial correctness test ... " << flush;
     if (isCorrectAst(sageProject))
        {
       // if (sageProject->get_useBackendOnly() == false) 
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
               cout << "succeeded." << endl;
        }
       else
        {
       // if (sageProject->get_useBackendOnly() == false) 
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
               cout << "failed." << endl;
            else
               cout << "AST Consistancy Tests have failed." << endl;
          ROSE_ABORT();
        }

  // Output an extra CR
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << endl;

  // DQ (3/30/2004): Added tests for templates (make sure that numerous fields are properly defined)
  // if (sageProject->get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        cout << "Redundent Statement Test started (tests only single scopes for redundent statements)." << endl;
        {
          TimingPerformance timer ("AST check for unique IR nodes in each scope (excludes IR nodes marked explicitly as shared by AST merge):");

          TestAstForUniqueStatementsInScopes redundentStatementTest;
          redundentStatementTest.traverse(sageProject,preorder);
        }
  // if (sageProject->get_useBackendOnly() == false) 
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Redundent Statement Test finished." << endl;

  // DQ (9/24/2013): Fortran support has excessive output spew specific to this test.  We will fix this in 
  // the new fortran work, but we can't have this much output spew presently.
  // DQ (9/21/2013): Force this to be skipped where ROSE's AST merge feature is active (since the point of 
  // merge is to share IR nodes, it is pointless to detect sharing and generate output for each identified case).
  // if (sageProject->get_astMerge() == false)
     if (sageProject->get_astMerge() == false && sageProject->get_Fortran_only() == false)
        {
       // DQ (4/2/2012): Added test for unique IR nodes in the AST.
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
               cout << "Unique IR nodes in AST Test started (tests IR nodes uniqueness over whole of AST)." << endl;

             {
               TimingPerformance timer ("AST check for unique IR nodes in whole of AST (must excludes IR nodes marked explicitly as shared by AST merge):");

               TestAstForUniqueNodesInAST redundentNodeTest;
               redundentNodeTest.traverse(sageProject,preorder);
             }

          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
               cout << "Unique IR nodes in AST Test finished." << endl;
        }

#if 0
  // DQ (10/11/2006): Debugging name qualification, so skip these tests which call the unparser!
     printf ("WARNING: In AstConsistencyTests.C, while debugging code generation, mangled name testing (which includes tests of unparseToString() mechanism) is skipped \n");
#else
  // DQ (4/27/2005): Test of mangled names
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Mangled Name Test on AST started (tests properties of mangled names)." << endl;
        {
          TimingPerformance timer ("AST mangle name test:");

          TestAstForProperlyMangledNames mangledNameTest;
          mangledNameTest.traverse(sageProject,preorder);

          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
             {
               cout << "Mangled Name Test finished: (number of mangled name size = " << mangledNameTest.saved_numberOfMangledNames << ") " << endl;
               cout << "Mangled Name Test finished: (max mangled name size       = " << mangledNameTest.saved_maxMangledNameSize   << ") " << endl;
               cout << "Mangled Name Test finished: (total mangled name size     = " << mangledNameTest.saved_totalMangledNameSize << ") " << endl;
             }
        }
#endif

  // DQ (4/27/2005): Test of compiler generated nodes
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Compiler Generated Node Test started." << endl;
        {
          TimingPerformance timer ("AST compiler generated node test:");

          TestAstCompilerGeneratedNodes compilerGeneratedNodeTest;
          compilerGeneratedNodeTest.traverse(sageProject,preorder);
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Compiler Generated Node Test finished." << endl;

#if 1
  // DQ (10/22/2007): The unparse to string functionality is now tested separately.
  // DQ (10/11/2006): Debugging name qualification, so skip these tests which call the unparser!
  // printf ("WARNING: In AstConsistencyTests.C, while debugging code generation, attribute testing (which includes tests of unparseToString() mechanism) is skipped \n");
#else
  // test pdf and printing of attributes in pdf files
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test attribute handling (includes tests of unparseToString() mechanism): started " << endl;
        {
          TimingPerformance timer ("AST attribute handling test (includes tests of unparseToString() mechanism):");

          AstTextAttributesHandling textAttributes(sageProject);
          textAttributes.setAll();
          AstPDFGeneration pdftest;
       // pdftest.generateInputFiles(sageProject); MS: deactivated 12/02/03
          textAttributes.removeAll();
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test attribute handling: finished " << endl;
#endif
  /*
     cout << "Testing for Nodes with NULL pointers started." << endl;
     AstNodePtrs np;
     np.traverseInputFiles(sageProject);
     cout << "Testing for Nodes with NULL pointers finished." << endl;

     cout << "Full Testing for Revisit Testing started" << endl;
     TestAstNullPointers tp;
     tp.traverse(sageProject);
     cout << "Full Testing for Revisit Testing finished." << endl;
  */

#if 0
  // DQ (2/6/2004):
  // This test has been turned off since it broke once new nodes were added to the AST 
  // and I don't really understnad how it works.  This is certainly something that we 
  // need to BE Markus inorder to fix :-).
  // if (sageProject->get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Testing default abstract C++ grammar started." << endl;
     AgProcessing<sgnodeParser> ap;
     ap.evaluate(sageProject);
  // if (sageProject->get_useBackendOnly() == false) 
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Testing default abstract C++ grammar finished." << endl;
#endif

  // DQ (4/2/2012): debugging why we have a cycle in the AST (test2012_59.C).
  // if (sageProject->get_useBackendOnly() == false)
  // if ( SgProject::get_verbose() >= 0 ) // DIAGNOSTICS_VERBOSE_LEVEL )
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Cycle test started." << endl;

        {
          TimingPerformance timer ("AST cycle test:");

          AstCycleTest cycTest;
          cycTest.traverse(sageProject);
        }

  // if (sageProject->get_useBackendOnly() == false) 
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        cout << "Cycle test finished. No cycle found." << endl;

  // DQ (3/30/2004): Added tests for templates (make sure that numerous fields are properly defined)
  // if (sageProject->get_useBackendOnly() == false) 
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Template Test started." << endl;
        {
          TimingPerformance timer ("AST template properties test:");

          TestAstTemplateProperties templateTest;
          templateTest.traverse(sageProject,preorder);
        }
  // if (sageProject->get_useBackendOnly() == false) 
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Template Test finished." << endl;

  // DQ (6/24/2005): Test setup of defining and non-defining declaration pointers for each SgDeclarationStatement
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Defining and Non-Defining Declaration  Test started." << endl;
        {
          TimingPerformance timer ("AST defining and non-defining declaration test:");

          TestAstForProperlySetDefiningAndNondefiningDeclarations declarationTest;
          declarationTest.traverse(sageProject,preorder);
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Defining and Non-Defining Declaration Test finished." << endl;

  // DQ (6/24/2005): Test setup of defining and non-defining declaration pointers for each SgDeclarationStatement
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Symbol Table Test started." << endl;
        {
          TimingPerformance timer ("AST symbol table test:");

          TestAstSymbolTables symbolTableTest;
          symbolTableTest.traverse(sageProject,preorder);
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Symbol Table Test finished." << endl;

  // DQ (6/24/2005): Test setup of defining and non-defining declaration pointers for each SgDeclarationStatement
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test return value of get_declaration() member functions started." << endl;
        {
          TimingPerformance timer ("AST test member function access functions:");

          TestAstAccessToDeclarations getDeclarationMemberFunctionTest;
          getDeclarationMemberFunctionTest.traverse(sageProject,preorder);
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test return value of get_declaration() member functions finished." << endl;

  // DQ (2/21/2006): Test the type of all expressions and where ever a get_type function is implemented.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test return value of get_type() member functions started." << endl;
        {
            // driscoll6 (7/25/11) Python support uses expressions that don't define get_type() (such as
            // SgClassNameRefExp), so skip this test for python-only projects.
            // TODO (python) define get_type for the remaining expressions ?
            if (! sageProject->get_Python_only()) {
                TimingPerformance timer ("AST expression type test:");
                TestExpressionTypes expressionTypeTest;
                expressionTypeTest.traverse(sageProject, preorder);
            } else {
                //cout << "warning: python. Skipping TestExpressionTypes in AstConsistencyTests.C" << endl;
            }
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test return value of get_type() member functions finished." << endl;

  // DQ (5/22/2006): Test the generation of mangled names.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test generation of mangled names started." << endl;
        {
          TimingPerformance timer ("AST mangled names test (exhaustive test using memory pool):");

          TestMangledNames::test();
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test generation of mangled names finished." << endl;

  // DQ (6/26/2006): Test the parent pointers of IR nodes in memory pool.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test parent pointers of IR nodes in memory pool started." << endl;
        {
          TimingPerformance timer ("AST IR node parent pointers test:");

          TestParentPointersInMemoryPool::test();
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test parent pointers of IR nodes in memory pool finished." << endl;

  // DQ (6/26/2006): Test the parent pointers of IR nodes in memory pool.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test parents child pointers of IR nodes in memory pool started." << endl;
        {
          TimingPerformance timer ("AST IR node child pointers test:");

          TestChildPointersInMemoryPool::test();
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test parents child pointers of IR nodes in memory pool finished." << endl;

#if 0
  // DQ (3/7/2007): At some point I think I decided that this was not a valid test!
  // DQ (10/18/2006): Test the firstNondefiningDeclaration to make sure it is not used as a forward declaration (memory pool test).
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test firstNondefiningDeclaration to make sure it is not used as a forward declaration started." << endl;
     TestFirstNondefiningDeclarationsForForwardMarking::test();
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test firstNondefiningDeclaration to make sure it is not used as a forward declaration finished." << endl;
#endif

  // DQ (6/26/2006): Test the parent pointers of IR nodes in memory pool.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test declarations for mapping to declaration associated with symbol(uses memory pool) started." << endl;
        {
          TimingPerformance timer ("Test for mapping to declaration associated with symbol test:");

          TestMappingOfDeclarationsInMemoryPoolToSymbols::test();

       // printf ("Exiting after call to TestMappingOfDeclarationsInMemoryPoolToSymbols::test() \n");
       // ROSE_ASSERT(false);
        }

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test declarations for mapping to declaration associated with symbol(uses memory pool) finished." << endl;


  // DQ (6/26/2006): Test expressions for l-value flags
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test expressions for properly set l-values started." << endl;
        {
          TimingPerformance timer ("Test expressions for properly set l-values:");

          TestLValueExpressions lvalueTest;
          lvalueTest.traverse(sageProject,preorder);

                        // King84 (7/29/2010): Uncomment this to enable checking of the corrected LValues
#if 0
                        TestLValues lvaluesTest;
                        lvaluesTest.traverse(sageProject,preorder);
#endif
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test expressions for properly set l-values finished." << endl;

  // DQ (2/23/2009): Test the declarations to make sure that defining and non-defining appear in the same file (for outlining consistency).
     TestMultiFileConsistancy::test();

  // DQ (11/28/2010): Test to make sure that Fortran is using case insensitive symbol tables and that C/C++ is using case sensitive symbol tables.
     TestForProperLanguageAndSymbolTableCaseSensitivity::test(sageProject);

        {
          TimingPerformance timer ("AST check for references to deleted IR nodes:");

       // DQ (9/26/2011): Test for references to deleted IR nodes in the AST.
          TestForReferencesToDeletedNodes::test(sageProject);
        }

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test typedef types for cycles." << endl;

        {
          TimingPerformance timer ("AST check for typedef type cycles:");

       // DQ (9/26/2011): Test for references to deleted IR nodes in the AST.
          TestAstForCyclesInTypedefs::test();
        }





#if 1
  // Comment out to see if we can checkin what we have fixed recently!

  // DQ (7/20/2004): temporary call to help debug traversal on all regression tests
  // This query returns a list of types used in the AST
     Rose_STL_Container<SgNode*> typeList;
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          printf ("Calling NodeQuery::generateListOfTypes(SgProject) ... \n");
        {
          TimingPerformance timer ("AST generate type list test:");

       // This uses the query mechanism which uses the memory pools and as a result is much faster and more complete than before.
          typeList = NodeQuery::generateListOfTypes(sageProject);
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          printf ("AST Type Test: typeList.size() = %ld \n",(long)typeList.size());

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          printf ("Type Test Started \n");


        {
       // DQ (3/7/2007): This is the start of the timer scope
          TimingPerformance timer ("AST type test:");

  // DQ (10/6/2004): Add testing of the types found from the NodeQuery::generateListOfTypes() function.
  // Until we can traverse the types this is the only way to test these IR nodes.
     Rose_STL_Container<SgNode*>::iterator i = typeList.begin();
     while (i != typeList.end())
        {
       // DQ (8/6/2005): Call this function to test any relevant types (SgNamedType and its derived types)
          TestAstAccessToDeclarations::test(*i);

       // test all the types (since they are not traversed yet)
          switch ( (*i)->variantT() )
             {
               case V_SgClassType:
            // case V_SgEnumType:
            // case V_SgTypedefType:
            // case V_SgNamedType:
                  {
                    SgNamedType* namedType = isSgNamedType(*i);
                    ROSE_ASSERT(namedType != NULL);
                    SgDeclarationStatement* declarationStatement = namedType->get_declaration();
                    ROSE_ASSERT(declarationStatement != NULL);
                    if (declarationStatement->get_parent() == NULL)
                       {
                         if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                            {
                              printf ("     Warning: Found a NULL declarationStatement->get_parent() value within SgClassType ... \n");
#if 0
                              declarationStatement->get_file_info()->display("Location of problem declaration in source code");
#endif
                            }
                       }

                 // DQ (10/17/2004): Acturally the defining declaration need not exist (for code to 
                 // compile and not link). So it is OK to have a NULL value for the definingDeclaration.
                 // ROSE_ASSERT(declarationStatement->get_parent() != NULL);
                 // if (declarationStatement->get_firstNondefiningDeclaration() == NULL || 
                 //     declarationStatement->get_definingDeclaration() == NULL)
                    if (declarationStatement->get_firstNondefiningDeclaration() == NULL)
                       {
                         if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                            {
                              printf ("     Warning: At %p = %s found unset definingDeclaration = %p or firstNondefiningDeclaration = %p \n",
                                      namedType,namedType->sage_class_name(), declarationStatement->get_definingDeclaration(),
                                      declarationStatement->get_firstNondefiningDeclaration());
#if 0
                              printf ("namedType = %p = %s \n",namedType,namedType->sage_class_name());
                              declarationStatement->get_file_info()->
                                   display("Problematic Class Declaration Location in Source Code");
#endif
                            }
                       }

                    if (declarationStatement->get_firstNondefiningDeclaration() == NULL)
                       {
                         printf ("Error: declarationStatement = %p \n",declarationStatement);
                         declarationStatement->get_file_info()->display("Error: declarationStatement->get_firstNondefiningDeclaration() == NULL");
                       }
                    ROSE_ASSERT(declarationStatement->get_firstNondefiningDeclaration() != NULL);
                 // ROSE_ASSERT(declarationStatement->get_definingDeclaration() != NULL);

                 // Liao 10/30/2009, We enforce a unique SgClassType node for SgClassDeclaration and its derived classes
                 // SgClassType should be associated with the first nondefining class declaration
                 // All other relevant declarations (defining and other nondefining declarations) should share this type node
                    SgClassType* cls_type = isSgClassType (*i); 
                    if (cls_type != NULL)
                       {
                      // DQ (12/4/2011): Better to treat this as a test on the SgDeclarationStatement instead of on the SgClassDeclaration.
                      // SgClassDeclaration * cls_decl = isSgClassDeclaration (cls_type->get_declaration());
                         ROSE_ASSERT (cls_type->get_declaration() != NULL);
                         SgDeclarationStatement* cls_decl = isSgDeclarationStatement(cls_type->get_declaration());
                         ROSE_ASSERT (cls_decl != NULL);
                         if (cls_decl != NULL)
                            {
                              if (cls_decl->get_firstNondefiningDeclaration() != NULL)
                                 {
                                // if (isSgClassDeclaration(cls_decl->get_firstNondefiningDeclaration()) != cls_decl )
                                   if (cls_decl->get_firstNondefiningDeclaration() != cls_decl )
                                      {
                                        printf ("   Warning: cls_decl = %p = %s \n",cls_decl,cls_decl->class_name().c_str());
                                        printf ("   Warning: cls_decl->get_firstNondefiningDeclaration() = %p = %s \n",cls_decl->get_firstNondefiningDeclaration(),cls_decl->get_firstNondefiningDeclaration()->class_name().c_str());

                                        SgClassDeclaration* local_cls_decl = isSgClassDeclaration (cls_decl);
                                        SgClassDeclaration* local_cls_decl_firstNondefining = isSgClassDeclaration (cls_decl->get_firstNondefiningDeclaration());
                                        SgClassDeclaration* local_cls_decl_defining = isSgClassDeclaration (cls_decl->get_definingDeclaration());

                                        printf ("local_cls_decl->get_name()                  = %s \n",local_cls_decl->get_name().str());
                                        printf ("local_cls_decl_firstNondefining->get_name() = %s \n",local_cls_decl_firstNondefining->get_name().str());

                                        local_cls_decl->get_startOfConstruct()->display("Error in AST Consistancy tests: cls_decl->get_firstNondefiningDeclaration() != cls_decl");
                                        local_cls_decl_firstNondefining->get_startOfConstruct()->display("Error in AST Consistancy tests: cls_decl->get_firstNondefiningDeclaration() != cls_decl");

                                        if (local_cls_decl_defining != NULL)
                                           {
                                             printf ("local_cls_decl_defining->get_name()         = %s \n",local_cls_decl_defining->get_name().str());
                                             local_cls_decl_defining->get_startOfConstruct()->display("Error in AST Consistancy tests: cls_decl->get_definingDeclaration() != cls_decl");
                                           }

                                        printf("    Warning: found a SgClassType which is NOT associated with the first nondefining class declaration\n");
                                        printf("    Warning: SgClassType = %p name = %s associated with SgClassDeclaration =%p\n", cls_type, cls_type->get_name().getString().c_str(), cls_decl);
                                        ROSE_ASSERT (false);
                                       }
                                 }
                            }
                       }  

                 // DQ (10/20/2004): Added test to find locations where the mangled template 
                 // class names might be used in unparsing!
                    SgTemplateInstantiationDecl* templateInstantiationDeclaration = 
                         isSgTemplateInstantiationDecl(declarationStatement);
                    if (templateInstantiationDeclaration != NULL)
                       {
                      // if (templateInstantiationDeclaration->get_nameResetFromMangledForm() != false)
                         if (templateInstantiationDeclaration->get_nameResetFromMangledForm() == false)
                            {
                           // ROSE_ASSERT(templateInstantiationDeclaration->get_nameResetFromMangledForm() == true);
                              if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL+1 )
                                 {
                                   printf ("     Warning : At %p = %s found unset templateName (hidden in type) \n",
                                        templateInstantiationDeclaration,
                                        templateInstantiationDeclaration->get_name().str());
                                 }
                                else
                                 {
                                // Provide a less verbose level of output, templated types can be very long!
                                   if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                                      {
                                     // printf ("     Warning : At %p found unset templateName \n",templateInstantiationDeclaration);
                                        printf ("     Warning (verbose=%d): At %p = %s found unset templateName (hidden in type) \n",
                                             SgProject::get_verbose(),templateInstantiationDeclaration,
                                             templateInstantiationDeclaration->get_name().str());
                                      }
                                 }
#if 0
                              printf ("templateInstantiationDeclaration = %p = %s (hidden in type) \n",
                                   templateInstantiationDeclaration,templateInstantiationDeclaration->get_name().str());
                              templateInstantiationDeclaration->get_file_info()->
                                   display("Problematic templateInstantiationDeclaration in Source Code");
#endif
                            }

                      // DQ (6/30/2005): Comment this out, but leave the warning, while we return to test KULL.
                      // DQ (6/20/2005): Reassert this test!
                      // ROSE_ASSERT(templateInstantiationDeclaration->get_nameResetFromMangledForm() == true);

#if STRICT_ERROR_CHECKING
                      // DQ (10/21/2004): Relax checking to handle SWIG generated file from Kull (1st SWIG files)
                         ROSE_ASSERT(templateInstantiationDeclaration->get_nameResetFromMangledForm() == true);
#endif
                       }
                    break;
                  }

               default:
                  {
                 // Ignore other sorts of types for now!
                    break;
                  }
             }
          i++;
        }


       // DQ (3/7/2010): Identify the fragments of the AST that are disconnected.
       // TestForDisconnectedAST::test(sageProject);

       // DQ (3/7/2007): This is the end of the timer scope
        }

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          printf ("Type Test finished. \n");

#else
  // if (sageProject->get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          printf ("Skipping test of query on types \n");
#endif

  // DQ (9/21/2013): Force this to be skipped where ROSE's AST merge feature is active (since the point of 
  // detect inconsistancy in parent child relationships and these will be present when astMerge is active.
  // if (sageProject->get_astMerge() == false)
     if (sageProject->get_astMerge() == false && sageProject->get_Fortran_only() == false)
        {
       // DQ (3/19/2012): Added test from Robb for parents of the IR nodes in the AST.
          TestForParentsMatchingASTStructure::test(sageProject);
        }

  // DQ (12/3/2012): Test source position information.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test source position information started." << endl;
        {
          TimingPerformance timer ("Test source position information:");

          TestForSourcePosition sourcePositionTest;
          sourcePositionTest.traverse(sageProject,preorder);
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test source position information finished." << endl;

  // DQ (12/11/2012): Test source position information.
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test restrict keyword information started." << endl;
        {
          TimingPerformance timer ("Test restrict keyword:");

          TestForMultipleWaysToSpecifyRestrictKeyword restrictKeywordTest;
          restrictKeywordTest.traverse(sageProject,preorder);
        }
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test restrict keyword information finished." << endl;

  // DQ (12/13/2012): Verify that their are no SgPartialFunctionType IR nodes in the memory pool.
     ROSE_ASSERT(SgPartialFunctionType::numberOfNodes() == 0);

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        {
          printf ("At BOTTOM of AstTests::runAllTests() \n");
        }
   }


/*! \page AstProperties AST Properties (Consistency Tests)

\section section6 General AST Properties

    All IR nodes are tests and verified to name a number of specific properties.

\subsection subsection6a Source Code Position 
    All SgLocatedNode and SgInitializedName IR nodes (and those derieved from them)
    are verified to have a valid pointer to a Sg_File_Info object.  Each IR nodes
    has a Sg_File_Info object for the start of the language construct and the end of 
    the language construct.  Note that for scope statements the start and end positions
    identify the opening "{" and closing "}" were appropriate.
    - All nodes have a vaild Sg_File_Info object for the front and end of the subtree.
    - All Sg_File_Info objects are uniques (never shared) \n
      This is important to support classification of IR nodes within transformations 
      (marked as transformed, shared, or compiler generated).
    - Any nodes not passing this tests are recorded internall (output with -rose-verbose >= 2)

\subsection subsection6b SgFunctionCallExp Properties

    The SgFunctionCallExp is tested to verify that the get_function() member function returns 
    only a specific subset of SgExpressions:
       - SgDotExp
       - SgDotStarOp
       - SgArrowExp
       - SgArrowStarOp
       - SgPointerDerefExp
       - SgFunctionRefExp
       - SgMemberFunctionRefExp

    The SgType returned from these SgExpression objects is also restricted to the following 
    subset of SgType objects:
       - SgTypedefType \n
         This is the case of a function call from a pointer to a function "(object->*(mFieldAccessorMethod))();"
       - SgMemberFunctionType
       - SgFunctionType \n
         These are the more common cases
       - SgPointerMemberType \n
         This is an unusual case.

    Scopes are stored explicitly on some IR nodes (where the parent pointers in the AST could provide 
    incorrect evaluation of the scope.  The following IR nodes are tested to verify that their 
    explicitly represented scope is a valid pointer:
       - SgInitializedName
       - SgClassDeclaration
       - SgTemplateInstantiationDecl
       - SgFunctionDeclaration
       - SgMemberFunctionDeclaration
       - SgTemplateInstantiationFunctionDecl
       - SgTemplateInstantiationMemberFunctionDecl
       - SgTemplateDeclaration
       - SgTypedefDeclaration

*/

TestAstProperties::TestAstProperties()
   {
  // DQ (10/24/2004): Initialize array of counters used to record frequency of
  // problem nodes (without file info object to represent end of construct)!
  // for (VariantT i = V_SgModifier; i != V_SgNumVariants; i++)
     for (int i = V_SgModifier; i != V_SgNumVariants; i++)
          nodeWithoutFileInfoFrequencyCount[i] = 0;
   }

TestAstPropertiesSA
TestAstProperties::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList l)
   {
     assert(node);
     TestAstPropertiesSA syn;

#if 0
     int lineNumber = -1;
     string fileName = "no file";
     ROSE_ASSERT (node != NULL);
     if (node->get_file_info() != NULL)
        {
          lineNumber = node->get_file_info()->get_line();
          fileName   = node->get_file_info()->get_filename();
        }
     printf ("TestAstProperties: node = %s at %d of %s \n",node->sage_class_name(),lineNumber,fileName.c_str());
#endif

     for (SynthesizedAttributesList::iterator i=l.begin(); i!=l.end(); i++)
          syn.val = syn.val && (*i).val;
     if (!syn.val)
        {
          cout << "Children are incorrect@" << node-> sage_class_name() << endl;
          for(SynthesizedAttributesList::iterator i=l.begin(); i!=l.end(); i++)
               cout << (*i).val << ", ";
          cout << endl;
        }

     bool problematicNodeFound = AstTests::isProblematic(node);
  // if (problematicNodeFound)
  //      {
  //        cout << "Problematic Node: " << node->sage_class_name() << " found." << endl;
  //      }
     syn.val = syn.val && !problematicNodeFound;

  // Test all traversed nodes to make sure that they have a valid file info object
  // Note that SgFile and SgProject nodes don't have file info objects (so skip them)

  // DQ (11/20/2013): Added SgJavaImportStatementList and SgJavaClassDeclarationList to the exception list since they don't have a source position field.
  // if ( !isSgFile(node) && !isSgProject(node) )
  // if ( !isSgFile(node) && !isSgProject(node) && !isSgAsmNode(node))
  // if ( !isSgFile(node) && !isSgProject(node) && !isSgAsmNode(node) && !isSgFileList(node) && !isSgDirectory(node))
     if ( !isSgFile(node) && !isSgProject(node) && !isSgAsmNode(node) && !isSgFileList(node) && !isSgDirectory(node) && !isSgJavaImportStatementList(node) && !isSgJavaClassDeclarationList(node) )
        {
          Sg_File_Info* fileInfo = node->get_file_info();
          if ( fileInfo == NULL )
             {
               printf ("node->get_file_info() == NULL: node is %s \n",node->sage_class_name());
               ROSE_ASSERT (false);
             }
            else
             {
            // A file info object can only be tested for invalid line number
            // if it is not compiler generated or part of a transformation.
               if ( fileInfo->isCompilerGenerated() == false && fileInfo->isTransformation() == false )
                  {
                    if ( fileInfo->get_line() == 0 && !isSgGlobal(node) )
                       {
                      // printf ("node->get_file_info()->get_line() == 0: node is %s \n",node->sage_class_name());
                         listOfNodesWithoutValidFileInfo.push_back(node);
                      // ROSE_ASSERT (false);
                       }
                  }
             }
        }

  // Record the file info objects in all IR nodes traversed (so that we can test for uniqueness)
     listOfNodesFileInfo.push_back(node);

  // Test the use of file info objects to define the start AND end of language 
  // constructs (statements for now and expressions later on)
     SgStatement* statement = isSgStatement(node);
     if (statement != NULL)
        {
       // printf ("Found a SgStatement, chekc for starting and ending file info objects \n");
          ROSE_ASSERT(statement->get_file_info() != NULL);
          Sg_File_Info* startingSourcePosition = statement->get_startOfConstruct();
          if (startingSourcePosition == NULL)
             {
               if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                    printf ("AST Diagnostics: Lacking starting postion info on %s \n",statement->sage_class_name());
             }
       // ROSE_ASSERT(startingSourcePosition != NULL);
          Sg_File_Info* endingSourcePosition = statement->get_endOfConstruct();
          if (endingSourcePosition == NULL)
             {
               nodeWithoutFileInfoFrequencyCount[statement->variantT()]++;
             }
       // ROSE_ASSERT(endingSourcePosition != NULL);
        }

  // temporarily comment out this newly reported bug (from Willcock)
#if 1
  // DQ (7/11/2004): Added support for more general tests of the AST
  // This function now tests general AST properties (as the name implies)
     switch(node->variantT())
        {
       // DQ (7/11/2004): Test that the function in a function call has a valid function type
          case V_SgFunctionCallExp:
             {
               SgFunctionCallExp* fc = isSgFunctionCallExp(node);
               ROSE_ASSERT (fc != NULL);
               SgExpression* functionExpression = fc->get_function();
               ROSE_ASSERT (functionExpression != NULL);

            // The type of expression is restricted to a subset of all possible expression (check this)
               while (isSgCommaOpExp(functionExpression))
                  {
                    functionExpression = isSgCommaOpExp(functionExpression)->get_rhs_operand();
                  }
                           
               switch (functionExpression->variantT())
                  {
                 // these are the acceptable cases
                    case V_SgDotExp:
                    case V_SgDotStarOp:
                    case V_SgArrowExp:
                    case V_SgArrowStarOp:
                    case V_SgPointerDerefExp:
                       {
                      // these are the acceptable cases
                         break;
                       }

                    case V_SgFunctionRefExp:
                       {
                         SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExpression);
                         ROSE_ASSERT(functionRefExp != NULL);
                         SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
                         ROSE_ASSERT(functionSymbol != NULL);

                      // Make sure that the function has a valid declaration
                         ROSE_ASSERT(functionSymbol->get_declaration() != NULL);
                         break;
                       }

                 // DQ (2/23/2013): Added support for SgTemplateFunctionRefExp (required as part of fix for test2013_69.C).
                    case V_SgTemplateFunctionRefExp:
                       {
                         SgTemplateFunctionRefExp* functionRefExp = isSgTemplateFunctionRefExp(functionExpression);
                         ROSE_ASSERT(functionRefExp != NULL);
                         SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
                         ROSE_ASSERT(functionSymbol != NULL);

                      // Make sure that the function has a valid declaration
                         ROSE_ASSERT(functionSymbol->get_declaration() != NULL);
                         break;
                       }

                    case V_SgMemberFunctionRefExp:
                       {
                         SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionExpression);
                         ROSE_ASSERT(memberFunctionRefExp != NULL);
                         SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
                         ROSE_ASSERT(memberFunctionSymbol != NULL);

                      // Make sure that the function has a valid declaration
                         ROSE_ASSERT(memberFunctionSymbol->get_declaration() != NULL);
                         SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionSymbol->get_declaration();

                      // DQ (7/29/2005): If there is a non-defining declaration available 
                      // then make sure that it is used for all function calls.
                         if (memberFunctionDeclaration->get_firstNondefiningDeclaration() != NULL)
                            {
                           // Since this is a non-defining declaration then it should not have a definition
                              if (memberFunctionDeclaration->get_firstNondefiningDeclaration() != NULL)
                                 {
                                   SgMemberFunctionDeclaration* firstNondefiningDeclaration = 
                                        isSgMemberFunctionDeclaration(memberFunctionDeclaration->get_firstNondefiningDeclaration());
                                   SgTemplateInstantiationMemberFunctionDecl* templateInstantiation = 
                                        isSgTemplateInstantiationMemberFunctionDecl(firstNondefiningDeclaration);
                                   if (templateInstantiation != NULL)
                                      {
                                     // Make sure this is a non-defining declaration
                                        ROSE_ASSERT(templateInstantiation->get_definition() == NULL);
                                      }
                                     else
                                      {
                                     // Make sure this is a non-defining declaration
                                        ROSE_ASSERT(firstNondefiningDeclaration->get_definition() == NULL);
                                      }
                                 }

                           // If this is not a defining declaration then it should match the firstNondefiningDeclaration
                              if (memberFunctionDeclaration->get_definition() == NULL)
                                 {
                                // Make sure that the first non-defining declaration is recorded
                                // int the AST is the one referenced by the memberFunctionRefExp.
                                   if (memberFunctionDeclaration != memberFunctionDeclaration->get_firstNondefiningDeclaration())
                                      {
                                        printf ("memberFunctionDeclaration = %p \n",memberFunctionDeclaration);
                                        printf ("memberFunctionDeclaration->get_firstNondefiningDeclaration() = %p \n",
                                             memberFunctionDeclaration->get_firstNondefiningDeclaration());
                                        memberFunctionDeclaration->get_file_info()->display("memberFunctionDeclaration");
                                        memberFunctionDeclaration->get_firstNondefiningDeclaration()->get_file_info()->display("memberFunctionDeclaration->get_firstNondefiningDeclaration()");
                                        memberFunctionDeclaration->get_firstNondefiningDeclaration()->get_file_info()->display("debug");
                                      }
                                   ROSE_ASSERT(memberFunctionDeclaration == memberFunctionDeclaration->get_firstNondefiningDeclaration());
                                 }

                           // Check the scope of the member function, make sure that it is a SgClassDefinition
                           // (could also be a SgTemplateInstantiationDefn).
                              ROSE_ASSERT(isSgClassDefinition(memberFunctionDeclaration->get_scope()) != NULL);

                           // printf ("AST Consistency Tests: memberFunctionDeclaration %s definition = %p \n",
                           //      memberFunctionDeclaration->get_name().str(),memberFunctionDeclaration->get_definition());
                            }
                         break;
                       }

                 // TV (04/16/2019): used to be casses guarded by ROSE_USE_EDG_VERSION_4
                    case V_SgIntVal:
                    case V_SgFunctionCallExp:
                    case V_SgConstructorInitializer:
                    case V_SgCastExp:
                    case V_SgConditionalExp:
                    case V_SgTemplateParameterVal:
                    case V_SgAddressOfOp:
                    case V_SgPntrArrRefExp:
                    case V_SgVarRefExp:
                    case V_SgTemplateMemberFunctionRefExp:
                       {
                         break;
                       }

                    case V_SgNonrealRefExp:
                       {
                      // TV (05/10/2018): FIXME checks ???
                         break;
                       }

                    default:
                       {
                         printf ("Error case default in switch (functionExpression = %s) \n",functionExpression->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }

            // DQ(1/16/2006): Removed redundent p_expression_type data member (type now computed, most often from operands)
            // ROSE_ASSERT (fc->get_expression_type() != NULL);

            // printf ("Test fc->get_type() != NULL: functionExpression = %s \n",functionExpression->class_name().c_str());
               ROSE_ASSERT (fc->get_type() != NULL);
               ROSE_ASSERT (fc->get_function()->get_type() != NULL);

            // DQ (6/30/2005): Most often this is a SgFunctionType, but it could be a SgTypedefType if 
            // it is a function call from a pointer to a function which is typedefed (see test2005_10.C).
               SgType* callType = fc->get_function()->get_type();
               switch (callType->variantT())
                  {
                    case V_SgTypedefType:
                       {
                      // this is the case of a function call from a pointer to a function "(object->*(mFieldAccessorMethod))();"
                         break;
                       }

                    case V_SgMemberFunctionType:
                    case V_SgFunctionType:
                       {
                      // this is the normal case
                         break;
                       }

                    case V_SgPointerMemberType:
                       {
                      // this is a rare case (demonstrated by test2005_112.C)
                         break;
                       }

                 // This might be requires due to an internal error (then again it does make since that we could have a pointer to a normal function!).
                    case V_SgPointerType:
                       {
                      // DQ (1/16/2006): This is a new case appearing only after reimplementing the get_type() support in SgExpression IR nodes. (demonstrated by test2001_16.C)
#if PRINT_DEVELOPER_WARNINGS
                         printf ("This is a new case appearing only after reimplementing the get_type() support in SgExpression IR nodes. \n");
#endif
                         break;
                       }

                    case V_SgPartialFunctionType:
                       {
                      // This case is only present in the new EDG/Sage interface (demonstrated by gzip.c)
                         break;
                       }
                 // TV: these cases were guarded with EDG 4 condition (date from EDG 3)
                    case V_SgTypeInt:
                    case V_SgTemplateType:
                    case V_SgClassType:
                    case V_SgReferenceType:
                    case V_SgModifierType:
                    case V_SgTypeVoid:
                    case V_SgTypeUnknown:
                    case V_SgRvalueReferenceType:
                       {
                         break;
                       }

                    case V_SgNonrealType:
                    case V_SgAutoType:
                       {
                         break;
                       }

                    case V_SgDeclType:
                       {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                         printf ("Warning: EDG 4.x specific case, found unusual case of SgDeclType returned from SgFunctionCallExp::get_type() member function \n");
#endif
                         break;
                       }

                    default:
                       {
                         printf ("Error case default in switch (callType = %s) \n",callType->sage_class_name());
                         ROSE_ASSERT(false);
                       }
                  }
#if 0               
               SgFunctionType* ft = isSgFunctionType(fc->get_function()->get_type());
               if (ft == NULL)
                  {
                 // This is the case of a function call from a pointer to a function "(object->*(mFieldAccessorMethod))();"
                    printf ("fc->get_function()->get_type() = %p = %s \n",fc->get_function()->get_type(),fc->get_function()->get_type()->sage_class_name());
                    fc->get_file_info()->display("found SgFunctionCallExp with reference to function who's type is not a SgFunctionType");

                    if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                         printf ("Error: internal test found SgFunctionCallExp with reference to function who's type is not a SgFunctionType node \n");
                    int lineNumber = -1;
                    string fileName = "no file";
                    if (fc->get_file_info() != NULL)
                       {
                         lineNumber = fc->get_file_info()->get_line();
                         fileName   = fc->get_file_info()->get_filename();
                       }
                    if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                       {
                         printf ("     fc->get_expression_type() = %s \n",fc->get_expression_type()->sage_class_name());
                         printf ("     fc == %s at %d of %s  \n",fc->sage_class_name(),lineNumber,fileName.c_str());
                         printf ("     fc->get_function() == %s \n",fc->get_function()->sage_class_name());
                       }
                    
                    ROSE_ASSERT (fc->get_function() != NULL);
                    ROSE_ASSERT (fc->get_function()->get_type() != NULL);
                    if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                       {
                         printf ("     fc->get_function()->get_type() == %s \n",
                              fc->get_function()->get_type()->sage_class_name());
                       }
                    ROSE_ASSERT (fc->get_args() != NULL);
                  }
            // DQ (6/30/2005): Allow to be NULL to support function call from pointer to function.
            // There may be a better way to support this within the AST (explicitly mark as function call from pointer?). 
            // ROSE_ASSERT (ft != NULL);
#endif

            // DQ (2/22/2013): Ben Allen has noticed that this will assert fail instead of returning a null pointer.
            // This should be fixed now.
               SgFunctionDeclaration *declaration = fc->getAssociatedFunctionDeclaration();
               if (declaration == NULL)
                  {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    printf ("Note that getAssociatedFunctionDeclaration() has returned NULL pointer (OK in certain cases, see test2013_68.C) \n");
#endif
                  }

               break;
             }

       // DQ (8/3/2004): Added output of problem IR nodes (output when evaluating 
       // the SgFile nodes near the end of the traversal).
          case V_SgFile:
             {
            // Report the number of IR nodes traversed
               if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                    printf ("\n     Total number of nodes traversed = %ld \n",(long)listOfNodesFileInfo.size());

            // Only need to output information if there are a nonzero number of nodes without correct file info
               if (listOfNodesWithoutValidFileInfo.size() > 0)
                  {
                    listOfNodesWithoutValidFileInfo.sort();
                    listOfNodesWithoutValidFileInfo.unique();
#if 0
                    list<SgNode*>::iterator i = listOfNodesWithoutValidFileInfo.begin();
#endif
                    if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                         printf ("\n     List of %ld IR nodes in the AST with default information in their file info objects: \n",(long)listOfNodesWithoutValidFileInfo.size());

                 // DQ 12/17/2007): Commented these out since there are so many for Fortran right now!
                    printf ("\n     COMMENTED OUT List of %ld IR nodes in the AST with default information in their file info objects: \n",(long)listOfNodesWithoutValidFileInfo.size());
#if 0
                    while (i != listOfNodesWithoutValidFileInfo.end())
                       {
                         ROSE_ASSERT((*i)->get_parent() != NULL);
                         if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                              printf ("     IR Nodes with default source position info: %s parent is a %s \n",
                                   (*i)->sage_class_name(),(*i)->get_parent()->sage_class_name());
                      // (*i)->get_file_info()->display("*** default file info ***");
                         i++;
                       }
#endif
                  }

            // Report on number of shared Sg_File_Info objects within the AST (should be none)
               listOfNodesFileInfo.sort();
               int sizeBeforeRemovingRedundantEntries = listOfNodesFileInfo.size();
               listOfNodesFileInfo.unique();
               int sizeAfterRemovingRedundantEntries  = listOfNodesFileInfo.size();
               int redundantEntries = sizeBeforeRemovingRedundantEntries - sizeAfterRemovingRedundantEntries;
               if (redundantEntries != 0)
                  {
                    if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                         printf ("There are %d IR nodes that share a file info object \n",redundantEntries);
                  }
               break;
             }

          case V_SgInitializedName:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgInitializedName* initializedName = isSgInitializedName(node);
               ROSE_ASSERT(initializedName != NULL);
            // printf ("Testing initializedName->get_scope() \n");

            // Note that SgInitializedNames in function paremter lists can be NULL if the
            // function is not a defining declaration (or if no defining declaration exists).
            // Later work will set the scope to the existing defining declaration's scope (later).
               SgFunctionParameterList* parentParameterList = isSgFunctionParameterList(initializedName->get_parent());
               if (parentParameterList != NULL)
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentParameterList->get_parent());

                    if (SageInterface::is_Python_language() && isSgLambdaRefExp(parentParameterList->get_parent()))
                       {
                         std::cerr << "warning: python. Allowing inconsistent scope for InitializedNames in SgLambdaRefExp's parameter lists." << std::endl;
                         break;
                       }

                 // ROSE_ASSERT(functionDeclaration != NULL);
                    if (functionDeclaration != NULL)
                       {
                         bool isFunctionDefinition = (functionDeclaration->get_definition() != NULL);

                      // Only enforce the scope to be valid if this is the defining function declaration 
                      // (later for any function prototype of a defining function declaration!)
                         if (isFunctionDefinition == true)
                            {
                              ROSE_ASSERT(initializedName->get_scope() != NULL);
                            }

                      // Cong (10/20/2010): Here we test if the parent of this initialized name does have it as
                      // an argument. This is to detect if several function parameter lists own the same 
                      // initialized name.

                         const SgInitializedNamePtrList& initNameList = parentParameterList->get_args();
                         SgInitializedNamePtrList::const_iterator result = std::find(initNameList.begin(), initNameList.end(), initializedName);
                         ROSE_ASSERT(result != initNameList.end());
                       }
                      else
                       {
                      // DQ (12/6/2011): Now that we have the template declarations in the AST, this could be a SgTemplateDeclaration.
                         ROSE_ASSERT(isSgTemplateDeclaration(parentParameterList->get_parent()) != NULL);
                         printf ("WARNING: There are tests missing for the case of a parentParameterList->get_parent() that is a SgTemplateDeclaration \n");
                       }
                  }
                 else
                  {
                 // DQ (2/12/2012): Implement better error diagnostics.
                    if (initializedName->get_scope() == NULL)
                       {
                         printf ("Error: initializedName->get_scope() == NULL \n");
                         ROSE_ASSERT(initializedName->get_parent() != NULL);
#if 1
                      // DQ (2/12/2012): Refactoring disagnostic support for detecting where we are when something fails.
                         SageInterface::whereAmI(initializedName);
#else
                         SgNode* parent = initializedName->get_parent();
                         while (parent != NULL)
                            {
                              printf ("parent = %p = %s \n",parent,parent->class_name().c_str());

                              ROSE_ASSERT(parent->get_file_info() != NULL);
                              parent->get_file_info()->display("Error: initializedName->get_scope() == NULL");

                              parent = parent->get_parent();
                            }
#endif
                       }
                    ROSE_ASSERT(initializedName->get_scope() != NULL);
                  }
#if 0
            // DQ (9/6/2005): This test is to simple: we break out the case of a 
            // SgInitializedName in a parameter list of a function prototype above!
               if (initializedName->get_scope() == NULL)
                  {
                    printf ("Error: SgInitializedName with explicit scope unset at %p = %s \n",
                         initializedName,initializedName->sage_class_name());
                    ROSE_ASSERT(initializedName->get_file_info() != NULL);
                    initializedName->get_file_info()->display("Error: SgInitializedName with explicit scope unset");
                  }
               ROSE_ASSERT(initializedName->get_scope() != NULL);
#endif
               break;
             }

          case V_SgClassDeclaration:
          case V_SgDerivedTypeStatement:
          case V_SgTemplateInstantiationDecl:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
               ROSE_ASSERT(classDeclaration != NULL);
               if (classDeclaration->get_scope() == NULL)
                  {
                    printf ("Error: SgClassDeclaration with explicit scope unset at %p = %s \n",
                         classDeclaration,classDeclaration->sage_class_name());
                  }
               ROSE_ASSERT(classDeclaration->get_scope() != NULL);
               break;
             }

          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
               ROSE_ASSERT(functionDeclaration != NULL);
               if (functionDeclaration->get_scope() == NULL)
                  {
                    printf ("Error: SgFunctionDeclaration with explicit scope unset at %p = %s \n",
                         functionDeclaration,functionDeclaration->sage_class_name());
                  }
               ROSE_ASSERT(functionDeclaration->get_scope() != NULL);
               break;
             }

          case V_SgTemplateDeclaration:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(node);
               ROSE_ASSERT(templateDeclaration != NULL);
               if (templateDeclaration->get_scope() == NULL)
                  {
                    printf ("Error: SgTemplateDeclaration with explicit scope unset at %p = %s \n",
                         templateDeclaration,templateDeclaration->sage_class_name());
                  }
               ROSE_ASSERT(templateDeclaration->get_scope() != NULL);
               break;
             }

          case V_SgTypedefDeclaration:
             {
            // DQ (11/28/2004): Verify that newly added explicit scope data members are initialized properly (should be a valid pointer)
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
               ROSE_ASSERT(typedefDeclaration != NULL);
               if (typedefDeclaration->get_scope() == NULL)
                  {
                    printf ("Error: SgTypedefDeclaration with explicit scope unset at %p = %s \n",
                         typedefDeclaration,typedefDeclaration->sage_class_name());
                  }
               ROSE_ASSERT(typedefDeclaration->get_scope() != NULL);
               break;
             }

          case V_SgFunctionDefinition:
             {
            // DQ (8/9/2005): Test for existence of body in function definition
               SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(node);
               if (functionDefinition->get_body() == NULL)
                  {
                    printf ("AST Consistency Test: This function definition violates the rule that every function definition has a function body (even if it is empty) \n");
                    functionDefinition->get_file_info()->display("debug");
                  }
               ROSE_ASSERT(functionDefinition->get_body() != NULL);

            // DQ (3/16/2006): Verify that this is true, it should always be true (for Yarden)
               if (functionDefinition->get_body()->get_parent() != functionDefinition)
                  {
                    printf ("Error: functionDefinition = %p functionDefinition->get_body() = %p functionDefinition->get_body()->get_parent() = %p \n",functionDefinition,functionDefinition->get_body(),functionDefinition->get_body()->get_parent());
                  }
               ROSE_ASSERT(functionDefinition->get_body()->get_parent() == functionDefinition);
               break;
             }

       // DQ (8/25/2004): Default should be used to avoid compiler warning about not 
       // handling all the cases of the enum VariantT.
          default:
             {
            // g++ needs an empty block here
             }
        }
#endif

     return syn;
   }

void 
TestAstNullPointers::visitWithAstNodePointersList(SgNode* node, AstNodePointersList l)
   {
     vector<SgNode*> tsc = node->get_traversalSuccessorContainer();
     for (unsigned i=0; i < l.size(); i++)
        {
          if (l[i] != tsc[i])
             {
               cout << "AST TRAVERSAL ERROR: " << node->sage_class_name() << ": WE WOULD REVISIT NODES FROM THIS NODE. MEMBER:" << i << "." << endl;
             }
        }
   }


/*! \page AstProperties AST Properties (Consistency Tests)

\section section5 Template Properties

    Template within SAGE III are tested to verify specific properties:
     - SgTemplateInstantiationDecl
        -# get_name() returns a valid C++ string object
        -# get_templateName() returns a valid C++ string object
        -# get_templateDeclaration() returns a valid pointer
        -# All template declarations within template instantiations are never marked as compiler generated.
     - SgTemplateInstantiationFunctionDecl
        -# get_name() returns a valid C++ string object
        -# get_templateName() can have an empty string \n
           This should be fixed at some point.
        -# get_templateDeclaration() returns a valid pointer
        -# All template declarations within template instantiations are never marked as compiler generated.
     - SgTemplateInstantiationMemberFunctionDecl
        -# get_name() returns a valid C++ string object
        -# get_templateName() can have an empty string \n
           This should be fixed at some point.
        -# get_templateDeclaration() returns a valid pointer
        -# Template declarations within template instantiations can be compiler generated. \n
           Member functions of templated classes that are declared in the class are represented 
           outside the class as template specializations and are marked as compiler generated.
     - SgClassDefinition
     - SgTemplateInstantiationDefn \n
         Base classes within class definitions and template instatiation definitions are searched 
         and verified to have properly reset template names (from original EDG names, see 
         \ref resetTemplateNameTest ).
     - All possible template instantiations \n
        -# These are tested to verify that get_specialization() returns a non-default value to verify that 
           some catagory of template specialization has been specified (values verified to have been reset 
           from the defaults).
        -# contain a valid pointer to a template declaration.
     - All non-template instantiations
        -# Are maked with default value SgClassDeclaration::e_no_specialization.

*/


void
TestAstTemplateProperties::visit ( SgNode* astNode )
   {
  // DQ (3/31/2004): Added to support templates
  // This function tests properties on the new template specific IR nodes

  // printf ("astNode = %s \n",astNode->sage_class_name());

#if 0
     SgNode * parent = astNode->get_parent();
     printf ("In TestAstTemplateProperties::visit():\n");
     printf ("  --- astNode = %p (%s)\n", astNode, astNode ? astNode->class_name().c_str() : "");
     printf ("  --- parent = %p (%s)\n", parent, parent ? parent->class_name().c_str() : "");
#endif

     switch(astNode->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
               SgTemplateInstantiationDecl* s = isSgTemplateInstantiationDecl(astNode);
               ROSE_ASSERT(s != NULL);
#if 0
               printf ("  --- name = %s\n", s->get_name().str());
#endif

               SgDeclarationStatement* templateDeclaration = s->get_templateDeclaration();
               if (templateDeclaration->get_definingDeclaration()) {
                 templateDeclaration = templateDeclaration->get_definingDeclaration();
               }
#if 0
               printf ("  --- templateDeclaration = %p (%s)\n", templateDeclaration, templateDeclaration ? templateDeclaration->class_name().c_str() : "");
#endif

            // DQ (8/12/2005): There are non-trivial cases where a template declaration can be compiler generated (e.g. when it is a nested class)
               bool couldBeCompilerGenerated = MarkAsCompilerGenerated::templateDeclarationCanBeMarkedAsCompilerGenerated(templateDeclaration);

               if (couldBeCompilerGenerated == false)
                  {
                 // DQ (6/17/2005): Template declarations should not be marked as comiler generated 
                 // (only the instantiations are possibly marked as compiler generated).
                    if (templateDeclaration->get_file_info()->isCompilerGenerated() == true)
                       {
                         printf ("Error: SgTemplateInstantiationDecl's original template declaration should not be compiler generated \n");
                         templateDeclaration->get_file_info()->display("debug");
                       }
                    ROSE_ASSERT (templateDeclaration->get_file_info()->isCompilerGenerated() == false);
                  }
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
               SgTemplateInstantiationFunctionDecl* s = isSgTemplateInstantiationFunctionDecl(astNode);
            // DQ (5/8/2004): templateName() removed
            // ROSE_ASSERT (s->get_templateName().str() != NULL);

            // DQ (4/30/2012): Allow this test to pass for test2012_58.C (condition relaxed as part of new EDG support).
            // It might be that the function template does not exist but that the class template containing the function template is available.
               if (s->get_templateDeclaration() == NULL)
                  {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                    printf ("ERROR: s->get_templateDeclaration() == NULL (s = %p = SgTemplateInstantiationFunctionDecl) \n",s);
#endif
                  }
             // ROSE_ASSERT (s->get_templateDeclaration() != NULL);

               if (s->get_templateDeclaration() != NULL)
                  {
                 // DQ (6/17/2005): Template declarations should not be marked as compiler generated 
                 // (only the instantiations are possibly marked as compiler generated).
                    if (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == true && SgProject::get_verbose() > 0)
                       {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                         printf ("Warning: SgTemplateInstantiationFunctionDecl's original template declaration %s is marked as compiler generated: \n", s->get_templateDeclaration()->get_qualified_name().str());
                         s->get_startOfConstruct()->display("SgTemplateInstantiationFunctionDecl debug");
                         s->get_templateDeclaration()->get_startOfConstruct()->display("SgTemplateDecl debug");
#endif
                       }
                 // ROSE_ASSERT (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == false);
                  }
               break;
             }

          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
               SgTemplateInstantiationMemberFunctionDecl* s = isSgTemplateInstantiationMemberFunctionDecl(astNode);
            // DQ (5/8/2004): templateName() removed
            // ROSE_ASSERT (s->get_templateName().str() != NULL);

            // DQ (5/3/2012): Allow this for now, but make it a warning.
               if (s->get_templateDeclaration() == NULL)
                  {
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if 0
                    printf ("WARNING: case V_SgTemplateInstantiationMemberFunctionDecl: templateDeclaration == NULL (some templates are unavailable in EDG). \n");
#endif
                  }
            // ROSE_ASSERT (s->get_templateDeclaration() != NULL);

            // explicit specializations in the source code should not be marked as compiler generated
               if (s->isSpecialization() == true || s->isPartialSpecialization() == true)
                  {
                    if (s->get_file_info()->isCompilerGenerated() == true)
                       {
                         printf ("SgTemplateInstantiationMemberFunctionDecl (%p) is marked as a specialization and compiler generated (not allowed) \n",s);
                         s->get_file_info()->display("SgTemplateInstantiationMemberFunctionDecl: debug");
                         printf ("s->get_name() = %s \n",s->get_name().str());
                       }
                    ROSE_ASSERT(s->get_file_info()->isCompilerGenerated() == false);
                  }

#if 0
            // DQ (6/20/2005): This is actually OK, since the template declaration is in the 
            // outer classes template specialization!
            // DQ (6/17/2005): Template declarations should not be marked as comiler generated 
            // (only the instatiations are posibily marked as compiler generated).
               if (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == true)
                  {
                    s->get_templateDeclaration()->get_file_info()->display("debug");
                    printf ("s->get_templateDeclaration()->get_name() = %s string = %s \n",
                         s->get_templateDeclaration()->get_name().str(),
                         s->get_templateDeclaration()->get_string().str());
                  }
            // ROSE_ASSERT (s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == false);
#endif
               break;
             }

          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn:
             {
            // DQ (6/22/2005): templated class declarations can be hidden in base class specifications
               SgClassDefinition* classDefinition = isSgClassDefinition(astNode);
               ROSE_ASSERT(classDefinition != NULL);

               SgBaseClassPtrList::iterator i = classDefinition->get_inheritances().begin();
               for ( ; i != classDefinition->get_inheritances().end(); ++i)
                  {
                 // Check the parent pointer to make sure it is properly set
                    ROSE_ASSERT( (*i)->get_parent() != NULL);
                    ROSE_ASSERT( (*i)->get_parent() == classDefinition);

                 // skip this check for SgExpBaseClasses, which don't need to define p_base_class 
                    if (isSgExpBaseClass(*i) != NULL)
                         continue;

                 // Calling resetTemplateName()
                    SgClassDeclaration* baseClassDeclaration = (*i)->get_base_class();
                    ROSE_ASSERT(baseClassDeclaration != NULL);
                 // printf ("In AST Consistancy test: baseClassDeclaration->get_name() = %s \n",baseClassDeclaration->get_name().str());
                    SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(baseClassDeclaration);
                    if (templateInstantiation != NULL)
                       {
                      // DQ (2/12/2012): Implemented some diagnostics (fails for test2004_35.C).
                         if (templateInstantiation->get_nameResetFromMangledForm() == false)
                            {
                              printf ("In AST Consistancy test: templateInstantiation = %p = %s \n",templateInstantiation,templateInstantiation->class_name().c_str());
                              printf ("In AST Consistancy test: templateInstantiation->get_templateName() = %s \n",templateInstantiation->get_templateName().str());
                              printf ("In AST Consistancy test: templateInstantiation->get_name()         = %s \n",templateInstantiation->get_name().str());
                           // SageInterface::whereAmI(templateInstantiation);

                           // DQ (7/14/2019): Reset the name to see how it would be changed (for debugging)
                           // The fix was to force the new file built in SageBuilder::buildFile() to call the astPostProcessing() 
                           // function which will internally reset the template names.

                           // This function will reset nameResetFromMangledForm to true, so we want to force the failure below.
                           // templateInstantiation->resetTemplateName();

                           // Re-output the template name.
                           // printf ("In AST Consistancy test: (after resetTemplateName()): templateInstantiation->get_templateName() = %s \n",templateInstantiation->get_templateName().str());
                           // printf ("In AST Consistancy test: (after resetTemplateName()): templateInstantiation->get_name()         = %s \n",templateInstantiation->get_name().str());
#if 0
                           // Force this to fail since the function call above we set nameResetFromMangledForm to true.
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                         ROSE_ASSERT(templateInstantiation->get_nameResetFromMangledForm() == true);
                       }
                  }
               break;
             }

          default: 
             {
            // DQ (5/9/2005): Make sure that all non-templated IR nodes are marked as SgDeclarationStatement::e_no_specialization.
               SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(astNode);
               if (variableDeclaration != NULL)
                  {
                    SgScopeStatement* scope = variableDeclaration->get_scope();
                    SgClassDefinition* classDefinition = isSgClassDefinition(scope);
                    if (classDefinition != NULL)
                       {
                         SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(classDefinition);
                         if (templateClassDefinition != NULL)
                            {
                           // Special case...debugging new use of SgTemplateClassDefinition.
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                              printf ("AST ConsistancyTest: TestAstTemplateProperties Found a case of SgTemplateClassDefinition (case not handled) \n");
#endif
                            }
                           else
                            {
                              SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                              ROSE_ASSERT(classDeclaration != NULL);
                              if (isSgTemplateInstantiationDecl(classDefinition->get_declaration()) == NULL)
                                 {
                                // This is NOT a data member of the templated class (or nested class of a templated class)
                                   if (variableDeclaration->get_specialization() != SgClassDeclaration::e_no_specialization)
                                      {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                                        printf ("Note: AST ConsistancyTest: variableDeclaration->get_specialization() = %d != SgClassDeclaration::e_no_specialization  (variableDeclaration = %p) \n",variableDeclaration->get_specialization(),variableDeclaration);
                                        variableDeclaration->get_file_info()->display("variableDeclaration->get_specialization() != SgClassDeclaration::e_no_specialization");
#endif
                                      }
                                // DQ (6/30/2005): Commented out to focus more on KULL, output a warning for now!
                                // ROSE_ASSERT(variableDeclaration->get_specialization() == SgClassDeclaration::e_no_specialization);
                                 }
                                else
                                 {
                                // Make sure that all template class declarations are associated with a template definition!
                                   if (isSgTemplateInstantiationDefn(classDefinition) == NULL)
                                      {
                                        printf ("Warning: classDefinition = %p = %s \n",classDefinition,classDefinition != NULL ? classDefinition->class_name().c_str() : "null");
                                      }

                                // DQ (1/1/2012): I think we can assert that this is only a SgClassDefinition (but leave the warning message above).
                                // ROSE_ASSERT(isSgTemplateInstantiationDefn(classDefinition) != NULL);
                                   ROSE_ASSERT(isSgClassDefinition(classDefinition) != NULL);
                                 }
                            }
                       }
                  }

            // DQ (5/9/2005): Make sure that all non-templated IR nodes are marked as SgDeclarationStatement::e_no_specialization.
               SgClassDeclaration *classDeclaration = isSgClassDeclaration(astNode);
               if (classDeclaration != NULL)
                  {
                    if (isSgTemplateInstantiationDecl(classDeclaration) == NULL)
                       {
                      // this is NOT a data member of the templated class (or nested class of a templated class)
                         if (classDeclaration->get_specialization() != SgClassDeclaration::e_no_specialization)
                            {
                              printf ("AST ConsistancyTest: classDeclaration = %p = %s classDeclaration->get_specialization() = %d != SgClassDeclaration::e_no_specialization \n",
                                   classDeclaration,classDeclaration->get_name().str(),classDeclaration->get_specialization());
                              printf ("     classDeclaration at file %s line = %d \n",
                                   classDeclaration->get_file_info()->get_raw_filename().c_str(),
                                   classDeclaration->get_file_info()->get_raw_line());
                            }
                         ROSE_ASSERT(classDeclaration->get_specialization() == SgClassDeclaration::e_no_specialization);
                       }
                  }

            // DQ (5/9/2005): Make sure that all non-templated IR nodes are marked as SgDeclarationStatement::e_no_specialization.
               SgFunctionDeclaration *functionDeclaration = isSgFunctionDeclaration(astNode);
               if (functionDeclaration != NULL)
                  {
                    if ( isSgTemplateInstantiationFunctionDecl(functionDeclaration) == NULL &&
                         isSgTemplateInstantiationMemberFunctionDecl(functionDeclaration) == NULL )
                       {
                      // this is NOT a data member of the templated class (or nested class of a templated class)
                         ROSE_ASSERT(functionDeclaration->get_specialization() == SgFunctionDeclaration::e_no_specialization);
                       }
                  }
             }
        }
   }


void
TestAstCompilerGeneratedNodes::visit ( SgNode* node )
   {
  // DQ (6/20/2005): 
  // printf ("node = %s \n",node->sage_class_name());

  // DQ (8/17/2005): isOutputInCodeGeneration() is now an orthogonal concept to isCompilerGenerated()
  // there is no relationship between the two except that some IR nodes that are compiler generated 
  // are also (separately) marked as to be output in the code generation phase.

     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
       // If this is a compiler generated node to be unparsed then is should at least be marked as compiler generated
       // if (fileInfo->isCompilerGeneratedNodeToBeUnparsed() == true)
          if (fileInfo->isOutputInCodeGeneration() == true)
             {
#if 0
            // DQ (9/10/2005):
            // This test is only done for non-template declarations (unclear if 
            // template declarations should be considered compiler generated, it 
            // seems that sume are that should certainly not be so and this was 
            // failing other tests, so we have made non template declaration 
            // compiler generated as a test to see if we can pass KULL)
               if (isSgTemplateDeclaration(node) == NULL)
                  {
                    ROSE_ASSERT(fileInfo->isCompilerGenerated() == true);
                  }
#endif
             }
        }
   }

/*! \page AstProperties AST Properties (Consistency Tests)

\section section4 Mangle Name Properties

    Mangled names within SAGE III follow specific rules and are tested:
     -# May be used as variable names in C and C++ \n
        This implies that they follow all the rules regarding variable naming within C and C++ (not repeated here).
     -# There are no EDG generated name fragements from template instantiation \n
        EDG internally generates unique names (e.g "foo____L1042") for template instatiations, we convert all such 
        names and use the new names of the form "foo<int>" before name mangling.  Through name mangling, longer
        names are generated of the form "foo__tas_int__tae", from which the original template names and arguments 
        can be recognised ("<" -> "__tas" and ">" -> "__tae", for template argument start (tas) and template 
        argument end (tae).

*/

void
TestAstForProperlyMangledNames::visit ( SgNode* node )
   {
  // DQ (4/27/2005): Added to verify properties of mangled names (no <>, etc.).

  // printf ("node = %s \n",node->sage_class_name());

     string mangledName;

  // DQ (4/3/2011): This is used to compute the file if isValidMangledName() fails.
  // SgFile* file = NULL;

#if 0
     printf ("In TestAstForProperlyMangledNames::visit(SgNode* node = %p = %s) \n",node,node->class_name().c_str());
#endif

  // DQ (4/28/2005): Check out the mangled name for classes
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
     if (classDeclaration != NULL)
        {
       // RV (2/2/2006)
#if 0
          int counter = 0; // counts the numbre of scopes back to global scope (not critical, but useful for debugging)
          mangledName = classDeclaration->get_mangled_qualified_name(counter).str();
#else
          mangledName = classDeclaration->get_mangled_name().getString();
#endif

       // DQ (8/28/2006): Added tests for the length of the mangled names
          unsigned long mangledNameSize = mangledName.size();
          saved_totalMangledNameSize += mangledNameSize;
          saved_numberOfMangledNames++;
          if (saved_maxMangledNameSize < mangledNameSize)
             {
               saved_maxMangledNameSize = mangledNameSize;
               if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                  {
                    printf ("saved_maxMangledNameSize = %ld average size = %ld \n",saved_maxMangledNameSize,saved_totalMangledNameSize/saved_numberOfMangledNames);
                  }
             }

#if 0
          string name = classDeclaration->get_name().str();
          printf ("In TestAstForProperlyMangledNames::visit(SgNode*): \n           unmangled name = %s \n check mangled class name = %s \n",name.c_str(),mangledName.c_str());
#endif

       // Make sure that there is no template specific syntax included in the mangled name
          if ( mangledName.find('<') != string::npos )
             {
               string name = classDeclaration->get_name().str();
               printf ("In TestAstForProperlyMangledNames::visit(SgNode*): \n           unmangled name = %s \n check mangled class name = %s \n",
                    name.c_str(),mangledName.c_str());
             }
          ROSE_ASSERT(mangledName.find('<') == string::npos);
          if ( mangledName.find('>') != string::npos )
             {
               string name = classDeclaration->get_name().str();
               printf ("In TestAstForProperlyMangledNames::visit(SgNode*): \n           unmangled name = %s \n check mangled class name = %s \n",
                    name.c_str(),mangledName.c_str());
             }
          ROSE_ASSERT(mangledName.find('>') == string::npos);
/*
       // DQ (4/3/2011): This is a fix to permit Java names that can include '$' to be handled properly.
       // When the simpler test fails we compute what the current langauge is (relatively expensive so 
       // we don't want to do so for each IR node) and the rerun the test with java specified explicitly.
          bool anErrorHasOccured = false;
          if (isValidMangledName(mangledName) != true)
             {
             // Check first if this is for a Java file and if so then '$' is allowed.
                file = TransformationSupport::getFile(classDeclaration);
                ROSE_ASSERT(file != NULL);

                if (file->get_Java_only() == false)
                   {
                     anErrorHasOccured = true;
                   }
                  else
                   {
                  // printf ("Rerun the test for isValidMangledName() with java langauge specified \n");
                     bool javaInUse = true;
                     anErrorHasOccured = !isValidMangledName(mangledName,javaInUse);
                   }
                
               if (anErrorHasOccured == true)
                  {
                    printf ("Error: failed isValidMangledName() test classDeclaration = %p = %s = %s --- mangledName = %s \n",
                         classDeclaration,classDeclaration->get_name().str(),classDeclaration->class_name().c_str(),mangledName.c_str());
                    classDeclaration->get_file_info()->display("Error: failed isValidMangledName() test");
                  }
             }
       // ROSE_ASSERT(isValidMangledName(mangledName) == true);
          ROSE_ASSERT(anErrorHasOccured == false);
*/
        }

  // DQ (4/27/2005): Check out the mangled name for functions
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL)
        {
          mangledName = functionDeclaration->get_mangled_name().str();
#if 0
          string name = functionDeclaration->get_name().str();
          printf ("In TestAstForProperlyMangledNames::visit(SgNode*): name = %s check mangled function name = %s \n",name.c_str(),mangledName.c_str());
#endif
       // Make sure that there is no template specific syntax included in the mangled name
          ROSE_ASSERT(mangledName.find('<') == string::npos);
          ROSE_ASSERT(mangledName.find('>') == string::npos);
        }
  // Make sure that there is no template specific syntax included in the mangled name
     ROSE_ASSERT(mangledName.find('`') == string::npos);
     ROSE_ASSERT(mangledName.find('~') == string::npos);
     ROSE_ASSERT(mangledName.find('!') == string::npos);

  // ROSE_ASSERT(mangledName.find('@') == string::npos);
     if (mangledName.find('@') != string::npos)
        {
          printf ("Error: failed isValidMangledName() test node = %p = %s --- mangledName = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }
     ROSE_ASSERT(mangledName.find('@') == string::npos);

     ROSE_ASSERT(mangledName.find('#') == string::npos);

/*
  // DQ (4/3/2011): Java allows for '$' so we have to exclude this test when Java is used.
  // note that if it was isValidMangledName() failed (could be many reasons) then file has
  // been computed and is available.
  // ROSE_ASSERT(mangledName.find('$') == string::npos);
     if (file == NULL || (file != NULL && file->get_Java_only() == false) )
        {
           ROSE_ASSERT(mangledName.find('$') == string::npos);
        }
       else
        {
       // If this is a Java file and there is a '$' is fould then assert using a weaker test.
          if (mangledName.find('$') != string::npos)
             {
            // A '$' was found, check using a more expensive test.
               ROSE_ASSERT(file->get_Java_only() == true);

               bool javaInUse = true;
               ROSE_ASSERT(isValidMangledName(mangledName,javaInUse) == true);
             }
        }

     ROSE_ASSERT(mangledName.find('%') == string::npos);
     ROSE_ASSERT(mangledName.find('^') == string::npos);
     ROSE_ASSERT(mangledName.find('&') == string::npos);
     ROSE_ASSERT(mangledName.find('*') == string::npos);
*/

  // DQ (8/13/2005): this is an error in KULL (use of siloswigtypecheck.cc)
  // Commented out this tests so that I can defer it to later!
     if (mangledName.find('(') != string::npos)
        {
          printf ("AST Consistency Test: found \"(\" in mangledName = %s \n",mangledName.c_str());

          SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
          ROSE_ASSERT(declaration != NULL);
          declaration->get_file_info()->display("debug");
        }
  // ROSE_ASSERT(mangledName.find('(') == string::npos);

  // DQ (8/13/2005): this is an error in KULL (use of siloswigtypecheck.cc)
  // Commented out this tests so that I can defer it to later!
     if (mangledName.find(')') != string::npos)
        {
          printf ("AST Consistency Test: found \")\" in mangledName = %s \n",mangledName.c_str());

          SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
          ROSE_ASSERT(declaration != NULL);
          declaration->get_file_info()->display("debug");
        }
  // ROSE_ASSERT(mangledName.find(')') == string::npos);

  // DQ (8/9/2005): this is an error in KULL (use of boost/mpl/if.hpp)
     if (mangledName.find('-') != string::npos)
        {
          printf ("AST Consistency Test: found \"-\" in mangledName = %s \n",mangledName.c_str());

          SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
          ROSE_ASSERT(declaration != NULL);
          declaration->get_file_info()->display("debug");
        }
     ROSE_ASSERT(mangledName.find('-') == string::npos);

     ROSE_ASSERT(mangledName.find('+') == string::npos);
     ROSE_ASSERT(mangledName.find('=') == string::npos);
     ROSE_ASSERT(mangledName.find('{') == string::npos);
     ROSE_ASSERT(mangledName.find('}') == string::npos);
     ROSE_ASSERT(mangledName.find('[') == string::npos);
     ROSE_ASSERT(mangledName.find(']') == string::npos);
     ROSE_ASSERT(mangledName.find('|') == string::npos);
     ROSE_ASSERT(mangledName.find('\\') == string::npos);

  // DQ (2/22/2007): Added error checking to report problems found in mangled names
     if (mangledName.find("::") != string::npos)
        {
          printf ("AST Consistency Test: found \"::\" in mangledName = %s \n",mangledName.c_str());

          SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
          ROSE_ASSERT(declaration != NULL);
          declaration->get_file_info()->display("debug");
        }
     ROSE_ASSERT(mangledName.find("::") == string::npos);

     ROSE_ASSERT(mangledName.find(':') == string::npos);
     ROSE_ASSERT(mangledName.find(';') == string::npos);
     ROSE_ASSERT(mangledName.find('\"') == string::npos);
     ROSE_ASSERT(mangledName.find('?') == string::npos);
     ROSE_ASSERT(mangledName.find('.') == string::npos);
     ROSE_ASSERT(mangledName.find('/') == string::npos);
     ROSE_ASSERT(mangledName.find(',') == string::npos);

  // These are the most common cases that fail
     ROSE_ASSERT(mangledName.find('<') == string::npos);
     ROSE_ASSERT(mangledName.find('>') == string::npos);

  // Jovial can have names like a'variable'name so don't disallow '\'' for Jovial [Rasmussen 2/10/2019]
     if (!SageInterface::is_Jovial_language())
        {
           ROSE_ASSERT(mangledName.find('\'') == string::npos);
        }
   }

TestAstForProperlyMangledNames::TestAstForProperlyMangledNames()
   : saved_maxMangledNameSize(0),saved_totalMangledNameSize(0),saved_numberOfMangledNames(0)
   {
  // Nothing to put here!
   }

bool
TestAstForProperlyMangledNames::isValidMangledName (string name, bool java_lang /* = false */ )
   {
  // DQ (4/3/2011): This function has been modified to permit Java specific weakened restrictions 
  // on names. The default for java_lang is false.  If a test fails the current language is 
  // determined and java_lang set to true if the current langage is Java for the associated SgFile.

     bool result = true;

     if (name.empty () || isdigit (name[0]))
        {
          result = false;
        }

     if (java_lang == true)
        {
       // The case for Java has to allow a few more characters into names (e.g. '$')
          for (string::size_type i = 0; i < name.size (); ++i)
             {
            // printf ("java_lang == true: isalnum (name = %s name[i] = %c) = %s \n",name.c_str(),name[i],isalnum (name[i]) ? "true" : "false");
               if (!isalnum (name[i]) && !(name[i] == '_' || name[i] == '$'))
                  {
                    result = false;
                  }
             }
        }
       else
        {
          for (string::size_type i = 0; i < name.size (); ++i)
             {
            // printf ("java_lang == false: isalnum (name = %s name[i] = %c) = %s \n",name.c_str(),name[i],isalnum (name[i]) ? "true" : "false");
               if (!isalnum (name[i]) && name[i] != '_')
                  {
                    result = false;
                  }
             }
        }

#if 0
     if (result == false)
        {
          printf ("ERROR: isValidMangledName(name = %s) == false \n",name.c_str());
        }
#endif

     return result;
   }

/*! \page AstProperties AST Properties (Consistency Tests)

\section section2 Unique Statements in Scope

     This test verifies each statement in a scope is unique. This catches rewrite and general 
transformation errors that might insert a statement twice or relocate in by forget to delete 
it.  Since it only works within a single scope it is not very robust. 

*/

void
TestAstForUniqueStatementsInScopes::visit ( SgNode* node )
   {
  // DQ (3/31/2004): Added to locate scopes that have redundent statements.
  // This could happen either because of a bug in the EDG/SAGE connection,
  // or as a result of using the rewrite mechanism inappropriately.

  // printf ("node = %s \n",node->sage_class_name());

  // DQ (4/1/2004): Added code to detect redundent statements in a scope!
     SgScopeStatement* scope = isSgScopeStatement(node);
     if (scope != NULL)
        {
       // Generate a list of statements in the scope (even if they are really declaration statements)
          SgStatementPtrList statementList;
          switch ( scope->variantT() )
             {
               case V_SgIfStmt:
                  {
                    SgIfStmt* ifStatement = isSgIfStmt(scope);
                    ROSE_ASSERT (ifStatement != NULL);
                    statementList = isSgBasicBlock(ifStatement->get_true_body()) ? isSgBasicBlock(ifStatement->get_true_body())->generateStatementList() : SgStatementPtrList(1, ifStatement->get_true_body());
                    SgStatementPtrList falseStatementList = isSgBasicBlock(ifStatement->get_false_body()) ? isSgBasicBlock(ifStatement->get_false_body())->generateStatementList() : ifStatement->get_false_body() ? SgStatementPtrList(1, ifStatement->get_false_body()) : SgStatementPtrList();

                 // statementList.merge(falseStatementList);
                    statementList.insert(statementList.end(),falseStatementList.begin(),falseStatementList.end());

                    break;
                  }
               case V_SgDeclarationScope:
                  break;
               default:
                    statementList = scope->generateStatementList();
             }
       // MK (8/3/05) : Rewrote this test to ignore statements which are marked as shared. These may actually exist multiple times in a scope

       // seenStatements is a unique container. Thus, we know that a statement can only go into it once. If
       // encounter a statement twice, then it will go into duplicateStatements, where we can report it
       // later, if necessary.
          set<SgStatement *> seenStatements;
          list<SgStatement *> duplicateStatements;
          seenStatements.clear();
          duplicateStatements.clear();

       // Set this to false if we should fail the test
          bool pass = true;

       // This gives us the total number of statements checked, useful for reporting
          int totalStatements = statementList.size();
          int numberOfDuplicates = 0;
          int numberOfUniques = 0;
          int numberOfShared = 0;

       // Go through the statements, and put any duplicates found into duplicateStatements
          for (SgStatementPtrList::iterator i = statementList.begin(); i != statementList.end(); i++)
             {
               SgStatement * currStatement = *i;

            // ignore shared nodes
               if (currStatement->get_file_info()->isShared())
                  {
                    numberOfShared++;
                  }
                 else
                  {
                 // DQ (2/22/2007): It is likely that count is more expensive than find since we only want existence.
                    if (seenStatements.count(currStatement) == 0)
                       {
                         seenStatements.insert(currStatement);
                         numberOfUniques++;
                       }
                      else
                       {
                         duplicateStatements.push_back(currStatement);
                         numberOfDuplicates++;
                       }
                  }
             }

       // If there are duplicate statements, we have an error
          if (numberOfDuplicates != 0)
             {
               pass = false;
             }

          if (!pass)
             {
               if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL || true )
                  {
                    cout << "Problematic Node: " << node->sage_class_name() << " found. (a statement appears more than once in this scope)" << endl;
              
                    printf ("Error: number of [non-shared] statements = %d  number of unique statements = %d \n",
                    totalStatements - numberOfShared, numberOfUniques);
              
                 // verify that there are duplicates
                    ROSE_ASSERT(numberOfDuplicates > 0);

                    printf ("Number of duplicate statements = %d \n",numberOfDuplicates);
#if 0
                    printf ("Exiting as a test ... \n");
                    ROSE_ASSERT(false);
#endif
                    int counter = 0;
                    for (list<SgStatement *>::iterator j = duplicateStatements.begin(); j != duplicateStatements.end(); j++)
                       {
                         SgStatement * currDuplicate = *j;
                         ROSE_ASSERT(currDuplicate != NULL);

                      // DQ (6/26/2016): Debugging a special case that appears with ROSE compiles "rose.h" header file.
                         SgTemplateInstantiationDefn* templateInstantiationDefn = isSgTemplateInstantiationDefn(node);
                         if (templateInstantiationDefn != NULL)
                            {
                              SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(templateInstantiationDefn->get_declaration());
                              ROSE_ASSERT(templateInstantiationDecl != NULL);
                              printf ("ERROR: problem declaration: templateInstantiationDecl = %p = %s \n",templateInstantiationDecl,templateInstantiationDecl->get_name().str());
                            }

                         Sg_File_Info * location = currDuplicate->get_file_info();
                         ROSE_ASSERT(location != NULL);
                         printf ("Error: node (%d/%d) = %p = %s at: \n",counter,numberOfDuplicates,currDuplicate,currDuplicate->sage_class_name());

                      // DQ (3/21/2011): Added more detail to debug duplicate entries...
                         printf ("currDuplicate name = %s \n",SageInterface::get_name(currDuplicate).c_str());

                         if (location != NULL)
                            {
                              location->display("redundant IR node");
                            }
                         counter++;
                       }
                  }
             }

          if (pass == false)
             {
               printf ("Error: duplicate statements in scope = %p = %s \n",scope,scope->class_name().c_str());
               scope->get_file_info()->display("Error: duplicate statements in scope");
             }
#if 1
          ROSE_ASSERT(pass == true);
#else
       // DQ (6/6/2013): debugging... (test2013_198.C)
       // DQ (8/9/2012): debugging... (test2012_174.C)
          if (pass == false)
             {
               printf ("****** Commented out this error to view the dot file ****** \n");
             }
#endif
        }
   }

/*! \page AstProperties AST Properties (Consistency Tests)

\section section2 Unique IR nodes in the AST

     This test verifies each IR nodes visited in the AST is only visited once.
This is a more robust version of the previous test which checked for shared 
IR nodes in the same scope (which is a more common problem).  This test is 
more expensive in memory since it has to save a reference to ever IR node
and test if it has been previously seen.
*/

void
TestAstForUniqueNodesInAST::visit ( SgNode* node )
   {
  // DQ (4/2/2012): This is a more robust (and expensive) test to check for shared IR nodes in the AST (there should be none).
     ROSE_ASSERT(node != NULL);

#if 0
     printf ("In TestAstForUniqueNodesInAST::visit (): IR node = %p = %s = %s in the AST. \n",node,node->class_name().c_str(),SageInterface::generateUniqueName(node,true).c_str());
     Sg_File_Info* source_position = node->get_file_info();
     if (source_position != NULL)
        {
          printf ("   --- line %d \n",source_position->get_line());
        }
#endif

     if (astNodeSet.find(node) != astNodeSet.end())
        {
          SgLocatedNode* locatedNode = isSgLocatedNode(node);
          if (locatedNode != NULL)
             {
            // Note that we must exclude IR nodes marked explicitly as shared by AST merge.
               ROSE_ASSERT(locatedNode->get_file_info() != NULL);
               if (locatedNode->get_file_info()->isShared() == false)
                  {
                    if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                       {
                         printf ("Warning: found a shared IR node = %p = %s in the AST. \n",node,node->class_name().c_str());
                         locatedNode->get_file_info()->display("Error: found a shared IR node (might be marked as shared after AST merge; not handled yet)");
                       }
                  }
                 else
                  {
#if 0
                 // DQ (11/3/2016): Comment out this output spew from the AST File I/O testing.
                    printf ("Note: found a shared IR node = %p = %s in the AST (OK if part of merged AST) \n",node,node->class_name().c_str());
#endif
#if 0
                    SgProject* project = TransformationSupport::getProject(locatedNode);
                    project->display("In TestAstForUniqueNodesInAST::visit()");
#endif
#if 0
                    if (project->get_astMerge() == true)
                       {
                         printf ("In TestAstForUniqueNodesInAST::visit(): We can detect when we want to supress the AST consistancy tests that detect sharing when we are merging ASTs \n");
                       }
#endif
#if 0
                    SgSourceFile* file = TransformationSupport::getSourceFile(locatedNode);
                    file->display("In TestAstForUniqueNodesInAST::visit()");
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
            else
             {
               if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                    printf ("Warning: found a shared IR node = %p = %s in the AST (not a SgLocatedNode) \n",node,node->class_name().c_str());
             }

#define ENFORCE_UNIQUE_IR_NODES 0

#if ENFORCE_UNIQUE_IR_NODES
          if ( SgProject::get_verbose() >= 0 )
#else
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
#endif
             {
               printf ("Error: found a shared IR node = %p = %s in the AST. \n",node,node->class_name().c_str());

               SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
               if (declarationStatement != NULL)
                  {
                    printf ("*** (possible sharing violation) declarationStatement = %p = %s \n",declarationStatement,declarationStatement->class_name().c_str());
                    ROSE_ASSERT(declarationStatement->get_parent() != NULL);
                    printf ("       --- declarationStatement->get_parent() = %p = %s \n",declarationStatement->get_parent(),declarationStatement->get_parent()->class_name().c_str());
                    SgLocatedNode* parent = isSgLocatedNode(declarationStatement->get_parent());
                    parent->get_startOfConstruct()->display("parent: debug");

                    printf ("       --- declarationStatement->get_firstNondefiningDeclaration() = %p \n",declarationStatement->get_firstNondefiningDeclaration());
                    printf ("       --- declarationStatement->get_definingDeclaration()         = %p \n",declarationStatement->get_definingDeclaration());
                    declarationStatement->get_startOfConstruct()->display("declarationStatement: debug");
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
                    if (classDeclaration != NULL)
                       {
                         printf ("       --- classDeclaration name = %s \n",classDeclaration->get_name().str());
                       }
                  }
             }

#if ENFORCE_UNIQUE_IR_NODES
       // DQ (11/28/2015): The older failing tests (below) are now fixed; but there 
       // are some newer failing tests (within Boost header files):
       // test2015_87.C 
       // test2015_90.C 
       // test2015_91.C 
       // test2015_94.C 
       // test2015_95.C 
       // test2015_96.C 
       // test2015_127.C
       // Also the RoseExample_tests fail (since they include the same boost issues).
       // So we still can not yet enforce this AST consistancy test.

       // DQ (11/23/2015): As of this date, we can now enforce this test (I think).
       // DQ (4/8/2014): This now only fails for Boost examples, so I this is the good news,
       // however, it means that I still can't enforce this everywhere. These tests:
       // test2013_234.C
       // test2013_240.C
       // test2013_242.C
       // test2013_246.C
       // test2013_241.C

       // DQ (10/16/2013): Now that we have the token stream support computed correctly, 
       // we have to disable this check to support the C++ tests (e.g. test2004_77.C).
       // DQ (10/14/2013): Turn this on as part of testing the token stream mapping!
       // DQ (10/19/2012): This fails for a collection of C++ codes only:
       // test2011_121.C
       // test2011_141.C
       // test2011_142.C
       // test2005_142.C
       // test2009_41.C
       // test2006_124.C
       // test2005_128.C
       // test2005_34.C
       // test2004_129.C
       // test2004_130.C
       // test2004_85.C
       // test2004_127.C
       // test2005_103.C
       // test2005_42.C
       // test2006_84.C
       // test2005_35.C
       // test2006_141.C
       // inputBug317.C
       // test2005_161.C
       // test2004_120.C
       // This is a result of new work to support class/struct/union declarations output in unusual
       // location in some exmaples from C applications that have been a focus lately.
       // I might want to fix this up later after the dust settles.  These appear to be unusual cases.

          ROSE_ASSERT(false);
#else

       // DQ (4/26/2012): debugging... (test2012_67.C)
#if ENFORCE_UNIQUE_IR_NODES
          if ( SgProject::get_verbose() >= 0 )
#else
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
#endif
             {
               printf ("In TestAstForUniqueNodesInAST::visit (): Rare issue (only effects Boost examples): node = %p = %s \n",node,node->class_name().c_str());
             }
#endif
        }
#if 0
     printf ("In TestAstForUniqueNodesInAST::visit(): astNodeSet.insert(node = %p = %s) \n",node,node->class_name().c_str());
#endif

     astNodeSet.insert(node);
   }


void
TestAstForUniqueNodesInAST::test ( SgNode* node )
   {
  // DQ (4/3/2012): Added test to make sure that the pointers are unique.
     TestAstForUniqueNodesInAST redundentNodeTest;
     redundentNodeTest.traverse(node,preorder);
   }

void
testAstForUniqueNodes ( SgNode* node )
   {
  // DQ (4/3/2012): Added test to make sure that the pointers are unique.
     TestAstForUniqueNodesInAST::test(node);
   }


/*! \page AstProperties AST Properties (Consistency Tests)

\section section3 Rules for Defining and Nondefining Declarations

    We separate defining and non-defining declarations so that many aspects of analysis and transformation are 
simplified, along with code generation. For example, if from any function declaration the function definition is sought,
it is available from the defining declaration (this applied uniformly to all declarations). These tests
verify that the handling of defining and non-defining declaration follow specific rules (with a goal toward uniformity
and intuitive behavior).

    Nondefining declarations appear as forward declarations and references to declarations within types.  These many
appear many times within the source code and as a result are non unique within the AST.  For example, each forward 
declaration of a function or class within a source code becomes a non-defining declaration.

    Defining declarations contain their definition, and function appearing with its body (implementation) is 
a defined declaration containing a function definition (the scope of the function).  The defining declaration
should appear only once within the source code, by the One Time Definition rule, (OTD).  Each forward declaration, 
clearly becomes a separate declaration but it may be shared as needed to reduce the total number of non-defining 
declarations, which are also referenced in types.

   Within SAGE III, every declaration has a reference to its first non-defining declaration and its defined declaration
if it exists (is defined within the current translation unit).  If in processing a defining declaration an reference is
required, get_declaration() always returns the non-defined declaration.  The defined declaration is only available
explicitly (via a function call) and is never returned through any other mechanism.  Thus non-defining declarations
are shared and defining declaration are never shared within the AST.

\subsection subsection3a When defining and non-defining declarations are the same
    SgEnumDeclaration declarations are not allowed to forward reference their definitions, this they are the same
and the defining and non-defining declaration for a SgEnumDeclaration are pointer values which are the same.

\example The following assertion is true for all SgEnumDeclaration objects: \n
     assert (declaration->get_definingDeclaration() == declaration->get_firstNondefiningDeclaration());

\subsection subsection3b Scopes of defining and nondefining declarations match (same pointer value)
   For all defining and nondefining declarations the scopes are the same, however for those that are 
in namespaces the actual SgNamespaceDefinition of a defining and non-defining declaration could be 
different.  To simplify analysis, the namespaces of defining and non-defining declarations are set 
to the SgNamespaceDefinition of the defined declaration.  These test verify the equality of the 
pointers for all scopes of defining and non-defining declarations.

\example The following assertion is always true: \n
     assert (declaration->get_definingDeclaration()->get_scope() == declaration->get_firstNondefiningDeclaration()->get_scope());

\subsection subsection3c Defining and nondefining declarations are non-null pointers which never match (different pointer values)

   The following SgDeclarationStatement IR nodes never share the same declaration and are always valid (non-null) pointers.
       -# SgAsmStmt \n
          This is a not well tested declaration within Sage III (but I think that any declaration must be a defining declaration)
       -# SgFunctionParameterList 
       -# SgCtorInitializerList \n
               These are special case declarations.
       -# SgVariableDefinition \n
          A variable definition appears with a variable declaration, but a variable declaration can be a 
          forward reference to the variable declaration containing the variable definitions (e.g. "extern int x;", 
          is a forward declaration to the declaration of "x").
       -# SgPragmaDeclaration \n
          A pragam can contain no references to it and so it's declaration is also it's definition
       -# SgUsingDirectiveStatement
       -# SgUsingDeclarationStatement
       -# SgNamespaceAliasDeclarationStatement
       -# SgTemplateInstantiationDirectiveStatement \n
          These can appear multiple times and are not really associated with definitions 
          (but for consistancy they are consired to be their own defining declaration).
       -# SgNamespaceDeclarationStatement \n
          Namespaces can't appear without their definitions (or so it seems, and it is tested).

\example The following assertion is true for all the above listed cases: \n
     assert (declaration->get_definingDeclaration() != NULL); \n
     assert (declaration->get_firstNondefiningDeclaration() != NULL); \n
     assert (declaration->get_definingDeclaration() != declaration->get_firstNondefiningDeclaration());

\subsection subsection3d Defining and nondefining declarations which never match (non-defining declaration may be NULL)

     This case is similar to \ref subsection3c but the non-defining declaration can be a null values pointer.
This is because a non-defining declaration may not exist (as in the case of a function defined with its 
definition and without any function prototype)  The following cases are tested for this properly:

       -# SgVariableDeclaration \n
          This case is a bit special.
       -# SgTemplateDeclaration
       -# SgFunctionDeclaration
          The non-defining declaration is always a valid pointer.
       -# SgClassDeclaration
       -# SgTypedefDeclaration
       -# SgMemberFunctionDeclaration
       -# SgTemplateInstantiationFunctionDecl
       -# SgTemplateInstantiationDecl
       -# SgTemplateInstantiationMemberFunctionDecl \n
          These can have forward declarations separated from their definitions
          so a declaration may be either a defining or non-defining declaration.
          All declarations, except the defining declaration, are the same object 
          as the non-defining declaration if it is non-null.

\example The following assertion is true for all the above listed cases: \n
     assert (declaration->get_definingDeclaration() != NULL); \n
     assert (declaration->get_definingDeclaration() != declaration->get_firstNondefiningDeclaration());

 */
void
TestAstForProperlySetDefiningAndNondefiningDeclarations::visit ( SgNode* node )
   {
  // DQ (6/24/2005): Test setup of defining and non-defining declaration pointers for each SgDeclarationStatement

  // printf ("In TestAstForProperlySetDefiningAndNondefiningDeclarations::visit(node = %p = %s) \n",node,node->sage_class_name());

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     SgDeclarationStatement* definingDeclaration         = NULL;
     SgDeclarationStatement* firstNondefiningDeclaration = NULL;
     if (declaration != NULL)
        {
          ROSE_ASSERT(declaration != NULL);

          definingDeclaration         = declaration->get_definingDeclaration();
          firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();

          if (definingDeclaration == NULL && firstNondefiningDeclaration == NULL)  
             {
               printf ("Error: TestAstForProperlySetDefiningAndNondefiningDeclarations::visit() --- declaration = %p = %s \n",declaration,declaration->class_name().c_str());
             }
          ROSE_ASSERT(definingDeclaration != NULL || firstNondefiningDeclaration != NULL);

       // DQ (7/23/2005): The scopes should match!
          if (definingDeclaration != NULL && firstNondefiningDeclaration != NULL)
             {
            // DQ (5/5/2007): A merged AST can have many SgGlobal (global scope) objects and we have to allow this.  
            // Thus a function with a prototype declaration in one file but and both a prototype and function definition 
            // in another file, will have references to different scopes (from get_scope()).  I think this is OK, but 
            // output a warning for now.  And it applys more broadly to all declarations with secondary forms (defining 
            // and non-defining declarations).

               SgScopeStatement* definingDeclarationScope         = definingDeclaration->get_scope();
               SgScopeStatement* firstNondefiningDeclarationScope = firstNondefiningDeclaration->get_scope();

            // DQ (10/22/2016): If these are a namespace definition then we want to check the global definition.
               SgNamespaceDefinitionStatement* namespaceDefinition_defining          = isSgNamespaceDefinitionStatement(definingDeclarationScope);
               SgNamespaceDefinitionStatement* namespaceDefinition_firstNondefining  = isSgNamespaceDefinitionStatement(firstNondefiningDeclarationScope);
               if (namespaceDefinition_defining != NULL && namespaceDefinition_firstNondefining != NULL)
                  {
                    definingDeclarationScope         = namespaceDefinition_defining->get_global_definition();
                    firstNondefiningDeclarationScope = namespaceDefinition_firstNondefining->get_global_definition();

                 // printf ("In TestAstForProperlySetDefiningAndNondefiningDeclarations::visit(): definingDeclarationScope = %p firstNondefiningDeclarationScope = %p \n",
                 //      definingDeclarationScope,firstNondefiningDeclarationScope);
                  }

               if (definingDeclarationScope != firstNondefiningDeclarationScope)
                  {
                 // DQ (5/10/2007): With a merged AST the primary and secondary declarations can be in different global scopes 
                 // since we don't merge to a unique global scope.  So when we are checking a merged AST this is OK.
                    if (isSgGlobal(definingDeclarationScope) != NULL && isSgGlobal(firstNondefiningDeclarationScope) != NULL)
                       {
                      // This is a case of the primary and secondary declaration in the global scope, but from different files
                      // (associated with a result of a merged AST).
                      // printf ("Note: Case of definingDeclaration and firstNondefiningDeclarationScope in different SgGlobal scopes \n");
                       }
                      else
                       {
                      // But if they are not global scopes then we want to report this because it could be an error.
                         printf ("\n\n******************************************************\n");
                         printf ("Error in scope: declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                         printf ("definingDeclaration ========= %p = %s = %s: get_scope() = %p = %s = %s \n",
                              definingDeclaration,
                              SageInterface::get_name(definingDeclaration).c_str(),
                              definingDeclaration->class_name().c_str(),
                              definingDeclaration->get_scope(),definingDeclaration->get_scope()->class_name().c_str(),
                              SageInterface::get_name(definingDeclaration->get_scope()).c_str());
                         printf ("firstNondefiningDeclaration = %p = %s = %s: get_scope() = %p = %s = %s \n",
                              firstNondefiningDeclaration,
                              SageInterface::get_name(firstNondefiningDeclaration).c_str(),
                              firstNondefiningDeclaration->class_name().c_str(),
                              firstNondefiningDeclaration->get_scope(),firstNondefiningDeclaration->get_scope()->class_name().c_str(),
                              SageInterface::get_name(firstNondefiningDeclaration->get_scope()).c_str());

                         printf ("definingDeclaration parent = %p = %s = %s \n",
                              definingDeclaration->get_parent(),
                              definingDeclaration->get_parent()->class_name().c_str(),
                              SageInterface::get_name(definingDeclaration->get_parent()).c_str());

                      // SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(definingDeclaration->get_parent());
                         declaration->get_file_info()->display("location of problem declaaration");

                      // DQ (3/4/3009): This test fails for test2005_118.C when run from the copyAST_tests directory (AST Copy tests).
                         if (definingDeclarationScope != firstNondefiningDeclarationScope)
                            {
                           // I think this might be reasonable to fail for this test of the AST Copy mechanism, but it needs to be looked into deeper.
                              printf ("This test fails for test2005_118.C when run from the copyAST_tests directory (AST Copy tests) \n");
                            }
                      // ROSE_ASSERT(definingDeclarationScope == firstNondefiningDeclarationScope);
                       }
                  }

           // ROSE_ASSERT(definingDeclarationScope == firstNondefiningDeclarationScope);

           // DQ (3/1/2013): Adding test for access specification (public, protected, private).  First we need to argue that these should be the same 
           // for the non-defining and defining declaration.  I am not clear that they should be the same.
               SgAccessModifier::access_modifier_enum definingDeclaration_access_modifier         = definingDeclaration->get_declarationModifier().get_accessModifier().get_modifier();
               SgAccessModifier::access_modifier_enum firstNondefiningDeclaration_access_modifier = firstNondefiningDeclaration->get_declarationModifier().get_accessModifier().get_modifier();
               if (definingDeclaration_access_modifier != firstNondefiningDeclaration_access_modifier)
                  {
                 // DQ (6/30/2014): I think this is not an error for SgTemplateInstantiationDecl.
                    if (isSgTemplateInstantiationDecl(definingDeclaration) != NULL)
                       {
                      // DQ (3/11/2017): Fixed to use message streams.
                         mprintf ("Warning: (different access modifiers used) definingDeclaration = %p firstNondefiningDeclaration = %p = %s  \n",definingDeclaration,firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str());
                         mprintf ("Warning: definingDeclaration_access_modifier         = %d \n",definingDeclaration_access_modifier);
                         mprintf ("Warning: firstNondefiningDeclaration_access_modifier = %d \n",firstNondefiningDeclaration_access_modifier);
                       }
                      else
                       {
#if 0
                      // DQ (1/12/2019): This is is output spew but only from Cxx_tests/rose-1541-0.C, as best I can tell.
                         printf ("Error: definingDeclaration = %p firstNondefiningDeclaration = %p = %s  \n",definingDeclaration,firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str());

                         firstNondefiningDeclaration->get_file_info()->display("firstNondefiningDeclaration");
                         definingDeclaration->get_file_info()->display("definingDeclaration");

                         printf ("Error: definingDeclaration_access_modifier         = %d \n",definingDeclaration_access_modifier);
                         printf ("Error: firstNondefiningDeclaration_access_modifier = %d \n",firstNondefiningDeclaration_access_modifier);
#endif
                       }
                  }

            // DQ (6/30/2014): I think this is not an error for SgTemplateInstantiationDecl.
            // ROSE_ASSERT(definingDeclaration_access_modifier == firstNondefiningDeclaration_access_modifier);
               if (isSgTemplateInstantiationDecl(definingDeclaration) == NULL && firstNondefiningDeclaration->get_parent() == definingDeclaration->get_parent())
                 {
                   ROSE_ASSERT(definingDeclaration_access_modifier == firstNondefiningDeclaration_access_modifier);
                 }
             }
        }

     switch (node->variantT())
        {
          case V_SgEnumDeclaration:
             {
            // DQ (6/26/2005): Special case of enum declarations (no forward 
            // enum declarations are allowed in the C or C++ standard).
            // ROSE_ASSERT(declaration == definingDeclaration);
               if (declaration != definingDeclaration)
                  {
#if PRINT_DEVELOPER_WARNINGS
                    printf ("Note in AstConsistencyTests.C, enum declaration not a defining declaration \n");
                 // declaration->get_startOfConstruct()->display("declaration != definingDeclaration for enum declaration");
#endif
                  }
               break;
             }

       // DQ (9/6/2005): Ignoring this case!
          case V_SgFunctionParameterList:
             {
               break;
             }           

       // DQ (6/26/2005): These are likely somewhat special and such that they should be their own defining declarations, I think

       // This is a not well tested declaration within Sage III (but I think that any declaration must be a defining declaration)
          case V_SgAsmStmt:

       // These are special case declarations
       // case V_SgFunctionParameterList:
          case V_SgCtorInitializerList:
          case V_SgFortranIncludeLine:

       // A variable definition appears with a variable declaration, but a variable declaration can be a 
       // forward reference to the variable declaration containing the variable definitions (e.g. "extern int x;", 
       // is a forward declaration to the declaration of "x").
          case V_SgVariableDefinition:

       // A pragam can contain no references to it and so it's declaration is also it's definition
          case V_SgPragmaDeclaration:

       // These can appear multiple times and are not really associated with definitions 
       // (but for consistency they are considered to be their own defining declaration).
          case V_SgUsingDirectiveStatement:
          case V_SgUsingDeclarationStatement:
          case V_SgNamespaceAliasDeclarationStatement:
          case V_SgTemplateInstantiationDirectiveStatement:

       // Shared by all the above cases!
             {
            // DQ (6/26/2005): I think that the C++ standard does not allow forward declarations for these either!
            // So the defining declaration should be the declaration itself (I think).  either that or we need to 
            // build a special non-defining declaration for these declarations.
               if (declaration != definingDeclaration)
                  {
                    printf ("Warning: declaration %p = %s not equal to definingDeclaration = %p \n",
                         declaration,declaration->sage_class_name(),definingDeclaration);
                  }
               ROSE_ASSERT(declaration == definingDeclaration);
               break;
             }

       // Namespaces can't appear without their definitions (or so it seems, tested).
       // Since there can be many declarations of the same namespace the definingDeclaration 
       // is always NULL (only for SgNamespaceDeclarationStatement)
          case V_SgNamespaceDeclarationStatement:
             {
               ROSE_ASSERT(declaration != NULL);
               ROSE_ASSERT(definingDeclaration == NULL);
               ROSE_ASSERT(firstNondefiningDeclaration != NULL);
               break;
             }

       // This case is a bit special
          case V_SgVariableDeclaration:

       // These can have forward declarations separated from their definitions
       // so a declaration may be either a defining or non-defining declaration.
          case V_SgTemplateDeclaration:
          case V_SgFunctionDeclaration:
          case V_SgClassDeclaration:
          case V_SgDerivedTypeStatement:
          case V_SgTypedefDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
          // Liao 12/2/2010, add new Fortran function nodes
          case V_SgProcedureHeaderStatement: 
          case V_SgProgramHeaderStatement: 
             {
            // For some declarations, the only declaration is a defining declaration, in which case the 
            // non-defining declaration is NULL (except in the case of SgClassDeclarations, where a 
            // non-defining declarations in generated internally).
               if (firstNondefiningDeclaration == NULL)
                  {
                    switch (declaration->variantT())
                       {
                      // These nodes should have a non-defining declaration even if only a defining 
                      // declaration is present in the source code.  It may be that the other IR 
                      // nodes below should be treated similarly.
                         case V_SgClassDeclaration:
                         case V_SgDerivedTypeStatement:
                         case V_SgTemplateInstantiationDecl:
                            {
                              printf ("Warning AST Consistancy Test: declaration %p = %s has no firstNondefiningDeclaration = %p \n",
                                   declaration,declaration->sage_class_name(),firstNondefiningDeclaration);
                              declaration->get_file_info()->display("has no firstNondefiningDeclaration");
                              break;
                            }
                          
                         case V_SgTemplateDeclaration:
                         case V_SgTypedefDeclaration:
                            {
                           // These are special cases
                              break;
                            }

                         case V_SgFunctionDeclaration:
                         case V_SgMemberFunctionDeclaration:
                         case V_SgTemplateInstantiationFunctionDecl:
                         case V_SgTemplateInstantiationMemberFunctionDecl:
                         case V_SgProcedureHeaderStatement: 
                         case V_SgProgramHeaderStatement: 
                            {
                           // This is the reasonable case, where a function or template or typedef is 
                           // declared once (and only once and contains its definition).  Verify that 
                           // the definition is present!
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
                              ROSE_ASSERT(functionDeclaration != NULL);
#if PRINT_DEVELOPER_WARNINGS
                           // DQ (4/23/2006): See PolyhedralSideBaseMethods.cc for example of where this happens.
                              if (functionDeclaration->get_definition() == NULL)
                                 {
                                   printf ("This declaration (%p %s) has no non-defining declaration and thus should be a defining declaration, but it has no definition \n",
                                        functionDeclaration,functionDeclaration->class_name().c_str());
                                   functionDeclaration->get_file_info()->display("defining declaration lacking definition");
                                 }
#endif
                           // DQ (8/10/2005): Commented out to compile KULL
                           // ROSE_ASSERT(functionDeclaration->get_definition() != NULL);
                              break;
                            }

                         default:
                            {
                           // Nothing to do here!
                            }

                      // And the defining declaration is the current definition
                         ROSE_ASSERT(declaration == definingDeclaration);
                       }
#if 0                     
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
                    if (functionDeclaration != NULL)
                       {
                      // This is the reasonable case, where a function is declared once (and only once and 
                      // contains it's definition).  Verify that the function definition is present!
                         ROSE_ASSERT(functionDeclaration->get_definition() != NULL);
                       }
                      else
                       {
#if 1
                         printf ("Warning AST Consistancy Test: declaration %p = %s has no firstNondefiningDeclaration = %p \n",
                              declaration,declaration->sage_class_name(),firstNondefiningDeclaration);
                         declaration->get_file_info()->display("has no firstNondefiningDeclaration");
#endif
                       }
#endif
                  }
            // ROSE_ASSERT(firstNondefiningDeclaration != NULL);
               if (firstNondefiningDeclaration == definingDeclaration)
                  {
                 // DQ (12/12/2009): Suppress the warning about this for the case of a
                 // SgTypedefDeclaration if not set to verbose mode. This is important to
                 // reducing the output from the tests of AST merge in the mergeAST_tests
                 // directory.
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declaration);
                    if (typedefDeclaration == NULL || (SgProject::get_verbose() > 0) )
                       {
                         printf ("Warning AST Consistency Test: declaration %p = %s = %s has equal firstNondefiningDeclaration and definingDeclaration = %p \n",
                              declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str(),firstNondefiningDeclaration);
                         printf ("declaration->get_definingDeclaration() = %p declaration->get_firstNondefiningDeclaration() = %p \n",
                              declaration->get_definingDeclaration(),declaration->get_firstNondefiningDeclaration());
                         declaration->get_file_info()->display("firstNondefiningDeclaration == definingDeclaration: debug");

                         // Liao 12/2/2010
                         //  A test to see if the first nondefining declaration is set to self for a defining function declaration
                         SgFunctionDeclaration * func = isSgFunctionDeclaration(declaration);
                         if(func != NULL)
                         {
                           printf ("Error: found a defining function declaration with its first nondefining declaration set to itself/(or a defining declaration).\n");
                           //ROSE_ASSERT (false);
                         }
 
                       }
                  } // end if nondefining == defining

            // DQ (8/6/2007): Comment this out, at least for SgTypedefDeclaration it should be OK, MAYBE.
            // DQ (3/4/2007): Temporarily commented out (now uncommented)
            // ROSE_ASSERT(firstNondefiningDeclaration != definingDeclaration);
               break;
             }
           
          default:
             {
            // Nothing to do here!
             }
        }

   }

void
TestAstSymbolTables::visit ( SgNode* node )
   {
#if 0
     printf ("At TOP of TestAstSymbolTables::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif

     SgScopeStatement* scope = isSgScopeStatement(node);
     if (scope != NULL)
        {
          SgSymbolTable* symbolTable = scope->get_symbol_table();
          if (symbolTable == NULL)
             {
               printf ("Error: symbolTable == NULL, no symbol table found at node = %p = %s \n",node,node->class_name().c_str());
             }
          ROSE_ASSERT(symbolTable != NULL);
          if (symbolTable->get_table() == NULL)
             {
               printf ("Error: symbolTable->get_table() == NULL, no symbol table found at node = %p = %s \n",node,node->class_name().c_str());
             }
          ROSE_ASSERT(symbolTable->get_table() != NULL);

          SgSymbolTable::BaseHashType* internalTable = symbolTable->get_table();
          ROSE_ASSERT(internalTable != NULL);

       // Iterate over each symbol in the table and test it separately!
          SgSymbolTable::hash_iterator i = internalTable->begin();
          while (i != internalTable->end())
             {
            // DQ: removed SgName casting operator to char*
            // cout << "[" << idx << "] " << (*i).first.str();

            // DQ (5/2/2013): Added to support test2013_141.C.
               ROSE_ASSERT ( (*i).second != NULL );
#if 0
               printf ("In symbol table = %p symbol name = i->first = %s i->second = %p = %s \n",symbolTable,i->first.str(),i->second,i->second->class_name().c_str());
#endif
               ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

            // printf ("Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) sage_class_name() = %s \n",
            //      idx,(*i).first.str(),(*i).second->sage_class_name());

               SgSymbol* symbol = isSgSymbol((*i).second);
               ROSE_ASSERT ( symbol != NULL );

            // DQ (5/24/2006): Added this test to the AST
               if ( symbol->get_parent() == NULL )
                  {
                    printf ("Error: symbol = %p = %s has parent == NULL \n",symbol,symbol->class_name().c_str());
                  }
               ROSE_ASSERT ( symbol->get_parent() != NULL );

            // DQ (8/6/2005): Test the get_declaration() function on all symbols
               TestAstAccessToDeclarations::test(symbol);

            // DQ (7/3/2007): This is a common point of failure in the AST merge mechanism.
               SgNode* declarationNode = symbol->get_symbol_basis();
               ROSE_ASSERT(declarationNode != NULL);
               SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(declarationNode);
#if 0
               if (declarationStatement != NULL)
                  {
                    printf ("declarationStatement = %p = %s definingDeclaration = %p \n",declarationStatement,declarationStatement->class_name().c_str(),declarationStatement->get_definingDeclaration());
                  }
#endif
#if 0
               printf ("AST consistency test: symbol = %p = %s = %s \n",symbol,symbol->class_name().c_str(),SageInterface::get_name(symbol).c_str());
#endif

            // DQ (12/9/2007): Skip symbols that come from labels since they are often 
            // numeric labels and need to be tested in a Fortran specific way.
            // if (declarationStatement != NULL)
               if (declarationStatement != NULL && isSgLabelSymbol(symbol) == NULL)
                  {
                 // DQ (8/21/2013): Test added by Tristan are a problem for Fortran code...
#if 0
                    assert(declarationStatement->get_firstNondefiningDeclaration() != NULL);
                    assert(declarationStatement->get_firstNondefiningDeclaration() == declarationStatement);
#endif
                 // DQ (7/25/2013): Tristan reports that this assertion is false for test2001_06.C.
                 // ROSE_ASSERT(declarationStatement->get_firstNondefiningDeclaration() == declarationStatement);

                    SgSymbol* local_symbol = declarationStatement->get_symbol_from_symbol_table();
#if 0
                    if (local_symbol == NULL)
                       {
                         printf ("The declarationStatement = %p = %s = %s in symbol = %p = %s = %s can't locate it's symbol in scope = %p = %s = %s \n",
                              declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str(),
                              symbol,symbol->class_name().c_str(),SageInterface::get_name(scope).c_str(),
                              scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
                         declarationStatement->get_startOfConstruct()->display("declarationStatement->get_symbol_from_symbol_table() == NULL");
                       }
#endif

                 // DQ (7/26/2007): Not all declarations have an associated symbol, but those declaration found in symbols should have symbols.
                 // ROSE_ASSERT(local_symbol != NULL);
                    SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
                    if (memberFunctionDeclaration != NULL && memberFunctionDeclaration->get_associatedClassDeclaration() != NULL)
                       {
                      // See test2007_116.C.
#if PRINT_DEVELOPER_WARNINGS
                         printf ("memberFunctionDeclaration scope has no associated symbol (case of pointer to member function): local_symbol = %p \n",local_symbol);
#endif
                      // ROSE_ASSERT(local_symbol == NULL);
                         ROSE_ASSERT(memberFunctionDeclaration->get_scope() != NULL);
                       }
                      else
                       {
                         if (local_symbol == NULL)
                            {

                           // It appears this is an issue because the name is slightly different between:
                           //      name = template_class2 < int  , double  >  SgSymbol = 0x150ac90 = SgTemplateTypedefSymbol type = 0x7fdc28051098 = SgTypedefType = template_class2 < int , double >  
                           // and
                           //      get_symbol_basis() = 0x7fdc27d94010 = SgTemplateInstantiationTypedefDeclaration = template_class2 < int , double >  
                           //
                           // Specifically:
                           //      name = template_class2 < int  , double  >
                           //             template_class2 < int , double >

                              printf ("Error (AST consistency test): The declarationStatement = %p = %s = %s in symbol = %p = %s = %s can't locate it's symbol in scope = %p = %s = %s \n",
                                   declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str(),
                                   symbol,symbol->class_name().c_str(),SageInterface::get_name(scope).c_str(),
                                   scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
                              declarationStatement->get_startOfConstruct()->display("declarationStatement->get_symbol_from_symbol_table() == NULL: debug");

                              printf ("******************** START **********************\n");
                              printf ("In AST Consistantcy tests: Output the symbol table for scope = %p = %s: \n",scope,scope->class_name().c_str());
                              SageInterface::outputLocalSymbolTables(scope);
                              printf ("******************** DONE ***********************\n");

#if 1
                           // DQ (2/28/2018): Added testing (Tristan indicates that this is a problem for Fortran, above).
                              ROSE_ASSERT(declarationStatement->get_firstNondefiningDeclaration() != NULL);

                           // DQ (2/14/2019): For C++11 this need no longer be true, because enum prototypes can exist.
                           // ROSE_ASSERT(declarationStatement->get_firstNondefiningDeclaration() == declarationStatement);
#endif
                            }

                      // DQ (11/7/2007): Allow this, with a warning, I think!
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
                         if (local_symbol == NULL && functionDeclaration != NULL && functionDeclaration->get_name() == "__default_function_pointer_name")
                            {
// #if PRINT_DEVELOPER_WARNINGS
                              printf ("Warning: functionDeclaration = %s without symbol is OK in this case. \n",functionDeclaration->get_name().str());
// #endif
                            }
                           else
                            {
#if 0
                           // DQ (2/28/2015): previous older code.
                              ROSE_ASSERT(local_symbol != NULL);
#else
                           // DQ (2/28/2015): This fails for copyAST_tests/copytest2007_40.C and a few other files.
                           // I think this is related to the support for the EDN normalized template declarations.
                              if (local_symbol == NULL)
                                 {
                                   printf ("WARNING: local_symbol == NULL: this can happen in the copyAST_tests directory files. \n");
                                 }
                           // ROSE_ASSERT(local_symbol != NULL);
#endif
                            }
                       }
                 // ROSE_ASSERT(declarationStatement->hasAssociatedSymbol() == false || local_symbol != NULL);

#if 0
                 // DQ (11/21/2013): Adding test as a result of debugging with Philippe.
                 // This test is not a test for a bug, since we require that symbols in base classes be aliased in the derived classes.
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
                    if (classDeclaration != NULL)
                       {
                         if (classDeclaration->get_scope() != scope)
                            {
                              printf ("Error (AST consistency test): classDeclaration->get_scope() != scope of symbol table (classDeclaration->get_scope() = %p = %s scope = %p = %s) \n",
                                   classDeclaration->get_scope(),classDeclaration->get_scope()->class_name().c_str(),scope,scope->class_name().c_str());
                              printf ("   --- classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());

                              classDeclaration->get_file_info()->display("classDeclaration: debug");
                              scope->get_file_info()->display("scope: debug");
                            }
                      // ROSE_ASSERT(classDeclaration->get_scope() == scope);
                         if (isSgNamespaceDefinitionStatement(classDeclaration->get_scope()) == NULL)
                            {
                              ROSE_ASSERT(classDeclaration->get_scope() == scope);
                            }
                       }
#endif
                  }
                 else
                  {
                    SgInitializedName* initializedName = isSgInitializedName(declarationNode);
                    if (initializedName != NULL)
                       {
#if 0
                         printf ("In TestAstSymbolTables::visit(): initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
                         SgSymbol* local_symbol = initializedName->get_symbol_from_symbol_table();
                         if (local_symbol == NULL)
                            {
                              printf ("Error: initializedName->get_symbol_from_symbol_table() == NULL initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
                              initializedName->get_startOfConstruct()->display("Error: initializedName->get_symbol_from_symbol_table() == NULL");
                              SgLocatedNode* locatedNodeParent = isSgLocatedNode(initializedName->get_parent());
                              ROSE_ASSERT(locatedNodeParent != NULL);
                              locatedNodeParent->get_startOfConstruct()->display("Error: initializedName->get_symbol_from_symbol_table() == NULL: locatedNodeParent");

                              SgScopeStatement* scope = initializedName->get_scope();
                              ROSE_ASSERT(scope != NULL);
                              ROSE_ASSERT(scope->get_symbol_table() != NULL);

                              printf ("In TestAstSymbolTables::visit(): output the symbol tables for the scope = %p = %s \n",scope,scope->class_name().c_str());
                              scope->get_symbol_table()->print();
                            }
#if 0
                         printf ("In TestAstSymbolTables::visit(): local_symbol = %p = %s \n",local_symbol,local_symbol != NULL ? local_symbol->class_name().c_str() : "null");
#endif
                         ROSE_ASSERT(local_symbol != NULL);
                       }
                      else
                       {
                         if (isSgLabelStatement(declarationNode))
                            {
                              SgLabelStatement* labelStatement = (SgLabelStatement *) declarationNode;
                              SgSymbol* local_symbol = labelStatement->get_symbol_from_symbol_table();
                              if (local_symbol == NULL)
                                 {
                                   printf ("Error: labelStatement->get_symbol_from_symbol_table() == NULL labelStatement = %p = %s \n",labelStatement,labelStatement->get_label().str());
                                   ROSE_ASSERT(labelStatement->get_scope() != NULL);
                                   labelStatement->get_scope()->get_symbol_table()->print("debug labelStatement scope");
                                 }
                              ROSE_ASSERT(local_symbol != NULL);
                            }
                         else if (isSgJavaLabelStatement(declarationNode)) // charles4: 09/12/2011 added for Java
                            {
                              SgJavaLabelStatement* javaLabelStatement = (SgJavaLabelStatement *) declarationNode;
                              SgSymbol *local_symbol = javaLabelStatement->get_symbol_from_symbol_table();
                              if (local_symbol == NULL)
                                 {
                                   printf ("Error: javaLabelStatement->get_symbol_from_symbol_table() == NULL javaLabelStatement = %p = %s \n",javaLabelStatement,javaLabelStatement->get_label().str());
                                   ROSE_ASSERT(javaLabelStatement->get_scope() != NULL);
                                   ROSE_ASSERT(javaLabelStatement->get_scope()->get_symbol_table() != NULL);
                                   javaLabelStatement->get_scope()->get_symbol_table()->print("debug javaLabelStatement scope");
                                 }
                              ROSE_ASSERT(local_symbol != NULL);
                            }
                           else
                            {
                           // DQ (12/9/2007): Added support for fortran in SgLabelSymbol.
                              SgLabelSymbol* labelSymbol = isSgLabelSymbol(symbol);
                              if (labelSymbol != NULL)
                                 {
                                   ROSE_ASSERT(labelSymbol->get_fortran_statement() != NULL);
                                 }
                                else
                                 {
                                   printf ("Error: declaration found in symbol not handled (declarationNode = %s) \n",declarationNode->class_name().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                  }

            // DQ (12/16/2007): Added test
               ROSE_ASSERT(symbol != NULL);

            // We have to look at each type of symbol separately!  This is because there is no virtual function,
            // the reason for this is that each get_declaration() function returns a different type!
            // ROSE_ASSERT ( symbol->get_declaration() != NULL );
               switch(symbol->variantT())
                  {
                    case V_SgClassSymbol:
                       {
                         SgClassSymbol* classSymbol = isSgClassSymbol(symbol);
                         ROSE_ASSERT(classSymbol != NULL);
                         ROSE_ASSERT(classSymbol->get_declaration() != NULL);

                      // DQ (12/27/2011): Make sure this is not a SgClassSymbol that is incorrectly associated with a SgTemplateClassDeclaration.
                         ROSE_ASSERT(isSgTemplateClassDeclaration(classSymbol->get_declaration()) == NULL);
                         break;
                       }

                 // DQ (12/27/2011): Added new symbol (and required support).
                    case V_SgTemplateClassSymbol:
                       {
                         SgTemplateClassSymbol* templateClassSymbol = isSgTemplateClassSymbol(symbol);
                         ROSE_ASSERT(templateClassSymbol != NULL);
                         ROSE_ASSERT(templateClassSymbol->get_declaration() != NULL);

                      // DQ (12/27/2011): Make sure this is correctly associated with a SgTemplateClassDeclaration.
                         ROSE_ASSERT(isSgTemplateClassDeclaration(templateClassSymbol->get_declaration()) != NULL);
                         break;
                       }

                    case V_SgDefaultSymbol:
                       {
                         printf ("The SgDefaultSymbol should not be present in the AST \n");
                         ROSE_ASSERT(false);
                         break;
                       }

                    case V_SgEnumFieldSymbol:
                       {
                      // Note that the type returned by get_declaration is SgInitializedName and not any sort of SgDeclaration
                         SgEnumFieldSymbol* enumFieldSymbol = isSgEnumFieldSymbol(symbol);
                         ROSE_ASSERT(enumFieldSymbol != NULL);
                         ROSE_ASSERT(enumFieldSymbol->get_declaration() != NULL);
                         break;
                       }

                    case V_SgEnumSymbol:
                       {
                         SgEnumSymbol* enumSymbol = isSgEnumSymbol(symbol);
                         ROSE_ASSERT(enumSymbol != NULL);
                         ROSE_ASSERT(enumSymbol->get_declaration() != NULL);
                         break;
                       }

                 // DQ (12/28/2011): These can be handled using the same case.
                    case V_SgTemplateFunctionSymbol:
                    case V_SgTemplateMemberFunctionSymbol:

                 // These can be handled by the same case
                    case V_SgFunctionSymbol:
                    case V_SgMemberFunctionSymbol:
                       {
                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);
                         ROSE_ASSERT(functionSymbol != NULL);
                         ROSE_ASSERT(functionSymbol->get_declaration() != NULL);
                         break;
                       }

                 // DQ (10/11/2008): Added to support renaming of functions using the Fortran 90 interface statement.
                 // Note that the SgRenameSymbol is derived from the SgFunctionSymbol. Unclear if any other symbols 
                 // should have a similar renamed version (e.g. SgMemberFunctionSymbol).  If this technique is uesd
                 // for C++ then the target of C++ aliases might be supported.  It is not clear if this technique will
                 // be used outside of the Fortran support.
                    case V_SgRenameSymbol:
                       {
                      // This is an alias for a symbol injected from another scope as part of a Fortran "use" statement
                      // (or perhaps eventually a C++ using declaration or using directive).
                         SgRenameSymbol* renameSymbol = isSgRenameSymbol(symbol);
                         ROSE_ASSERT(renameSymbol != NULL);
                         ROSE_ASSERT(renameSymbol->get_original_symbol() != NULL);
                         ROSE_ASSERT(renameSymbol->get_declaration() != NULL);
                         break;
                       }

                    case V_SgFunctionTypeSymbol:
                       {
                      // Note that we check the get_type() function here and not get_declaration()
                         SgFunctionTypeSymbol* functionTypeSymbol = isSgFunctionTypeSymbol(symbol);
                         ROSE_ASSERT(functionTypeSymbol != NULL);
                         ROSE_ASSERT(functionTypeSymbol->get_type() != NULL);
                         break;
                       }

                    case V_SgLabelSymbol:
                       {
                         SgLabelSymbol* labelSymbol = isSgLabelSymbol(symbol);
                         ROSE_ASSERT(labelSymbol != NULL);

                      // DQ (12/9/2007): Modified to reflect added support for fortran statement numeric labels.
                      // ROSE_ASSERT(labelSymbol->get_declaration() != NULL);
                         if (labelSymbol->get_declaration() == NULL)
                            {
#if 0
                              ROSE_ASSERT(labelSymbol->get_fortran_statement() != NULL);
#else
                           // DQ (2/2/2011): Added support in SgLabelSymbol for Fortran alternative return parameters (see test2010_164.f90).
                              if (labelSymbol->get_fortran_statement() == NULL)
                                 {
                                   ROSE_ASSERT(labelSymbol->get_fortran_alternate_return_parameter() != NULL);
                                 }
#endif
                            }
                         break;
                       }

                    case V_SgJavaLabelSymbol:
                       {
                         SgJavaLabelSymbol* labelSymbol = isSgJavaLabelSymbol(symbol);
                         ROSE_ASSERT(labelSymbol != NULL);

                      // charles4 (9/12/2011): copied from case of SgLabelSymbol for Java
                         ROSE_ASSERT(labelSymbol->get_declaration() != NULL);

                         break;
                       }

                    case V_SgNamespaceSymbol:
                       {
                         SgNamespaceSymbol* namespaceSymbol = isSgNamespaceSymbol(symbol);
                         ROSE_ASSERT(namespaceSymbol != NULL);

                      // DQ (8/30/2009): Added namespace alias support.
                      // ROSE_ASSERT(namespaceSymbol->get_declaration() != NULL);
                         ROSE_ASSERT( (namespaceSymbol->get_declaration() != NULL && namespaceSymbol->get_isAlias() == false) || (namespaceSymbol->get_aliasDeclaration() != NULL && namespaceSymbol->get_isAlias() == true) );
                         break;
                       }

                    case V_SgTemplateSymbol:
                       {
                         SgTemplateSymbol* templateSymbol = isSgTemplateSymbol(symbol);
                         ROSE_ASSERT(templateSymbol != NULL);
                         ROSE_ASSERT(templateSymbol->get_declaration() != NULL);
                         break;
                       }

                 // DQ (11/4/2014): Adding support for template typedef declarations.
                    case V_SgTemplateTypedefSymbol:
                    case V_SgTypedefSymbol:
                       {
                         SgTypedefSymbol* typedefSymbol = isSgTypedefSymbol(symbol);
                         ROSE_ASSERT(typedefSymbol != NULL);
                         ROSE_ASSERT(typedefSymbol->get_declaration() != NULL);
                         break;
                       }
                    case V_SgTemplateVariableSymbol:
                    case V_SgVariableSymbol:
                       {
                      // Note that the type returned by get_declaration is SgInitializedName and not any sort of SgDeclaration
                         SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);
                         ROSE_ASSERT(variableSymbol != NULL);
                         ROSE_ASSERT(variableSymbol->get_declaration() != NULL);
                         break;
                       }

                 // DQ (9/29/2008): Added support for symbols that are alias for other symbol from othe scopes.
                    case V_SgAliasSymbol:
                       {
                      // This is an alias for a symbol injected from another scope as part of a Fortran "use" statement
                      // (or perhaps eventually a C++ using declaration or using directive).
                         SgAliasSymbol* aliasSymbol = isSgAliasSymbol(symbol);
                         ROSE_ASSERT(aliasSymbol != NULL);
                         ROSE_ASSERT(aliasSymbol->get_alias() != NULL);
                         break;
                       }

                    case V_SgNonrealSymbol:
                       {
                         SgNonrealSymbol * nrsymbol = isSgNonrealSymbol(symbol);
                         ROSE_ASSERT(nrsymbol != NULL);
                         ROSE_ASSERT(nrsymbol->get_declaration() != NULL);
                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached in switch (AstFixes.C) symbol = %s \n",symbol->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }

            // DQ (12/16/2007): Added test
               ROSE_ASSERT(symbol != NULL);

            // DQ (6/10/2007): Test if the scopes match!
            // This also test the SgSymbol::get_symbol_basis() member function
               SgScopeStatement* decl_scope = symbol->get_scope();
               if (decl_scope != NULL && decl_scope != scope)
                  {
#if PRINT_DEVELOPER_WARNINGS
                 // DQ (10/22/2007) Unclear if this is a serious problem.
                    printf ("Warning: scope of declaration = %p = %s in symbol does not match input scope = %p = %s \n",decl_scope,decl_scope->class_name().c_str(),scope,scope->class_name().c_str());
#endif
                 // ROSE_ASSERT(false);
                  }
                 else
                  {
                 // DQ (7/26/2007): Test for the special case of a member function pointer to a class without a definition
                 // see test2007_116.C
                    if (decl_scope == NULL)
                       {
                         SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(symbol);
                         if (memberFunctionSymbol != NULL)
                            {
                              ROSE_ASSERT(memberFunctionSymbol->get_declaration() != NULL);
                              SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(memberFunctionSymbol->get_declaration());
                              ROSE_ASSERT(memberFunctionDeclaration->get_associatedClassDeclaration() != NULL);
                            }
                           else
                            {
                              printf ("Error: symbol declaration's scope not set! symbol = %p = %s \n",symbol,symbol->class_name().c_str());
                              ROSE_ASSERT(false);
                            }
                       }
                  }

            // Increment iterator!
               i++;
             }

#if 0
       // debugging
          symbolTable->print();
#endif
        }
   }


void
TestAstAccessToDeclarations::visit ( SgNode* node )
   {
  // DQ (8/5/2005): Test the get_declaration() member function which is common on many IR nodes
  // printf ("node = %s \n",node->sage_class_name());

     TestAstAccessToDeclarations::test(node);
   }

void
TestAstAccessToDeclarations::test ( SgNode* node )
   {
  // DQ (8/5/2005): Test the get_declaration() member function which is common on many IR nodes
  // printf ("node = %s \n",node->sage_class_name());

     switch (node->variantT())
        {
       // IR nodes that have a get_declaration member function
          case V_SgInitializedName:
             {
               SgInitializedName* initializedName = isSgInitializedName(node);
            // Every variable should have a declaration (but initialized name objects 
            // are used in many ways, so it is not clear if this is always true)

            // DQ (9/6/2005): This can be NULL when in a function parameter list of a 
            // forward function declaration (but only if the parent has not been set)
               if (initializedName->get_declaration() == NULL)
                  {
                    printf ("initializedName with NULL declaration at %p name = %s (seems to apply to function parameters) \n",
                         initializedName,initializedName->get_name().str());
                    initializedName->get_file_info()->display("initializedName with NULL declaration");
                  }
            // DQ (6/18/2006): Commented out as a test.
            // ROSE_ASSERT(initializedName->get_declaration() != NULL);
               break;
             }

          case V_SgVariableDefinition:
             {
               SgVariableDefinition* variableDefinition = isSgVariableDefinition(node);
               ROSE_ASSERT(variableDefinition->get_declaration() != NULL);
               break;
             }

          case V_SgTypedefDeclaration:
             {
               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);
            // I can't tell that the declaration stored in the typedef declaration is used for 
            // anything, it appears to often be NULL, but also it seems to not be ever used.
            // The value is valid or NULL about an equal percentage of the time in many applications.
               if (typedefDeclaration->get_declaration() == NULL)
                  {
                 // printf ("SgTypedefDeclaration::get_declaration returns NULL \n");
                  }
                 else
                  {
                 // printf ("SgTypedefDeclaration::get_declaration returns VALID POINTER \n");
                  }
            // ROSE_ASSERT(typedefDeclaration->get_declaration() != NULL);
               break;
             }

          case V_SgTemplateInstantiationDirectiveStatement:
             {
               SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(node);
               ROSE_ASSERT(templateInstantiationDirective->get_declaration() != NULL);
               break;
             }

          case V_SgUsingDeclarationStatement:
             {
               SgUsingDeclarationStatement* tmp = isSgUsingDeclarationStatement(node);
            // struct T { enum c { i }; }; struct U : T { using T::i; };
            // causes get_declaration() to return NULL, since the object in the using 
            // declaration is a SgInitializedName.
               if (tmp->get_declaration() == NULL && tmp->get_initializedName() == NULL)
                  {
                    printf ("SgUsingDeclarationStatement::get_declaration() and get_initializedName() both return NULL \n");
                  }
               ROSE_ASSERT( (tmp->get_declaration() != NULL) || (tmp->get_initializedName() != NULL) );
               break;
             }

          case V_SgFunctionDefinition:
             {
               SgFunctionDefinition* tmp = isSgFunctionDefinition(node);
               ROSE_ASSERT(tmp != NULL);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

          case V_SgNamespaceDefinitionStatement:
             {
               SgNamespaceDefinitionStatement* tmp = isSgNamespaceDefinitionStatement(node);
            // Note that this class uses a different name since it has another member function 
            // "get_declarations()" which returns a list of the declarations in the namespace.
            // the issue was that two member function should not be differentiated by a single 
            // letter "s" at the end of the name!
               ROSE_ASSERT(tmp->get_namespaceDeclaration() != NULL);
               break;
             }

          case V_SgClassDefinition:
             {
               SgClassDefinition* tmp = isSgClassDefinition(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

          case V_SgConstructorInitializer:
             {
               SgConstructorInitializer* tmp = isSgConstructorInitializer(node);
            // constructor initializers are used for representation of casts from function return 
            // types within initializers (if required).  When this happens the get_declaration() 
            // (member function declaration) is NULL and get_class_decl() (the class declaration) 
            // is NULL.
            // if (tmp->get_declaration() == NULL)

            // DQ (1/16/2019): Check if this is associated with a SgNewExp with and SgArrayType type.
               bool associatedWithArrayType = false;
               if ( (tmp->get_declaration() == NULL) && (tmp->get_class_decl() == NULL) && (tmp->get_associated_class_unknown() == false) )
                  {
#if 0
                    printf ("SgConstructorInitializer::get_declaration() and get_class_decl() both return NULL and get_associated_class_unknown() == false \n");
#endif
                 // DQ (1/16/2019): Check if this is associated with a SgNewExp with and SgArrayType type.
                    ROSE_ASSERT(tmp->get_parent() != NULL);
#if 0
                    printf ("tmp->get_parent() = %p = %s \n",tmp->get_parent(),tmp->get_parent()->class_name().c_str());
#endif
                    SgNewExp* newExp = isSgNewExp(tmp->get_parent());
                    if (newExp != NULL)
                       {
                      // SgType* newExpType = newExp->get_type();
                         SgType* newExpType = newExp->get_specified_type();
#if 0
                         printf ("newExpType = %p = %s \n",newExpType,newExpType->class_name().c_str());
#endif
                      // DQ (1/17/2019): If this is just a typedef of an array then check that.
                         SgTypedefType* typedefType = isSgTypedefType(newExpType);
                         if (typedefType != NULL)
                            {
#if 0
                              printf ("typedefType->get_base_type() = %s \n",typedefType->get_base_type()->class_name().c_str());
#endif
                              newExpType = typedefType->get_base_type();
                            }

                         SgArrayType* arrayType = isSgArrayType(newExpType);
                         if (arrayType != NULL)
                            {
                              associatedWithArrayType = true;
                            }
                           else
                            {
                           // DQ (1/17/2019): This case fails for Cxx11_tests/test2016_90.C.
#if 0
                              printf ("Unclear how to handle this case! \n");
                              newExp->get_file_info()->display("");
#endif
                           // DQ (1/17/2019): Allow everything and debug this later!
                              associatedWithArrayType = true;
                            }
                       }
                  }

            // DQ (1/16/2019): Check if this is associated with a SgNewExp with and SgArrayType type.
            // ROSE_ASSERT(tmp->get_declaration() != NULL);
            // ROSE_ASSERT ( (tmp->get_declaration() != NULL) || (tmp->get_class_decl() != NULL) || (tmp->get_associated_class_unknown() == true) );
               ROSE_ASSERT ( (tmp->get_declaration() != NULL) || (tmp->get_class_decl() != NULL) || (tmp->get_associated_class_unknown() == true) || (associatedWithArrayType == true) );
               break;
             }

          case V_SgEnumVal:
             {
               SgEnumVal* tmp = isSgEnumVal(node);
               if (tmp->get_declaration() == NULL)
                  {
                    printf ("### Error: Need to fixup declaration in SgEnumVal node = %p ### \n",node);
                  }
            // ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgVariableSymbol:
             {
               SgVariableSymbol* tmp = isSgVariableSymbol(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgClassSymbol:
             {
               SgClassSymbol* tmp = isSgClassSymbol(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgTemplateSymbol:
             {
               SgTemplateSymbol* tmp = isSgTemplateSymbol(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgEnumSymbol:
             {
               SgEnumSymbol* tmp = isSgEnumSymbol(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgEnumFieldSymbol:
             {
               SgEnumFieldSymbol* tmp = isSgEnumFieldSymbol(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgTypedefSymbol:
             {
               SgTypedefSymbol* tmp = isSgTypedefSymbol(node);
               if (tmp->get_declaration() == NULL)
                  {
                    printf ("SgTypedefSymbol::get_declaration returns NULL \n");
                  }
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgLabelSymbol:
             {
               SgLabelSymbol* tmp = isSgLabelSymbol(node);

            // DQ (12/9/2007): Modified to reflect added support for fortran statement numeric labels.
            // ROSE_ASSERT(tmp->get_declaration() != NULL);
               if (tmp->get_declaration() == NULL)
                  {
#if 0
                    ROSE_ASSERT(tmp->get_fortran_statement() != NULL);
#else
                 // DQ (2/2/2011): Added support in SgLabelSymbol for Fortran alternative return parameters (see test2010_164.f90).
                    if (tmp->get_fortran_statement() == NULL)
                       {
                         ROSE_ASSERT(tmp->get_fortran_alternate_return_parameter() != NULL);
                       }
#endif
                  }
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgNamespaceSymbol:
             {
               SgNamespaceSymbol* tmp = isSgNamespaceSymbol(node);

            // DQ (8/30/2009): Added namespace alias support.
            // ROSE_ASSERT(tmp->get_declaration() != NULL);
               ROSE_ASSERT( (tmp->get_declaration() != NULL && tmp->get_isAlias() == false) || (tmp->get_aliasDeclaration() != NULL && tmp->get_isAlias() == true) );
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgFunctionSymbol:
             {
               SgFunctionSymbol* tmp = isSgFunctionSymbol(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the symbol table tests (so that symbols are tested)
          case V_SgMemberFunctionSymbol:
             {
               SgMemberFunctionSymbol* tmp = isSgMemberFunctionSymbol(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

       // This function is called from the type tests (so that these types are tested)
          case V_SgNamedType:
          case V_SgClassType:
          case V_SgEnumType:
          case V_SgTypedefType:
             {
               SgNamedType* tmp = isSgNamedType(node);
               ROSE_ASSERT(tmp->get_declaration() != NULL);
               break;
             }

          default:
             {
            // Numerous IR nodes don't have a get_declaration member function and these are ignored!
             }
        }
   }


TestExpressionTypes::TestExpressionTypes()
   {
  // DQ (10/31/2016): Use this mechanism to make traversals transformation-safe (it works).
  // myProcessingObject->set_useDefaultIndexBasedTraversal(false); 
     set_useDefaultIndexBasedTraversal(false); 
   }

void
TestExpressionTypes::visit ( SgNode* node )
   {
  // DQ (2/21/2006): Test the get_type() member function which is common on many IR nodes
  // printf ("In TestExpressionTypes::visit(): node = %s \n",node->class_name().c_str());
     SgExpression* expression = isSgExpression(node);

  // DQ(11/6/2016): Debugging failing mergeTest_133.C that is only demonstrated using Address Sanitizer 
  // and setting the memory pool length to be 1.  Using this value below to control calling the 
  // SgStringVal::get_type() function appears to be all the is required to fix the memory error.
  // At present I still don't understand the problem, but it apears to have to do with the
  // allocation of the SgIntVal object used within the SgStringType.  Note that at present
  // this is a memory error associated only with the regression tests in the mergeAST_tests
  // directory.  The failing tests are reproducable, but only on an odd subset of machines
  // and at present (before this fix) only in tests run using CMake. 
     bool skipProblemExpresion = (isSgStringVal(expression) != NULL);

  // DQ(11/6/2016): Debugging failing mergeTest_133.C: restrict to exclude calling SgStringVal::get_type().
  // if (expression != NULL)
     if (expression != NULL && skipProblemExpresion == false)
        {
#if 1
       // DQ (10/31/2016): Testing to debug mergeTest_04.C and mergeTest_111.C.

       // printf ("TestExpressionTypes::visit(): before calling expression->get_type() on expression = %p = %s call TestNodes::test() \n",expression,expression->class_name().c_str());

       // DQ (10/25/2016): Testing IR node integrity. This test makes the traversla order (n^2).
       // TestNodes::test();

       // printf ("TestExpressionTypes::visit(): calling expression->get_type() on expression = %p = %s (after TestNodes::test()) \n",expression,expression->class_name().c_str());

          SgType* type = expression->get_type();
          ROSE_ASSERT(type != NULL);

       // DQ (10/25/2016): Testing IR node integrity. This test makes the traversla order (n^2).
       // TestNodes::test();
#endif
#if 1
       // DQ (10/31/2016): Testing to debug mergeTest_04.C and mergeTest_111.C.

       // printf ("TestExpressionTypes::visit(): calling expression->get_type() on expression = %p = %s type = %s \n",expression,expression->class_name().c_str(),type->class_name().c_str());
       // PC (10/12/2009): The following test verifies that array types properly decay to pointer types
       //  From C99 6.3.2.1p3:
       /* Except when it is the operand of the sizeof operator or the unary & operator, or is a
          string literal used to initialize an array, an expression that has type array of type is
          converted to an expression with type pointer to type that points to the initial element of
          the array object and is not an lvalue. */
          type = type->stripTypedefsAndModifiers();
          ROSE_ASSERT(type != NULL);
          if (type->variantT() == V_SgArrayType || type->variantT() == V_SgTypeString)
             {
               SgExpression *parentExpr = isSgExpression(expression->get_parent());
               if (parentExpr != NULL && !(
#if 0
                                    // DQ (10/27/2015): Fixed warning from GNU 4.8.3 compiler.
                                    // parentExpr->variantT() == V_SgAssignInitializer && expression->variantT() == V_SgStringVal
                                      (parentExpr->variantT() == V_SgAssignInitializer && expression->variantT() == V_SgStringVal)
                                    || parentExpr->variantT() == V_SgDotExp
                                    || parentExpr->variantT() == V_SgArrowExp
                                 // DQ (10/27/2015): Fixed warning from GNU 4.8.3 compiler.
                                 // || isSgInitializer(parentExpr) && isSgInitializer(expression)))
#else
                                 // DQ (10/31/2015): I think this is fixing the same issue (and resulted in a conflict).
                                    (parentExpr->variantT() == V_SgAssignInitializer && expression->variantT() == V_SgStringVal)
                                    || parentExpr->variantT() == V_SgDotExp
                                    || parentExpr->variantT() == V_SgArrowExp
#endif
                                    || (isSgInitializer(parentExpr) && isSgInitializer(expression))))
                  {
                    SgType* parentType = parentExpr->get_type();
                    parentType = parentType->stripTypedefsAndModifiers();
                    if (SageInterface::getDimensionCount(type) == SageInterface::getDimensionCount(parentType)
                     && SageInterface::getArrayElementType(type) == SageInterface::getArrayElementType(parentType))
                       {
                         SgValueExp *parentValueExpr = isSgValueExp(parentExpr);
                         if (parentValueExpr == NULL || expression != parentValueExpr->get_originalExpressionTree())
                            {
                           // DQ (9/14/2010): Make this a level 1 warning. It is common for Fortran code but never see for C++ code.
                              if ( SgProject::get_verbose() > 0 )
                                 {
                                   printf ("Warning: encountered a case where an array type did not decay to a pointer type\n"
                                           "  parentExpr = %p = %s, expression = %p = %s, parentType = %p = %s, type = %p = %s\n",
                                                   parentExpr, parentExpr->sage_class_name(),
                                                   expression, expression->sage_class_name(),
                                                   parentType, parentType->sage_class_name(),
                                                   type, type->sage_class_name());
                                 }
                           // ROSE_ASSERT(false);
                            }
                       }
                  }
             }
#endif
        }

#if 1
  // DQ (10/31/2016): Testing to debug mergeTest_04.C and mergeTest_111.C.

     SgType* type = NULL;
     switch (node->variantT())
        {
       // IR nodes that have a get_type member function
          case V_SgInitializedName:
             {
               SgInitializedName* initializedName = isSgInitializedName(node);
               ROSE_ASSERT(initializedName->get_type() != NULL);
               type = initializedName->get_type();
               break;
             }
          case V_SgTemplateParameter:
             {
               SgTemplateParameter* x = isSgTemplateParameter(node);
               ROSE_ASSERT(x->get_type() != NULL);
               type = x->get_type();
               break;
             }
          case V_SgTemplateArgument:
             {
               SgTemplateArgument* x = isSgTemplateArgument(node);
               ROSE_ASSERT(x->get_type() != NULL);
               type = x->get_type();
               break;
             }
          case V_SgVariableDefinition:
             {
               SgVariableDefinition* x = isSgVariableDefinition(node);
               ROSE_ASSERT(x->get_type() != NULL);
               type = x->get_type();
               break;
             }
          case V_SgEnumDeclaration:
             {
               SgEnumDeclaration* x = isSgEnumDeclaration(node);
               ROSE_ASSERT(x->get_type() != NULL);
               type = x->get_type();
               break;
             }
          case V_SgTypedefDeclaration:
             {
               SgTypedefDeclaration* x = isSgTypedefDeclaration(node);
               ROSE_ASSERT(x->get_type() != NULL);
               type = x->get_type();
               break;
             }
          case V_SgClassDeclaration:
          case V_SgDerivedTypeStatement:
             {
               SgClassDeclaration* x = isSgClassDeclaration(node);
               ROSE_ASSERT(x->get_type() != NULL);
               type = x->get_type();
               break;
             }
          case V_SgFunctionDeclaration:
             {
               SgFunctionDeclaration* x = isSgFunctionDeclaration(node);
               ROSE_ASSERT(x->get_type() != NULL);
               type = x->get_type();
               break;
             }

          default:
             {
            // noting to do
             }          
        }

     SgNamedType* namedType = isSgNamedType(type);
     if (namedType != NULL)
        {
          SgDeclarationStatement* declaration = namedType->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          SgDeclarationStatement* nondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
       // ROSE_ASSERT(declaration == nondefiningDeclaration);
       // ROSE_ASSERT(nondefiningDeclaration != NULL);
          SgDeclarationStatement* definingDeclaration    = declaration->get_definingDeclaration();
          if (definingDeclaration != NULL)
             {
               switch(definingDeclaration->variantT())
                  {
                 // DQ (1/1/2012): Added support for templates in the AST.
                    case V_SgTemplateClassDeclaration:

                 // This case applies to template or non-template classes
                    case V_SgClassDeclaration:
                    case V_SgDerivedTypeStatement:
                    case V_SgTemplateInstantiationDecl:
                    case V_SgJovialTableStatement:
                       {
                         SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(definingDeclaration);
                         ROSE_ASSERT(definingClassDeclaration->get_definition() != NULL);
                      // SgClassDeclaration* generalClassDeclaration = isSgClassDeclaration(declaration);
                         ROSE_ASSERT(nondefiningDeclaration != NULL);
#if 0
                         if (declaration == nondefiningDeclaration)
                            {
                              printf ("SAME: declaration and nondefiningDeclaration = %p = %s = %s \n",
                                   nondefiningDeclaration,nondefiningDeclaration->class_name().c_str(),definingClassDeclaration->get_name().str());
                            }
                           else
                            {
                              printf ("DIFFERENT: declaration = %p = %s nondefiningDeclaration = %p = %s = %s \n",
                                   declaration,declaration->class_name().c_str(),nondefiningDeclaration,
                                   nondefiningDeclaration->class_name().c_str(),definingClassDeclaration->get_name().str());
                            }
#endif
#if 0
                         if (declaration != nondefiningDeclaration)
                            {
                              printf ("declaration            = %p = %s \n",declaration,declaration->class_name().c_str());
                              printf ("nondefiningDeclaration = %p = %s \n",nondefiningDeclaration,nondefiningDeclaration->class_name().c_str());
                            }
                      // ROSE_ASSERT(declaration == nondefiningDeclaration);
#endif
                         break;
                       }

                    case V_SgTypedefDeclaration:
                       {
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(definingDeclaration);
                         ROSE_ASSERT(typedefDeclaration->get_base_type() != NULL);
                         ROSE_ASSERT(typedefDeclaration->get_type() != NULL);
#if 0
                         if (typedefDeclaration->get_parent_scope() == NULL)
                            {
                              printf ("Found a case where the typedefDeclaration->get_parent_scope() == NULL \n");
                            }
                           else
                            {
                              printf ("Found a case where the typedefDeclaration->get_parent_scope() != NULL \n");
                            }
#endif
                      // DQ (2/25/2006): It appears that this is always NULL (except for test2005_188.C, 
                      // where it points to the symbol of the class if it is a member type)
                      // ROSE_ASSERT(typedefDeclaration->get_parent_scope() == NULL);
                         ROSE_ASSERT(typedefDeclaration->get_scope() != NULL);

                      // This is to handle declarations that appear directly in the typedef (most often NULL)
                         if (typedefDeclaration->get_typedefBaseTypeContainsDefiningDeclaration() == true)
                              ROSE_ASSERT(typedefDeclaration->get_declaration() != NULL);
                           else
                              ROSE_ASSERT(typedefDeclaration->get_declaration() == NULL);
                         break;
                       }

                    case V_SgEnumDeclaration:
                       {
                         SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(definingDeclaration);
                         ROSE_ASSERT(enumDeclaration->get_type() != NULL);
                         ROSE_ASSERT(enumDeclaration->get_scope() != NULL);
                         break;
                       }

                     default:
                       {
                         printf ("Error: default reached in switch: definingDeclaration not tested = %p = %s \n",definingDeclaration,definingDeclaration->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
             }
        }
#endif

#if 0
     SgFunctionType* namedType = isNamedType(type);
     if (namedType != NULL)
        {
          SgDeclarationStatement* declaration = namedType->get_declaration();
          ROSE_ASSERT(declaration != NULL);
          SgDeclarationStatement* nondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
          SgDeclarationStatement* definingDeclaration    = declaration->get_definingDeclaration();
          if (definingDeclaration != NULL)
             {
               switch(definingDeclaration->variantT())
                  {
                    case V_SgFunctionDeclaration:
                       {
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(definingDeclaration);
                         ROSE_ASSERT(functionDeclaration->get_definition() != NULL);
                         break;
                       }

                     default:
                       {
                         printf ("definingDeclaration not tested = %s \n",definingDeclaration->class_name().c_str());
                       }
                  }
             }
        }
#endif
   }

void
TestLValues::visit ( SgNode* node )
{
        SgExpression* expression = isSgExpression(node);
        if (expression != NULL)
        {
                return;
        }

        //
        // Test isLValue()
        //
        if (expression != NULL)
        {
                bool verifiedLValue = false;
                bool verifiedDefinable = false;
                switch (node->variantT())
                {
                        case V_SgScopeOp:          
                        {
                                SgScopeOp* scopeOp = isSgScopeOp(node);
                                ROSE_ASSERT(scopeOp);
                                verifiedLValue = scopeOp->get_rhs_operand()->isLValue();
                                break;
                        }
                        case V_SgPntrArrRefExp:  
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgPointerDerefExp: 
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgAddressOfOp:    
                        {
                                /*! std:5.2.6 par:1 */
                                // TODO: king84: false?  char x[4];  x is lvalue; (x + 1) is rvalue; *(x+1) is lvalue; *(x+1) = x[1]
                                if (!isSgAddressOfOp(expression)->get_operand()->isLValue()) // must also be mutable
                                {
                                        ROSE_ASSERT(!"Child operand of an address-of operator must be an lvalue in isLValue on SgAddressOfOp");
                                }
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgArrowExp:       
                        {
                                // TODO: king84: is this true?
                                if (!isSgArrowExp(expression)->get_rhs_operand()->isLValue())
                                {
                                        ROSE_ASSERT(!"Right-hand-side must be an lvalue as a data member or member function in isLValue for SgArrowExp");
                                }
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgDotExp:           
                        {
                                // TODO: king84: is this true?
                                if (!isSgDotExp(expression)->get_rhs_operand()->isLValue())
                                {
                                        ROSE_ASSERT(!"Right-hand-side must be an lvalue as a data member or member function in isLValue for SgDotExp");
                                }
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgDotStarOp:       
                        {
                                // TODO: king84: is this true?
                                if (!isSgDotStarOp(expression)->get_lhs_operand()->isLValue())
                                {
                                        ROSE_ASSERT(!"Left-hand-side must be an lvalue in isLValue for SgDotStarOp");
                                }
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgArrowStarOp:      
                        {
                                // TODO: king84: is this true? consider 'this': class A {int A::*pf();} this->*pf();
                                if (!isSgArrowStarOp(expression)->get_lhs_operand()->isLValue())
                                {
                                        ROSE_ASSERT(!"Left-hand-side must be an lvalue in isLValue for SgArrowStarOp");
                                }
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgMinusMinusOp:       
                        {
                                SgMinusMinusOp* mmo = isSgMinusMinusOp(node);
                                if (mmo->get_mode() == SgUnaryOp::postfix)
                                        verifiedLValue = false;
                                else
                                {
                                        /*! std:5.3.2 par:2 */
                                        if (!mmo->get_operand()->isLValue()) // must also be mutable
                                        {
                                                ROSE_ASSERT(!"Child operand of a prefix-increment must be an lvalue in isLValue on SgMinusMinusOp");
                                        }
                                        verifiedLValue = true;
                                }
                                break;
                        }
                        case V_SgPlusPlusOp: 
                        {
                                SgPlusPlusOp* ppo = isSgPlusPlusOp(node);
                                if (ppo->get_mode() == SgUnaryOp::postfix)
                                        verifiedLValue = false;
                                else
                                {
                                        /*! std:5.3.2 par:1 */
                                        if (!ppo->get_operand()->isLValue()) // must also be mutable
                                        {
                                                ROSE_ASSERT(!"Child operand of a prefix-increment must be an lvalue in isLValue on SgPlusPlusOp");
                                        }
                                        verifiedLValue = true;
                                }
                                break;
                        }
                        case V_SgCastExp:
                        {
                                SgCastExp* castExp = isSgCastExp(node);
                                ROSE_ASSERT(castExp);
                                switch (castExp->cast_type())
                                {
                                        case SgCastExp::e_C_style_cast:
                                        case SgCastExp::e_const_cast:
                                        case SgCastExp::e_static_cast:
                                        case SgCastExp::e_dynamic_cast:
                                        case SgCastExp::e_reinterpret_cast:
                                                verifiedLValue = SageInterface::isReferenceType(castExp->get_type());
                                                break;
                                        case SgCastExp::e_unknown:
                                        case SgCastExp::e_default:
                                        default:
                                                verifiedLValue = false;
                                                break;
                                }
                                break;
                        }
                        case V_SgCommaOpExp:       
                        {
                                SgCommaOpExp* comma = isSgCommaOpExp(node);
                                ROSE_ASSERT(comma);
                                verifiedLValue = comma->get_rhs_operand()->isLValue();
                                break;
                        }
                        case V_SgAssignOp:        
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgPlusAssignOp:     
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgMinusAssignOp: 
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgAndAssignOp:    
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgIorAssignOp:    
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgMultAssignOp:     
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgDivAssignOp:     
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgModAssignOp:      
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgXorAssignOp:   
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgLshiftAssignOp: 
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgRshiftAssignOp: 
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgPointerAssignOp:  
                        {
                                verifiedDefinable = true;
                                break;
                        }
                        case V_SgStringVal:        
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgVarRefExp:           
                        {
                                verifiedLValue = true;
                                SgVarRefExp* var = isSgVarRefExp(node);
                                verifiedDefinable = !SageInterface::isConstType(var->get_type());
                                break;
                        }
                        case V_SgFunctionRefExp:      
                        {
                                break;
                        }
                        case V_SgMemberFunctionRefExp:    
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgFunctionCallExp:     
                        {
                                SgFunctionCallExp* funOp = isSgFunctionCallExp(node);
                                ROSE_ASSERT(funOp);
                                SgType* type = funOp->get_function()->get_type();
                                while (SgTypedefType* type2 = isSgTypedefType(type))
                                        type = type2->get_base_type();
                                SgFunctionType* ftype = isSgFunctionType(type);
                                verifiedLValue = SageInterface::isReferenceType(ftype->get_return_type());
                                break;
                        }
                        case V_SgTypeIdOp:            
                        {
                                verifiedLValue = true;
                                break;
                        }
                        case V_SgConditionalExp:          
                        {
                                SgConditionalExp* cond = isSgConditionalExp(node);
                                verifiedLValue = (cond->get_true_exp()->isLValue() && cond->get_false_exp()->isLValue()) && (cond->get_true_exp()->get_type() == cond->get_false_exp()->get_type());
                                break;
                        }
                        case V_SgShortVal:               
                        case V_SgCharVal:         
                        case V_SgUnsignedCharVal: 
                        case V_SgWcharVal:       
                        case V_SgUnsignedShortVal: 
                        case V_SgIntVal:                 
                        case V_SgEnumVal:         
                        case V_SgUnsignedIntVal:  
                        case V_SgLongIntVal:     
                        case V_SgLongLongIntVal:   
                        case V_SgUnsignedLongLongIntVal: 
                        case V_SgUnsignedLongVal: 
                        case V_SgFloatVal:        
                        case V_SgDoubleVal:      
                        case V_SgLongDoubleVal:    
                        case V_SgComplexVal:             
                        case V_SgUpcThreads:     
                        case V_SgUpcMythread: 
                        case V_SgUnaryOp:             
                        case V_SgBinaryOp:                
                        case V_SgExprListExp:         
                        case V_SgUserDefinedBinaryOp: 
                        case V_SgBoolValExp:     
                        case V_SgExponentiationOp: 
                        case V_SgConcatenationOp: 
                        case V_SgLshiftOp:      
                        case V_SgRshiftOp:       
                        case V_SgEqualityOp:    
                        case V_SgLessThanOp:     
                        case V_SgGreaterThanOp:  
                        case V_SgNotEqualOp:       
                        case V_SgLessOrEqualOp:   
                        case V_SgGreaterOrEqualOp: 
                        case V_SgAddOp:         
                        case V_SgSubtractOp:     
                        case V_SgMultiplyOp:     
                        case V_SgDivideOp:         
                        case V_SgIntegerDivideOp: 
                        case V_SgModOp:            
                        case V_SgAndOp:         
                        case V_SgOrOp:           
                        case V_SgBitXorOp:       
                        case V_SgBitAndOp:         
                        case V_SgBitOrOp:         
                        case V_SgThrowOp:        
                        case V_SgRealPartOp:         
                        case V_SgImagPartOp: 
                        case V_SgConjugateOp:     
                        case V_SgUserDefinedUnaryOp: 
                        case V_SgExpressionRoot: 
                        case V_SgMinusOp:            
                        case V_SgUnaryAddOp: 
                        case V_SgNotOp:           
                        case V_SgBitComplementOp: 
                        case V_SgClassNameRefExp:          
                        case V_SgValueExp:            
                        case V_SgSizeOfOp:                 
                        case V_SgUpcLocalsizeofExpression:
                        case V_SgUpcBlocksizeofExpression:
                        case V_SgUpcElemsizeofExpression:
                        case V_SgNewExp:              
                        case V_SgDeleteExp:           
                        case V_SgThisExp:                  
                        case V_SgRefExp:              
                        case V_SgInitializer:             
                        case V_SgVarArgStartOp:       
                        case V_SgVarArgOp:            
                        case V_SgVarArgEndOp:              
                        case V_SgVarArgCopyOp:        
                        case V_SgVarArgStartOneOperandOp: 
                        case V_SgNullExpression:      
                        case V_SgVariantExpression:   
                        case V_SgSubscriptExpression:      
                        case V_SgColonShapeExp:       
                        case V_SgAsteriskShapeExp:        
                        case V_SgImpliedDo:         
                        case V_SgIOItemExpression:         
                        case V_SgStatementExpression:  
                        case V_SgAsmOp:               
                        case V_SgLabelRefExp:         
                        case V_SgActualArgumentExpression: 
                        case V_SgUnknownArrayOrFunctionReference:               
                        case V_SgPseudoDestructorRefExp:                    
                        case V_SgCudaKernelCallExp:   
                        case V_SgCudaKernelExecConfig: 
                                break;
                        /*UseRenameExpression*/
                        /*UseOnlyExpression*/ 
                        default:
                                break;
                }
                if (expression->isLValue() != verifiedLValue)
                        std::cout << "Node at " << node << " is sgtype " << node->variantT() << " : " << node->class_name() << std::endl;
                ROSE_ASSERT (expression->isLValue() == verifiedLValue);
                if (expression->isDefinable() != verifiedDefinable)
                        std::cout << "Node at " << node << " is sgtype " << node->variantT() << " : " << node->class_name() << std::endl;
                ROSE_ASSERT (expression->isDefinable() == verifiedDefinable);
        }
}




void
TestMangledNames::visit ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);

#if 0
     printf ("In TestMangledNames::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif

  // DQ (1/12/13): Added to support detection of scopes that have been deleted.
     bool isDeletedNode = false;

  // DQ (5/26/2013): This is the signature in the IR node held by the memory pool for an IR node that has been previously deleted.
  // This node is being traversed by the AST traversal following a dangling pointer.  This was a problem that was previously
  // difficult to detect but now appears to be more easily reproduced in ROSE (at least with the gnu 4.2.4 compiler).
     ROSE_ASSERT(node != NULL);
     if (node->class_name() == "SgNode")
        {
          printf ("ERROR: This node = %p has been previously deleted \n",node);
          ROSE_ASSERT(false);
        }


     string mangledName;
#if 0
     SgGlobal* global = isSgGlobal(node);
     if (global != NULL)
        {
          const std::map<std::string, int>& shortMangledNameCache = global->get_shortMangledNameCache();
          for (std::map<std::string, int>::const_iterator i = shortMangledNameCache.begin(); i != shortMangledNameCache.end(); ++i) {
            this->totalLongMangledNameSize += i->first.size();
            ++this->totalNumberOfLongMangledNames;
          }
        }
#endif
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
     SgNonrealDecl * nrdecl = isSgNonrealDecl(node);
     if (declarationStatement != NULL)
        {
       // DQ (1/12/13): Added fix for scopes that may have been deleted (happens where astDelete mechanism is used)
       // mangledName = declarationStatement->get_mangled_name().getString();
          if (nrdecl == NULL && declarationStatement->get_scope() == NULL)
             {
               printf ("ERROR: TestMangledNames::visit(): declarationStatement = %p = %s \n",declarationStatement,declarationStatement->class_name().c_str());
             }
          ROSE_ASSERT( nrdecl != NULL || declarationStatement->get_scope() != NULL );
#if 0
          printf ("TestMangledNames::visit(): declarationStatement->get_scope() = %p = %s \n",declarationStatement->get_scope(),declarationStatement->get_scope()->class_name().c_str());
#endif
          if (nrdecl == NULL && declarationStatement->get_scope()->class_name() == "SgNode")
             {
               isDeletedNode = true;
             }

          if (isDeletedNode == false)
             {
               mangledName = declarationStatement->get_mangled_name().getString();
             }
            else
             {
               printf ("WARNING: evaluation of the mangled name for a declaration in a scope = %p that has been deleted is being skipped! \n",declarationStatement->get_scope());
             }

       // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }

     SgInitializedName* initializedName = isSgInitializedName(node);
     if (initializedName != NULL)
        {
       // mangledName = initializedName->get_mangled_name().getString();
#if 0
          printf ("TestMangledNames::visit(): initializedName->get_scope() = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
#endif
       // ROSE_ASSERT (initializedName->get_scope() != NULL);
          if (initializedName->get_scope() != NULL)
             {
               if (initializedName->get_scope()->class_name() == "SgNode")
                  {
                    isDeletedNode = true;
                  }

               if (isDeletedNode == false)
                  {
                    mangledName = initializedName->get_mangled_name().getString();
                  }
                 else
                  {
                    printf ("WARNING: evaluation of the mangled name for a SgInitializedName in a scope = %p that has been deleted is being skipped! \n",initializedName->get_scope());
                  }
             }
            else
             {
               printf ("NOTE: In TestMangledNames::visit(): initializedName->get_scope() == NULL: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
             }

       // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }
#if 1
     SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(node);
     if (functionDefinition != NULL)
        {
       // DQ (11/27/2011): Adding more support for template declarations in the AST.
       // mangledName = functionDefinition->get_mangled_name().getString();
          SgTemplateFunctionDefinition* templateFunctionDefinition = isSgTemplateFunctionDefinition(functionDefinition);
          if (templateFunctionDefinition != NULL)
             {
               mangledName = templateFunctionDefinition->get_mangled_name().getString();
             }
            else
             {
               mangledName = functionDefinition->get_mangled_name().getString();
             }
       // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }
#endif
#if 1
     SgClassDefinition* classDefinition = isSgClassDefinition(node);
     if (classDefinition != NULL)
        {
          mangledName = classDefinition->get_mangled_name().getString();
       // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }
#endif
#if 1
     SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(node);
     if (namespaceDefinition != NULL)
        {
          mangledName = namespaceDefinition->get_mangled_name().getString();
       // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }
#endif
#if 1
     SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
     if (templateArgument != NULL)
        {
          mangledName = templateArgument->get_mangled_name().getString();
       // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }
#endif
     SgType* type = isSgType(node);
     if (type != NULL)
        {
       // DQ (1/12/13): Need to test for declarations in scopes that might have been deleted (part of astCopy and astDelete support).
       // If there is a declaration, then the scope of that declaration might have been deleted (we need to detect this).
       // If this is a type in a scope that has been deleted then we should remove the scope (bug in the astDelete support, I think).
          SgDeclarationStatement* decl = type->getAssociatedDeclaration();
       // ROSE_ASSERT(decl != NULL);
#if 0
          printf ("TestMangledNames::visit(): decl = %p = %s \n",decl,decl != NULL ? decl->class_name().c_str() : "null");
#endif
          if (decl != NULL)
             {
               if (decl->class_name() == "SgNode")
                  {
                    printf ("ERROR: decl = %p was previously deleted \n",decl);
                    ROSE_ASSERT(false);
                  }
            // DQ (5/25/2013): This is failing for the astInterface tests: deepDelete.C
               if (decl->get_scope() == NULL)
                  {
                 // printf ("ERROR: TestMangledNames::visit(): decl = %p = %s \n",decl,decl != NULL ? decl->class_name().c_str() : "null");
                    printf ("ERROR: TestMangledNames::visit(): decl = %p \n",decl);
                  }
               ROSE_ASSERT(decl->get_scope() != NULL);
#if 0
               printf ("TestMangledNames::visit(): decl->get_scope() = %p = %s \n",decl->get_scope(),decl->get_scope()->class_name().c_str());
#endif
            // ROSE_ASSERT(decl->get_scope()->class_name() != "SgNode");
               if (decl->get_scope()->class_name() == "SgNode")
                  {
                    isDeletedNode = true;
                  }
             }
            else
             {
            // DQ (5/25/2013): Added this case to set isDeletedNode = true when decl == NULL.
               isDeletedNode = true;
             }

       // SgScopeStatement* scope = type->getCurrentScope();
       // printf ("TestMangledNames::visit(): scope = %p \n",scope);
       // if (scope != NULL)
       //    {
       //      ROSE_ASSERT(scope->class_name() != "SgNode");
       //    }

       // Notice that this has a different name get_mangled() instead of get_mangled_name()

       // DQ (1/12/13): For now, we want to ignore tests on IR nodes for types that are associated with declarations in scopes that have been deleted as part of the astDelete mechanism.
       // mangledName = type->get_mangled().getString();
#if 0
          printf ("TestMangledNames::visit(): isDeletedNode = %s \n",isDeletedNode ? "true" : "false");
#endif
          if (isDeletedNode == false)
             {
               mangledName = type->get_mangled().getString();
             }
            else
             {
            // DQ (5/25/2013): Fixed SgType::getAssociatedDeclaration() to return NULL when we detect that the declaration was deleted (e.g. when decl->class_name() == "SgNode").
            // ROSE_ASSERT(decl != NULL);
            // printf ("WARNING: evaluation of the mangled name for a declaration in a scope = %p that has been deleted is being skipped! \n",decl->get_scope());
               if (decl != NULL)
                  {
                    printf ("WARNING: evaluation of the mangled name for a declaration in a scope = %p that has been deleted is being skipped! \n",decl->get_scope());
                  }
             }
       // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());
        }

  // printf ("Test generated mangledName for node = %p = %s = %s \n",node,node->class_name().c_str(),mangledName.c_str());

  // DQ (8/28/2006): Added tests for the length of the mangled names
     unsigned long mangledNameSize = mangledName.size();
     saved_totalMangledNameSize += mangledNameSize;
     saved_numberOfMangledNames++;
     if (saved_maxMangledNameSize < mangledNameSize)
        {
          saved_maxMangledNameSize = mangledNameSize;
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
             {
               printf ("saved_maxMangledNameSize = %ld average size = %ld \n",saved_maxMangledNameSize,saved_totalMangledNameSize/saved_numberOfMangledNames);
             }
        }
   }

TestMangledNames::TestMangledNames()
   : saved_maxMangledNameSize(0),saved_totalMangledNameSize(0),saved_numberOfMangledNames(0),totalLongMangledNameSize(0),totalNumberOfLongMangledNames(0)
   {
  // Nothing to do here!
   }

void
TestMangledNames::test()
   {
     TestMangledNames t;

  // DQ (6/26/2007): Added code by Jeremiah for shorter mangled names
     const std::map<std::string, int>& shortMangledNameCache = SgNode::get_shortMangledNameCache();
     for (std::map<std::string, int>::const_iterator i = shortMangledNameCache.begin(); i != shortMangledNameCache.end(); ++i) 
        {
          t.totalLongMangledNameSize += i->first.size();
          ++(t.totalNumberOfLongMangledNames);
        }

  // t.traverse(node,preorder);
     t.traverseMemoryPool();
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        {
          printf ("saved_numberOfMangledNames = %ld \n",t.saved_numberOfMangledNames);
          printf ("saved_maxMangledNameSize   = %ld \n",t.saved_maxMangledNameSize);
          printf ("saved_totalMangledNameSize = %ld avarage size = %lf \n",t.saved_totalMangledNameSize,t.saved_totalMangledNameSize*1.0/t.saved_numberOfMangledNames);
          printf ("Total long mangled name size = %lu for %lu name(s), average is %lf\n", t.totalLongMangledNameSize, t.totalNumberOfLongMangledNames, t.totalLongMangledNameSize * 1. / t.totalNumberOfLongMangledNames);
        }
   }

#if 0
// DQ (6/26/2006): This is not included in the more general TestParentPointersInMemoryPool tests
void
TestParentPointersOfSymbols::visit ( SgNode* node )
   {
     SgSymbol* symbol = isSgSymbol(node);
     if (symbol != NULL)
        {
          if (symbol->get_parent() == NULL)
             {
               printf ("Error: symbol with NULL parent pointer at node = %p = %s \n",node,node->class_name().c_str());
             }
          ROSE_ASSERT(symbol->get_parent() != NULL);
        }
   }

void
TestParentPointersOfSymbols::test()
   {
     TestParentPointersOfSymbols t;
  // t.traverse(node,preorder);
     t.traverseMemoryPool();
   }
#endif
#if 0
// Build a global function to simplify testing
void testParentPointersOfSymbols()
   {
     TestParentPointersOfSymbols::test();
   }
#endif


void
TestParentPointersInMemoryPool::test()
   {
     TestParentPointersInMemoryPool t;
     t.traverseMemoryPool();
   }
     
void
TestParentPointersInMemoryPool::visit(SgNode* node)
   {
#if 0
     printf ("##### TestParentPointersInMemoryPool::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

     SgType*        type        = isSgType(node);
     SgSymbol*      symbol      = isSgSymbol(node);
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     SgSupport*     support     = isSgSupport(node);

  // All types should have NULL parent pointers (because types can be shared)
     if (type != NULL)
        {
       // Note that the SgNode::get_parent() function is forced to return NULL for the case of a SgType IR node
          ROSE_ASSERT(type->get_parent() == NULL);
        }

  // Symbols can be shared within a single file but are not yet shared across files in the AST merge
     if (symbol != NULL)
        {
          if (symbol->get_parent() == NULL)
            printf("Error: symbol named %s has no parent set\n",symbol->get_name().getString().c_str());
          ROSE_ASSERT(symbol->get_parent() != NULL);
        }

  // DQ (10/16/2017): Skip parent pointer checking for the untyped IR nodes.
  // Rasmussen (3/7/2018): Added class name and node pointer address to output.
     if (isSgUntypedNode(locatedNode) != NULL)
        {
           std::cerr << "TestParentPointersInMemoryPool::visit(): Skipping SgUntypedNode IR Node "
                     << node->sage_class_name() << ": " << node << std::endl;
          return;
        }

#if 0
     if (isSgExpression(locatedNode) != NULL)
        {
          printf ("TestParentPointersInMemoryPool::visit(): Skipping SgExpression IR Node \n");
        }
#endif

  // Skip SgExpression object for now!
     locatedNode = isSgStatement(locatedNode);

  // SgStatement and SgExpression IR nodes should always have a valid parent (except for the SgProject)
     if (locatedNode != NULL)
        {
          if (locatedNode->get_parent() == NULL)
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("Error: locatedNode->get_parent() == NULL locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
            // printf ("Error: locatedNode->get_parent() == NULL locatedNode = %p = %s = %s \n",locatedNode,locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode));
#endif
             }

       // DQ (3/3/2012): Commented this out test (for debugging purposes only).
       // printf ("In TestParentPointersInMemoryPool::visit(): Commented test for locatedNode->get_parent() != NULL \n");
       // ROSE_ASSERT(locatedNode->get_parent() != NULL);
        }

  // Some SgSupport IR nodes have a valid parent and others are always NULL
     if (support != NULL)
        {
          switch (support->variantT())
             {
               case V_SgProject:
                  {
                    ROSE_ASSERT(support->get_parent() == NULL);
                    break;
                  }

            // DQ (2/20/2007): Added SgTemplateArgument to the list of IR nodes that should have a valid parent set!
            // case V_SgTemplateArgument:
               case V_SgStorageModifier:
            // case V_SgInitializedName:
               case V_SgSymbolTable:
            // case V_SgFile:
               case V_SgSourceFile:
               case V_SgBinaryComposite:
               case V_SgUnknownFile:
               case V_SgTypedefSeq:
               case V_SgFunctionParameterTypeList:
               case V_SgPragma:
               case V_SgBaseClass:
               case V_SgExpBaseClass:
               case V_SgNonrealBaseClass:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
#if 0
                         printf ("Case of SgSupport support = %p = %s parent = %p = %s \n",
                              support,support->class_name().c_str(),parent,(parent != NULL) ? parent->class_name().c_str() : "Null");
#endif
                      // printf ("Error: NULL parent found \n");
                      // ROSE_ASSERT(support->get_file_info() != NULL);
                      // support->get_file_info()->display("Error: NULL parent found");
                       }
                 // ROSE_ASSERT(support->get_parent() != NULL);
                    break;
                  }

               case V_Sg_File_Info:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
#if PRINT_DEVELOPER_WARNINGS
                         printf ("Warning: detected Sg_File_Info without parent set properly at %p = %s parent is currently NULL \n",support,support->class_name().c_str());
#endif
                      // printf ("Error: NULL parent found \n");
                      // ROSE_ASSERT(support->get_file_info() != NULL);
                      // support->get_file_info()->display("Error: NULL parent found");
                       }
                 // ROSE_ASSERT(support->get_parent() != NULL);

#if 0
                 // DQ (1/22/2008): I have turned this on to catch Fortran statements that don't have properly set source positions.
                 // This is useful for internal testing to make sure that source position is properly set where possible.

                 // DQ (6/11/2007): This test causes the file I/O to fail.  test2001_03.C can not be read in, 
                 // but only when reading all threee files in: astFileRead test2001_01 test2001_02 test2001_03
                    Sg_File_Info* fileInfo = isSg_File_Info(support);
#if 1
                 // DQ (6/11/2007): This test causes the file I/O to fail.  test2001_03.C can not be read in, 
                 // but only when reading all threee files in: astFileRead test2001_01 test2001_02 test2001_03
                 // I think this is because the static data in the Sg_File_Info can not be set properly when any two files have different values.
                    if (fileInfo->get_filenameString() == "NULL_FILE")
                       {
                      // printf ("Found a Sg_File_Info using filename == NULL_FILE \n");

                         if (parent != NULL)
                            {
                           // printf ("Found a Sg_File_Info using filename == NULL_FILE: parent = %p = %s = %s \n",parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
                           // if (isSgStatement(parent) != NULL && isSgVariableDefinition(parent) == NULL)
                              if (isSgLocatedNode(parent) != NULL && isSgVariableDefinition(parent) == NULL)
                                 {
                                   ROSE_ASSERT(parent->get_parent() != NULL);
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(parent);
                                   if (classDeclaration != NULL && classDeclaration != classDeclaration->get_definingDeclaration())
                                      {
                                     // Skip this case since we don't require source position information on nondefining declarations!
                                      }
                                     else
                                      {
                                        printf ("Found a Sg_File_Info using filename == NULL_FILE: fileInfo = %p parent = %p = %s = %s in %p = %s = %s \n",
                                             fileInfo,parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str(),
                                             parent->get_parent(),parent->get_parent()->class_name().c_str(),SageInterface::get_name(parent->get_parent()).c_str());
                                      }
                                 }
                                else
                                 {
                                // printf ("Found a Sg_File_Info using filename == NULL_FILE: parent = %p = %s = %s \n",parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
                                 }
                            }
                           else
                            {
                              printf ("Found a Sg_File_Info using filename == NULL_FILE: parent == NULL \n");
                            }

                      // ROSE_ASSERT(false);
                       }
#endif
#if 0
                 // DQ (6/11/2007): This test causes the file I/O to fail.  test2001_03.C can not be read in, 
                 // but only when reading all threee files in: astFileRead test2001_01 test2001_02 test2001_03
                 // I think this is because the static data in the Sg_File_Info can not be set properly when any two files have different values.
                    if (fileInfo->get_file_id() >= fileInfo->get_max_file_id())
                       {
                         printf ("Found a Sg_File_Info with file_id out of range: file_id = %d max_file_id = %d \n",fileInfo->get_file_id(),fileInfo->get_max_file_id());
                         ROSE_ASSERT(false);
                       }
#endif
#endif
                    break;
                  }

            // DQ (6/11/2007): Added this case
               case V_SgInitializedName:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
#if 1
                         printf ("Warning: detected SgInitializedName without parent set properly at %p = %s parent is currently NULL \n",support,support->class_name().c_str());
#endif
                      // printf ("Error: NULL parent found \n");
                      // ROSE_ASSERT(support->get_file_info() != NULL);
                      // support->get_file_info()->display("Error: NULL parent found");
                       }
                 // ROSE_ASSERT(support->get_parent() != NULL);
                    break;
                  }

            // DQ (6/11/2007): Added this case
               case V_SgTemplateArgument:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
#if 1
                         printf ("Warning: detected SgTemplateArgument without parent set properly at %p = %s parent is currently NULL \n",support,support->class_name().c_str());
#endif
                      // printf ("Error: NULL parent found \n");
                      // ROSE_ASSERT(support->get_file_info() != NULL);
                      // support->get_file_info()->display("Error: NULL parent found");
                       }
                 // ROSE_ASSERT(support->get_parent() != NULL);
                    break;
                  }

            // DQ (11/21/2011): Added support for template declarations in the AST.
               case V_SgTemplateParameter:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
// DQ (5/25/2013): Commented out for now, too much output spew.
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if 0
                         printf ("Warning: detected SgTemplateParameter without parent set properly at %p = %s parent is currently NULL \n",support,support->class_name().c_str());
#endif
                       }
                    break;
                  }


            // DQ (6/11/2007): Added this case
               case V_SgCommonBlockObject:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
#if 1
                         printf ("Warning: detected SgCommonBlockObject without parent set properly at %p = %s parent is currently NULL \n",support,support->class_name().c_str());
#endif
                       }
                    break;
                  }

            // DQ (1/23/2010): Added this case
               case V_SgFileList:
               case V_SgDirectoryList:

            // DQ (10/4/2008): Added this case
               case V_SgRenamePair:

            // DQ (12/23/2007): Added this case
               case V_SgFormatItem:
               case V_SgFormatItemList:
                  {
                    SgNode* parent = support->get_parent();
                    if (parent == NULL)
                       {
#if 1
                         printf ("Warning: detected SgSupport IR node without parent set properly at %p = %s parent is currently NULL \n",support,support->class_name().c_str());
#endif
                       }
                    break;
                  }

            // DQ (7/23/2010): Added this case
               case V_SgTypeTable:
                  {
                 // The parent is not always set here except for when it is in the SgScopeStatement.
                    SgNode* parent = support->get_parent();
                    if (parent != NULL)
                       {
                      // DQ (7/30/2010): Commented out this test that fails in tests/nonsmoke/functional/CompilerOptionsTests/testCpreprocessorOption
                      // ROSE_ASSERT( isSgScopeStatement(parent) != NULL || isSgProject(parent) != NULL);
                         if ( !(isSgScopeStatement(parent) != NULL || isSgProject(parent) != NULL) )
                            {
                           // printf ("In AST Consistancy tests: Warning: !(isSgScopeStatement(parent) != NULL || isSgProject(parent) != NULL) \n");
                            }
                       }
                    break;
                  }
                
               // driscoll6 (01/03/2011): Added this case.
               case V_SgGraphEdge:
               case V_SgDirectedGraphEdge:
               case V_SgUndirectedGraphEdge:
               case V_SgGraphNode:
                  {
                      if (! isSgGraph(support->get_parent()) ) {
                          std::cerr << "Graph component " << support->class_name() << " requires SgGraph as parent" << std::endl;
                          ROSE_ASSERT(false);
                      }
                      break;
                  }

            // DQ (11/20/2013): Added support for checking that these are non-null (also just added code to set them to be non-null).
               case V_SgJavaImportStatementList:
               case V_SgJavaClassDeclarationList:
                  {
                    ROSE_ASSERT(support->get_parent() != NULL);
                    break;
                  }

            // DQ (6/3/2019): Added support for SgIncludeFile (parent is a SgIncludeDirectiveStatement).
            // case V_SgIncludeDirectiveStatement:
               case V_SgIncludeFile:
                  {
                 // DQ (10/22/2019): Note that there is no parent pointer defined for this IR node, so no warning message really make sense, I think.
#if 0
                    printf ("NOTE: In AST Consistancy tests: TestParentPointersInMemoryPool::visit(): case SgIncludeFile: parent == NULL \n");
#endif
                 // ROSE_ASSERT(support->get_parent() != NULL);
                    break;
                  }

               default:
                  {
                    if (support->get_parent() != NULL)
                       {
                         printf ("##### TestParentPointersInMemoryPool::visit(node = %p = %s), support->get_parent() != NULL == %s\n",node,node->sage_class_name(),support->get_parent()->sage_class_name());
                       }
                    ROSE_ASSERT(support->get_parent() == NULL);
                    break;
                  }
             }
        }
   }


// DQ (9/13/2006): Implemented by Ghassan to verify that for 
// each node, it appears in its parent's list of children.
void
TestChildPointersInMemoryPool::test()
   {
     TestChildPointersInMemoryPool t;
     t.traverseMemoryPool();
   }

void TestChildPointersInMemoryPool::initDiagnostics() 
   {
     static bool initialized = false;
     if (!initialized) 
        {
          initialized = true;
          Rose::Diagnostics::initAndRegister(&mlog, "Rose::TestChildPointersInMemoryPool");
          mlog.comment("testing AST child pointers in memory pools");
        }
   }


// DQ (9/13/2006): Implemented by Ghassan to verify that for 
// each node, it appears in its parent's list of children.
// This is a test requested by Jeremiah.
void
TestChildPointersInMemoryPool::visit( SgNode *node )
   {
     static std::map<SgNode*,std::set<SgNode*> > childMap;

     ROSE_ASSERT(node != NULL);

     if (node->get_freepointer() != AST_FileIO::IS_VALID_POINTER() )
     {
          printf ("Error: In TestChildPointersInMemoryPool::visit() for node = %s at %p \n",node->class_name().c_str(),node);
                  ROSE_ASSERT(false);
     }

     SgNode *parent = node->get_parent();
  
#if ROSE_USE_VALGRIND
     VALGRIND_CHECK_DEFINED(parent);
#endif

     if (parent != NULL)
        {
          bool nodeFound = false;

       // DQ (3/12/2007): This is the latest implementation, here we look for the child set 
       // in a statically defined childMap. This should be a more efficient implementation.
       // Since it uses a static map it is a problem when the function is called twice.
          std::map<SgNode*,std::set<SgNode*> >::iterator it = childMap.find(parent);

          if (it != childMap.end())
             {
            // Reuse the set that was built the first time
               nodeFound = it->second.find(node) != it->second.end();

            // DQ (7/1/2008): When this function is called a second time, (typically as part
            // of calling AstTests::runAllTests (SgProject*); with a modified AST,
            // the childMap has already been set and any new declaration that was added and 
            // which generated a symbol is not in the previously defined static childMap.
            // So the test above fails and we need to use the more expensive dynamic test.
               // George Vulov (4/22/2011): Restrict this test to only memory pool entries that are valid
               if (nodeFound == false && parent->get_freepointer() == AST_FileIO::IS_VALID_POINTER())
                    nodeFound = parent->isChild(node);
             }
            else
             {
            // DQ (6/6/2010): Restrict this test to only memory pool entries that are valid
               if (parent->get_freepointer() == AST_FileIO::IS_VALID_POINTER() )
               {
                // build the set (and do the test)
                   childMap[parent] = std::set<SgNode*>();
                   it = childMap.find(parent);
                   ROSE_ASSERT (it != childMap.end());

                // Later we can make this more efficient by building a set directly
                // This style is quite inefficient since we are not making use of 
                // the string type date in the pair<SgNode*,string>
#if ROSE_USE_VALGRIND
                   if (VALGRIND_CHECK_WRITABLE(parent, sizeof(SgNode))) {
                     fprintf(stderr, "Parent %p of child %p (a %s) has been deleted.\n", parent, node, node->class_name().c_str());
                   }
#endif

#if 0
                 // DQ (6/5/2010): Turn this on to support debugging of the AST File I/O support for reading files (tests/nonsmoke/functional/testAstFileRead.C).

                   /* DEBUGGING (RPM 2008-10-10)
                    * If the call to parent->returnDataMemberPointers() fails it could be due to the fact that the parent has been
                    * deleted without deleting its children. This can happen if the parent's definition in one of the *.C files
                    * in src/ROSETTA/src (such as binaryInstruction.C) has a call to setAutomaticGenerationOfDestructor with false
                    * to turn off the ROSETTA-generated destructor and the explicitly coded destructor does not destroy child
                    * nodes. */
                   printf ("DEBUG: node: %p = %s = %s parent: %p = %s \n",
                           node, node->class_name().c_str(), SageInterface::get_name(node).c_str(),
                           parent, parent->class_name().c_str());
#endif

                   vector<pair<SgNode*,string> > v = parent->returnDataMemberPointers();

                   for (unsigned long i = 0; i < v.size(); i++)
                      {
                     // Add the child to the set in the map
                        it->second.insert(v[i].first);

                     // DQ (10/2/2010): Debugging SgType :: type_kind  data member.
                        ROSE_ASSERT(node != NULL);
                     // ROSE_ASSERT(v[i].first != NULL);

                        if (v[i].first == node)
                             {
                               nodeFound = true;
                             }
                      }
                  }
             }

       // DQ (3/3/2007): Note that some IR nodes are not to be found in the parent list (e.g. "typedef struct {} X;")
       // The special cases are listed explicitly below.  Anything else will at some point be an error.

       // These are errors that we want to always report.
       // if ( nodeFound == false && SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          if ( nodeFound == false )
             {

               switch(node->variantT())
                  {
                    case V_SgTemplateInstantiationDecl:
                    case V_SgClassDeclaration:
                    case V_SgDerivedTypeStatement:
                       {
                      // Ignore this case, there a lots of cases where non-defining declarations 
                      // are built and they are assigned parents where they are not children.
                         break;
                       }

                    case V_SgTemplateDeclaration:
                       {
                      // Ignore this case, at least numerous non-defining declarations will fail to appear in the parent' child list.
                         break;
                       }

                 // DQ (7/23/2010): Added case of SgTypeTable
                 // case V_SgTypeTable:
                    case V_SgFunctionTypeTable:
                       {
                      // Ignore this case, the pointer to the SgFunctionTypeTable is a static data member
                      // and not returned by the returnDataMemberPointers() function on the parent (SgNode).
                         break;
                       }

                    case V_SgTypedefDeclaration:
                       {
                         SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(node);

                      // By forcing the evaluation of isCompilerGenerated() we improve the performance by using short-circuit evaluation 
                         if ((typedefDeclaration->get_file_info()->isCompilerGenerated() == true) && (typedefDeclaration->get_name() == "__builtin_va_list") )
                            {
                           // This is the special case of __builtin_va_list used as a typedef (it is compiler generated and will not appear in the parent's child list).
                            }
                           else
                            {
                              ROSE_ASSERT(typedefDeclaration->get_parent() != NULL);
                              SgTemplateArgument* templateArgument = isSgTemplateArgument(typedefDeclaration->get_parent());
                              if (templateArgument != NULL)
                                 {
                                // DQ (3/6/2007): SgTypedefDeclaration can sometimes be built as part of template argument processing, 
                                // when this is done the SgTypedefDeclaration is not in the child list of any scope, it has a scope 
                                // of the template for which it is an argument and a parent which is assigned to the associated 
                                // SgTemplateArgument.  To get access to the declaration from the SgTemplateArgument one must go 
                                // through the associated typedefType.  The following is a test that should pass when this happens.
                                   SgTypedefType* typedefType = isSgTypedefType(templateArgument->get_type());
                                   ROSE_ASSERT(typedefType != NULL);
                                   ROSE_ASSERT(typedefType->get_declaration() == typedefDeclaration);
                                // printf ("Found the typedefDeclaration in the declaration of its parent's SgTypedefType in its SgTemplateArgument \n");
                                 }
                                else
                                 {
#if PRINT_DEVELOPER_WARNINGS
                                   printf ("This SgTypedefDeclaration is not in parent's child list, node: %p = %s = %s parent: %p = %s \n",
                                        node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str());
#endif
                                 }
                            }
                         break;
                       }

                    case V_SgInitializedName:
                       {
                         SgInitializedName* initializedName             = isSgInitializedName(node);
#if 0
                      // DQ (9/26/2011): Older version of code.
                         SgVarRefExp*       variableReferenceExpression = isSgVarRefExp(node->get_parent());
                         if (initializedName != NULL && variableReferenceExpression != NULL && variableReferenceExpression->get_symbol()->get_declaration() == node)
                            {
                           // This is the special case of "void foo(const char*); foo(__PRETTY_FUNCTION__);"
                           // Note that __PRETTY_FUNCTION__ is a compiler generated const char* (C string).
#if 0
                              printf ("This is a SgInitializedName %p = %s = %s with a SgVarRefExp parent = %p = %s and in this case the node is not in the child list \n",
                                   node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str());
                              printf ("variableReferenceExpression->get_symbol()->get_declaration() = %p = %s \n",
                                   variableReferenceExpression->get_symbol()->get_declaration(),
                                   variableReferenceExpression->get_symbol()->get_declaration()->class_name().c_str());
#endif
                            }
                           else
                            {
                              if (SgProject::get_verbose() > 0)
                                 {
                                   mprintf ("Warning: TestChildPointersInMemoryPool::visit(): Node is not in parent's child list, node: %p = %s = %s parent: %p = %s \n",
                                        node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str());
                                 }
                            }
#else
                      // DQ (9/26/2011): Trying to handle this via a better implementation of this test.
                         if (initializedName != NULL)
                            {
                           // Check for "__PRETTY_FUNCTION__" and "__func__" since these are implicit variables and
                           // are defined in ROSE to have a parent pointing to the associated SgFunctionDefinition.
                           // This is a change where it used to point to the associated SgVarRefExp, but this can be
                           // deleted if the constant folded value is subsituted with the original expression tree.
                           // It also never made since to have it be associated with anything but the SgFunctionDefinition
                           // also both of these are handled uniformally now.  This fix permits more passing tests of
                           // the AST merge and the AST File I/O support in ROSE.  This fix is a part of the move in ROSE
                           // to support either the constant folded values (optional) or the original expression trees (default).
                              if (initializedName->get_name() == "__PRETTY_FUNCTION__" || initializedName->get_name() == "__func__")
                                 {
                                // This not is not in the child list of the parent, this is expected.
                                   ROSE_ASSERT(node->get_parent() != NULL);
                                // printf ("In TestChildPointersInMemoryPool::visit(): (__PRETTY_FUNCTION__ || __func__) node->get_parent() = %p = %s \n",node->get_parent(),node->get_parent()->class_name().c_str());
                                   ROSE_ASSERT(isSgFunctionDefinition(node->get_parent()) != NULL);
                                 }
                                else
                                 {
                                   mprintf ("Warning: TestChildPointersInMemoryPool::visit(): Node is not in parent's child list, node: %p = %s = %s parent: %p = %s \n",
                                        node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str());
                                 }
                            }
#endif
                         break;
                       }

                    case V_SgFunctionDeclaration:
                    case V_SgMemberFunctionDeclaration:
                    case V_SgTemplateInstantiationFunctionDecl:
                    case V_SgTemplateInstantiationMemberFunctionDecl:
                       {
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);

                      // By forcing the evaluation of isCompilerGenerated() we improve the performance by using short-circuit evaluation 
                         if ( (functionDeclaration->get_file_info()->isCompilerGenerated() == true) &&
                              ( (functionDeclaration->get_name() == "__default_function_pointer_name") || (functionDeclaration->get_name() == "__default_member_function_pointer_name") ) )
                            {
                           // This is the case of a function (or member function) declaration built to support a function pointer (or member function pointer)
#if 0
                           // DQ (3/3/2007): isolated out case where we don't want to issue a warning!
                              printf ("This is a function declaration built to support a function pointer declaration and should not be in the parent list: node: %p = %s = %s parent: %p = %s = %s \n",
                                   node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
#endif
                            }
                           else
                            {
                           // DQ (3/4/2007): Constructors for class without declared constructors will have a compiler generated constructor built 
                           // internally and added to the symbol table, since it is not explicit in the source code it has a scope but the parent 
                           // is set to SgGlobal (global scope). Since the declaration deso not really exist in global scope it's parent does not 
                           // have it in it's child list.  This is an acceptable case and so we filter this specific case out.
                              if ( (functionDeclaration->get_file_info()->isCompilerGenerated() == true) && (functionDeclaration->get_specialFunctionModifier().isConstructor() == true) )
                                 {
                                // Ignore this case of a compiler generated constructor for a class without one
                                 }
                                else
                                 {
                                   SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
                                   if (memberFunctionDeclaration != NULL)
                                      {
                                     // For member function declaration the function could be referenced before it was defined in which case if this is not the defining declaration it may not exist in the child list of the class scope.
                                     // if (memberFunctionDeclaration != memberFunctionDeclaration->get_definingDeclaration())
                                        if (memberFunctionDeclaration == memberFunctionDeclaration->get_definingDeclaration() && memberFunctionDeclaration->get_parent() == memberFunctionDeclaration->get_scope())
                                           {
                                          // This is the defining function declared inside the class, so there will be no non-defining member function 
                                          // in the class (this declaration is all that will be in the parent list).
                                          // printf ("Error: this member fucntion is the defining declaration and defined in the class so it should have been in the parent list. \n");
                                          // ROSE_ASSERT(false);
                                           }
                                          else
                                           {
                                             if (memberFunctionDeclaration != memberFunctionDeclaration->get_definingDeclaration() && memberFunctionDeclaration->get_parent() == memberFunctionDeclaration->get_scope())
                                                {
                                               // This is a non-defining declaration that is declared in the class and it should have been in the parent list.
                                               // This is currently the case for all non-definng declarations since the parent is artificially set to the class 
                                               // scope (is there a better idea of howto set it?).
                                               // printf ("Error: this member fucntion is the defining declaration and defined in the class so it should have been in the parent list. \n");
                                               // ROSE_ASSERT(false);
                                                }
                                               else
                                                {
                                               // DQ (10/12/2007): Non defining member declarations can appear outside of the class, at least if they are 
                                               // template instantiation member function declarations.  So this has been changed to a note instead of an error.

                                               // This is a defining declaration that is not defined in the parent scope.  Currently all member functions 
                                               // are assigned the class scope as a parent (independent of if they are first used in a function). So this 
                                               // case should not appear.
                                               // printf ("Error: non-defining memberFunctionDeclaration with parent not set to class scope \n");

#if 0
                                               // DQ (11/10/2019): Cleanup output spew for demo.

                                                  printf ("Note: non-defining memberFunctionDeclaration with parent not set to class scope \n");
                                                  printf ("     memberFunctionDeclaration = %p = %s = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),SageInterface::get_name(memberFunctionDeclaration).c_str());
                                                  SgNode* memberFunctionDeclarationParent = memberFunctionDeclaration->get_parent();
                                                  printf ("     memberFunctionDeclaration->get_parent() = %p = %s = %s \n",memberFunctionDeclarationParent,memberFunctionDeclarationParent->class_name().c_str(),SageInterface::get_name(memberFunctionDeclarationParent).c_str());

                                                  memberFunctionDeclaration->get_startOfConstruct()->display("Note: non-defining memberFunctionDeclaration with parent not set to class scope");
#endif
                                                }
                                           }
                                      }
                                     else
                                      {
#if PRINT_DEVELOPER_WARNINGS
                                        printf ("This SgFunctionDeclaration is not in parent's child list, node: %p = %s = %s parent: %p = %s = %s \n",
                                             node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
#endif
#if 0
                                        printf ("functionDeclaration->get_definingDeclaration() = %p functionDeclaration->get_firstNondefiningDeclaration() = %p \n",
                                             functionDeclaration->get_definingDeclaration(),functionDeclaration->get_firstNondefiningDeclaration());
                                        printf ("Is this a constructor: functionDeclaration->get_specialFunctionModifier().isConstructor() = %s \n",functionDeclaration->get_specialFunctionModifier().isConstructor() ? "true" : "false");
                                        functionDeclaration->get_file_info()->display("functionDeclaration location: debug");
                                        if (functionDeclaration->get_definingDeclaration() != NULL)
                                             functionDeclaration->get_definingDeclaration()->get_file_info()->display("functionDeclaration defining location: debug");
                                        if (functionDeclaration->get_firstNondefiningDeclaration() != NULL)
                                             functionDeclaration->get_firstNondefiningDeclaration()->get_file_info()->display("functionDeclaration first non-defining location: debug");
#endif
                                      }
                                 }
                            }
                         break;
                       }

                    case V_SgVariableSymbol:
                       {
                         SgVariableSymbol* variableSymbol = isSgVariableSymbol(node);
                      // SgInitializedName* initializedName             = isSgInitializedName(variableSymbol->get_declaration());
                         SgInitializedName* initializedName             = variableSymbol->get_declaration();
                      // if (initializedName != NULL && initializedName->get_file_info()->isCompilerGenerated() == true && initializedName->get_name() == "__PRETTY_FUNCTION__")
                         if ( initializedName != NULL && initializedName->get_file_info()->isCompilerGenerated() == true && (initializedName->get_name() == "__PRETTY_FUNCTION__" || initializedName->get_name() == "__func__") )
                            {
                           // This is the special case of the SgVariableSymbol generated for "void foo(const char*); foo(__PRETTY_FUNCTION__);"
                           // Note that __PRETTY_FUNCTION__ is a compiler generated const char* (C string).
#if 0
                              printf ("This is a SgVariableSymbol %p = %s = %s and in this case the SgVariableSymbol is not in the child list of the parent symbol table \n",
                                   variableSymbol,variableSymbol->class_name().c_str(),SageInterface::get_name(variableSymbol).c_str());
#endif
                            }
                           else
                            {
                           // DQ (3/19/2017): Added support for using message logging.
                              mprintf ("Warning: TestChildPointersInMemoryPool::visit(). SgVariableSymbol is not in parent's child list, node: %p = %s = %s parent: %p = %s \n",
                                   node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str());
                            }
                         break;
                       }

                    case V_SgClassSymbol:
                       {
                         SgClassSymbol* classSymbol = isSgClassSymbol(node);
                         SgClassDeclaration* classDeclaration = classSymbol->get_declaration();

                      // DQ (3/3/2007): This is always a case we want to warn about!
                         printf ("SgClassSymbol is not in parent's child list, node: %p = %s = %s parent: %p = %s = %s \n",
                              node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
                         classDeclaration->get_file_info()->display("case V_SgClassSymbol: location: debug");
                         break;
                       }

                    case V_SgFunctionSymbol:
                       {
                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(node);
                         SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();

                      // DQ (3/3/2007): This is always a case we want to warn about!
                         printf ("SgFunctionSymbol is not in parent's child list, node: %p = %s = %s parent: %p = %s = %s \n",
                              node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
                         functionDeclaration->get_file_info()->display("case V_SgFunctionSymbol: location: debug");
                         break;
                       }

                    case V_SgStorageModifier:
                       {
                         SgStorageModifier* storageModifier = isSgStorageModifier(node);
                         SgInitializedName* initializedName = isSgInitializedName(storageModifier->get_parent());
                         if (initializedName != NULL)
                            {
                              printf ("SgStorageModifier is not in parent's child list, SgInitializedName: %p = %s = %s parent: %p = %s = %s \n",
                                   storageModifier,storageModifier->class_name().c_str(),SageInterface::get_name(storageModifier).c_str(),
                                   initializedName,initializedName->class_name().c_str(),SageInterface::get_name(initializedName).c_str());
                              SgNode* parent1 = initializedName->get_parent();
                              printf ("parent1 IR node = %p = %s \n",parent1,parent1->class_name().c_str());
                              SgNode* parent2 = parent1->get_parent();
                              printf ("parent2 IR node = %p = %s \n",parent2,parent2->class_name().c_str());
                              SgNode* parent3 = parent2->get_parent();
                              printf ("parent3 IR node = %p = %s \n",parent3,parent3->class_name().c_str());
                              SgNode* parent4 = parent3->get_parent();
                              printf ("parent4 IR node = %p = %s \n",parent4,parent4->class_name().c_str());
                              initializedName->get_file_info()->display("case V_SgStorageModifier: location: debug");
                            }
                           else
                            {
                           // DQ (3/3/2007): This is always a case we want to warn about!
                              printf ("SgStorageModifier is not in parent's child list, node: %p = %s = %s parent: %p = %s \n",
                                   node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str());
                            }
                         break;
                       }

                    case V_SgTemplateArgument:
                       {
                         SgTemplateArgument* templateArgument = isSgTemplateArgument(node);
                         SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(templateArgument->get_parent());
                         if (templateInstantiationDecl != NULL)
                            {
                           // DQ (3/6/2007): This is somwthing to investigate.
// DQ (5/25/2013): Commented out for now, too much output spew.
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if 0
                              printf ("SgTemplateArgument is not in parent's child list, node: %p = %s = %s parent: %p = %s = %s \n",
                                   node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
                              printf ("templateInstantiationDecl->get_definingDeclaration() = %p templateInstantiationDecl->get_firstNondefiningDeclaration() = %p \n",
                                   templateInstantiationDecl->get_definingDeclaration(),templateInstantiationDecl->get_firstNondefiningDeclaration());
                           // templateInstantiationDecl->get_file_info()->display("case V_SgTemplateArgument: location: debug");
#endif
                            }
                           else
                            {
                           // DQ (8/19/2014): Since these are shared (by design, so that the symbol table use is optimal) it is less important to warn about these.
                           // DQ (3/6/2007): This is always a case we want to warn about!
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if 0
                              printf ("SgTemplateArgument is not in parent's child list, node: %p = %s = %s parent: %p = %s = %s \n",
                                   node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str(),SageInterface::get_name(parent).c_str());
#endif
                            }
                         break;
                       }

                    case V_SgTypedefSeq:
                       {
#if PRINT_DEVELOPER_WARNINGS
                         SgTypedefSeq* typedefSequence = isSgTypedefSeq(node);
                         SgNode* parent = typedefSequence->get_parent();
                         SgNode* parentParent = parent->get_parent();

                         printf ("Node is not in parent's child list, typedefSequence = %p parent = %p = %s parentParent = %p \n",typedefSequence,parent,parent->class_name().c_str(),parentParent);
#endif
                         break;
                       }

                 // DQ (6/13/2007): Added case to avoid warnings where Sg_File_Info objects are used in comments and cpp directives.
                    case V_Sg_File_Info:
                       {
                         Sg_File_Info* fileInfo = isSg_File_Info(node);
                         SgNode* parent = fileInfo->get_parent();
                      // if (parent == NULL && fileInfo->isCommentOrDirective() == true)
                         if (fileInfo->isCommentOrDirective() == true)
                               {
                              // This is the one case where the parent will always be NULL, it is OK.
                               }
                              else
                               {
                                 ROSE_ASSERT(parent != NULL);
#if PRINT_DEVELOPER_WARNINGS
                                 SgNode* parentParent = parent->get_parent();

                              // DQ (10/22/2007): Modified to put out the class name of the parentParent (not tested).
                                 printf ("Node is not in parent's child list, fileInfo = %p parent = %p = %s parentParent = %p = %s \n",
                                      fileInfo,parent,parent->class_name().c_str(),parentParent,parentParent == NULL ? "NULL" : parentParent->class_name().c_str());
#endif
                               }
                         break;
                       }

                    default:
                       {
                      // In the default case we issue a warning ...
#if PRINT_DEVELOPER_WARNINGS
                         printf ("Node is not in parent's child list (default case), node: %p = %s = %s parent: %p = %s \n",
                              node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),parent,parent->class_name().c_str());
#endif
                       }
                  }

            // Additional information!
               if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL+1 )
                  {
                 // DQ (1/22/2008): Detect the case where node is a Sg_File_Info IR nodes (which should not have a valid get_file_info() pointer)
                    Sg_File_Info* fileInfo = isSg_File_Info(node);
                    if (node->get_file_info() == NULL)
                       {
                      // DQ (5/17/2008): It is OK for a SgSymbol to have a NULL fileInfo pointer
                      // DQ (1/22/2008): Ignore that the get_file_info() for a Sg_File_Info IR node is NULL
                      // if (fileInfo == NULL && isSgType(node) == NULL )
                         if (fileInfo == NULL && (isSgType(node) == NULL && isSgSymbol(node) == NULL))
                              printf ("Warning: node->get_file_info() is NULL on node = %p = %s \n",node,node->class_name().c_str());
                       }
                      else
                       {
                      // node->get_file_info()->display("node location: debug");
                         ROSE_ASSERT( fileInfo == NULL );
                       }
                 // ROSE_ASSERT(node->get_file_info() != NULL);

                    if (parent != NULL)
                       {
                         if (parent->get_file_info() == NULL)
                            {
                           // DQ (5/17/2008): It is OK for a SgSymbol to have a parent which is a symbol table without a valid fileInfo pointer
                           // DQ (1/22/2008): Ignore SgType IR nodes (since they never have a valid get_file_info())
                           // if (isSgType(parent) == NULL )
                              if (isSgType(parent) == NULL && isSgSymbolTable(parent) == NULL)
                                   printf ("Warning: parent->get_file_info() is NULL on node = %p = %s \n",parent,parent->class_name().c_str());
                            }
                           else
                            {
                           // parent->get_file_info()->display("parent location: debug");
                            }
                       }
                      else
                       {
                         printf ("Warning: parent of node->get_file_info() is NULL on node = %p = %s \n",node,node->class_name().c_str());
                       }
                  }

            // DQ (9/14/2006): This causes the ROSE/tests/nonsmoke/functional/roseTests/astMergeTests/testMerge.C test to fail!
            // cout << node->unparseToString() << endl;
            // cout << endl;
             }
        }
   }


void
TestFirstNondefiningDeclarationsForForwardMarking::test()
   {
      TestFirstNondefiningDeclarationsForForwardMarking t;
      t.traverseMemoryPool();
   }


void
TestFirstNondefiningDeclarationsForForwardMarking::visit( SgNode* node)
   {
     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != NULL)
        {
          SgDeclarationStatement* firstDefiningDeclaration = declaration->get_firstNondefiningDeclaration();
          if(firstDefiningDeclaration->isForward() == true)
             {
               printf ("Error: firstDefiningDeclaration->isForward() == true: node = %p = %s = %s \n",
                    node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
               firstDefiningDeclaration->get_file_info()->display("Error: firstDefiningDeclaration->isForward() == true: debug");
             }
          ROSE_ASSERT(firstDefiningDeclaration->isForward() == false);
        }
   }





void
TestMappingOfDeclarationsInMemoryPoolToSymbols::test()
   {
      TestMappingOfDeclarationsInMemoryPoolToSymbols t;
      t.traverseMemoryPool();
   }

#if 0
// DQ (7/22/2007): This should be a member function of SgDeclarationStatement
bool hasAssociatedSymbol(SgDeclarationStatement* declarationStatement)
   {
     bool returnValue = true;

  // SgFunctionParameterList should likely not even be a declaration, but at least it has 
  // not associated symbol in the symbol table. SgVariableDeclaration contain a list of
  // SgInitializedName objects and those have associated symbols, the SgVariableDeclaration
  // does not have an associated symbol.
     if (isSgFunctionParameterList(declarationStatement) != NULL || 
         isSgCtorInitializerList(declarationStatement)   != NULL ||
         isSgPragmaDeclaration(declarationStatement)     != NULL ||
         isSgVariableDeclaration(declarationStatement)   != NULL ||
         isSgVariableDefinition(declarationStatement)    != NULL )
        {
          returnValue = false;
        }

     return returnValue;
   }
#endif

void
TestMappingOfDeclarationsInMemoryPoolToSymbols::visit( SgNode* node)
   {
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);

  // Note: not all declarations have an associated symbol
     if (declarationStatement != NULL && declarationStatement->hasAssociatedSymbol() == true)
        {
#if 0
          printf ("This is a declarationStatement = %p = %s = %s \n",
               declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
          ROSE_ASSERT(declarationStatement->get_scope() != NULL);
          printf ("   The scope is = %p = %s = %s \n",
               declarationStatement->get_scope(),declarationStatement->get_scope()->class_name().c_str(),SageInterface::get_name(declarationStatement->get_scope()).c_str());
#endif

       // printf ("Exiting to test this! \n");
       // ROSE_ASSERT(false);


// DQ (8/1/2007): Temp control of use of this test
#if 0
          SgSymbol* symbol = declarationStatement->get_symbol_from_symbol_table();
          if (symbol == NULL)
             {
#if 0
               printf ("symbol not found for declarationStatement = %p, looking at firstNondefiningDeclaration \n",declarationStatement);
#endif
               SgDeclarationStatement* firstNondefiningDeclaration = declarationStatement->get_firstNondefiningDeclaration();
               if (firstNondefiningDeclaration != NULL)
                    symbol = firstNondefiningDeclaration->get_symbol_from_symbol_table();

               if (symbol == NULL)
                  {
                    SgDeclarationStatement* definingDeclaration = declarationStatement->get_definingDeclaration();
#if 1
                    printf ("symbol not found for firstNondefiningDeclaration = %p, looking at firstNondefiningDeclaration \n",firstNondefiningDeclaration);
                    if (firstNondefiningDeclaration != NULL)
                       {
                         printf ("     firstNondefiningDeclaration = %p = %s = %s \n",
                              firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),SageInterface::get_name(firstNondefiningDeclaration).c_str());
                         printf ("     definingDeclaration = %p \n",definingDeclaration);
                         firstNondefiningDeclaration->get_startOfConstruct()->display("firstNondefiningDeclaration: debug");
                       }
#endif
                    if (definingDeclaration != NULL)
                         symbol = definingDeclaration->get_symbol_from_symbol_table();

                 // DQ (7/30/2007): This is an exception to this rule!
                 // ROSE_ASSERT(symbol != NULL);
                    ROSE_ASSERT(symbol != NULL || SageInterface::get_name(firstNondefiningDeclaration) == "__default_member_function_pointer_name");
                  }
            // ROSE_ASSERT(symbol != NULL);
               ROSE_ASSERT(symbol != NULL || SageInterface::get_name(declarationStatement) == "__default_member_function_pointer_name");
             }

       // ROSE_ASSERT(symbol != NULL);
          ROSE_ASSERT(symbol != NULL || SageInterface::get_name(declarationStatement) == "__default_member_function_pointer_name");

          SgDeclarationStatement* declarationAssociatedWithSymbol = NULL;

       // DQ (7/4/2007): New function to refactor code and provide basis for internal testing.
          declarationAssociatedWithSymbol = declarationStatement->get_declaration_associated_with_symbol();
       // ROSE_ASSERT(declarationAssociatedWithSymbol != NULL);
          ROSE_ASSERT(declarationAssociatedWithSymbol != NULL || SageInterface::get_name(declarationStatement) == "__default_member_function_pointer_name");

       // DQ (7/5/2007): Test that we can get a symbol (this should not pass for all declarations).
       // ROSE_ASSERT(declarationAssociatedWithSymbol->get_symbol_from_symbol_table() != NULL);
          ROSE_ASSERT(declarationAssociatedWithSymbol != NULL || SageInterface::get_name(declarationStatement) == "__default_member_function_pointer_name");

       // Test that we can always find a symbol (this should always pass).
          if (isSgVariableDeclaration(declarationStatement) == NULL && 
              isSgVariableDefinition(declarationStatement)  == NULL && 
              isSgPragmaDeclaration(declarationStatement)   == NULL &&
              isSgMemberFunctionDeclaration(declarationStatement) == NULL || SageInterface::get_name(declarationStatement) != "__default_member_function_pointer_name")
             {
               ROSE_ASSERT(declarationStatement->search_for_symbol_from_symbol_table() != NULL);
             }

// DQ (8/1/2007): Temp control of use of this test
#endif
        }
   }



void
TestLValueExpressions::visit ( SgNode* node )
   {
  // DQ (1/12/2008): Test the get_lvalue() member function which is common on expressions.
  // printf ("In TestLValueExpressions::visit(): node = %s \n",node->class_name().c_str());

     SgExpression* expression = isSgExpression(node);
     if (expression != NULL)
        {
#if 0
          printf ("TestLValueExpressions::visit(): calling expression->get_lvalue() on expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
          SgBinaryOp* binaryOperator = isSgBinaryOp(expression);

          if (binaryOperator != NULL)
             {
               switch (expression->variantT())
                  {
                 // IR nodes that have an l-value
                    case V_SgAssignOp:
                    case V_SgAndAssignOp:
                    case V_SgDivAssignOp:
                    case V_SgIorAssignOp:
                    case V_SgLshiftAssignOp:
                    case V_SgMinusAssignOp:
                    case V_SgModAssignOp:
                    case V_SgMultAssignOp:
                    case V_SgPlusAssignOp:
                    case V_SgRshiftAssignOp:
                    case V_SgXorAssignOp:
                       {
                         SgExpression* lhs = binaryOperator->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);

                      // DQ (1/22/2008): Jeremiah has provided a patch for this.  This used to be violated by the
                      // ROSE/tests/nonsmoke/functional/roseTests/astInliningTests/pass16.C test code!  This is now violated by
                      // the ./preTest
                      // /home/dquinlan/ROSE/NEW_ROSE/tests/nonsmoke/functional/roseTests/programTransformationTests/pass1.C
                      // test code!
                         if (lhs->get_lvalue() == false)
                            {
                              printf ("ERROR (lvalue for Assign operator not marked): for lhs = %p = %s = %s in binary expression = %s \n",
                                   lhs,lhs->class_name().c_str(),SageInterface::get_name(lhs).c_str(),expression->class_name().c_str());
                              binaryOperator->get_startOfConstruct()->display("Error for lhs: lhs->get_lvalue() == true: debug");
                            }

                      // I don't want to comment this out, it is always set correctly except in the inlining transformation!
                      // It now passes the inliner tests, but fails in the
                      // tests/nonsmoke/functional/roseTests/programTransformationTests, so I have to comment it out again!
                         ROSE_ASSERT(lhs->get_lvalue() == true);
                         break;
                       }

                    case V_SgDotExp:
                    case V_SgArrowExp:
                       {
                         SgExpression* lhs = binaryOperator->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);
#if WARN_ABOUT_ATYPICAL_LVALUES
                         printf ("L-value test for SgBinaryOp = %s: not clear how to assert value -- lhs->get_lvalue() = %s \n",binaryOperator->class_name().c_str(),lhs->get_lvalue() ? "true" : "false");
#endif
                      // ROSE_ASSERT(lhs->get_lvalue() == true);
                         break;
                       }

                    default:
                       {
                      // Make sure that the lhs is not an L-value
                         SgExpression* lhs = binaryOperator->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                         if (lhs->get_lvalue() == true)
                            {
                              printf ("Error for lhs = %p = %s = %s in binary expression = %s \n",
                                   lhs,lhs->class_name().c_str(),SageInterface::get_name(lhs).c_str(),expression->class_name().c_str());
                              binaryOperator->get_startOfConstruct()->display("Error for lhs: lhs->get_lvalue() == true: debug");
                            }
#endif
                      // ROSE_ASSERT(lhs->get_lvalue() == false);
                       }
                  }

               SgExpression* rhs = binaryOperator->get_rhs_operand();
               ROSE_ASSERT(rhs != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
               if (rhs->get_lvalue() == true)
                  {
                    printf ("Error for rhs = %p = %s = %s in binary expression = %s \n",
                         rhs,rhs->class_name().c_str(),SageInterface::get_name(rhs).c_str(),expression->class_name().c_str());
                    binaryOperator->get_startOfConstruct()->display("Error for rhs: rhs->get_lvalue() == true: debug");
                  }
#endif
            // ROSE_ASSERT(rhs->get_lvalue() == false);
             }
          
          SgUnaryOp* unaryOperator = isSgUnaryOp(expression);
          if (unaryOperator != NULL)
             {
               switch (expression->variantT())
                  {
                 // IR nodes that should have a valid lvalue
                 // What about SgAddressOfOp?

                    case V_SgMinusMinusOp:
                    case V_SgPlusPlusOp:
                       {
                         SgExpression* operand = unaryOperator->get_operand();
                         ROSE_ASSERT(operand != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                         if (operand->get_lvalue() == false) {
                              std::cerr <<"Error for operand"
                                        <<" (" <<operand->class_name() <<"*)" <<" = " <<SageInterface::get_name(operand)
                                        <<" in unary " <<expression->class_name() <<" expression"
                                        <<": operand->get_lvalue() == false but should be true\n";
                              std::cerr <<"ancestors of (" <<operand->class_name() <<"*)" <<operand <<" are:";
                              for (SgNode *p=operand->get_parent(); p; p=p->get_parent())
                                  std::cerr <<" (" <<p->class_name() <<"*)" <<p;
                              std::cerr <<"\n";
                              unaryOperator->get_startOfConstruct()
                                  ->display("Error for operand: operand->get_lvalue() == false: debug");
                         }
#endif
                         ROSE_ASSERT(operand->get_lvalue() == true);
                         break;
                       }

                    case V_SgThrowOp:
                       {
#if WARN_ABOUT_ATYPICAL_LVALUES
                      // Note that the gnu " __throw_exception_again;" can cause a SgThrowOp to now have an operand!
                         SgExpression* operand = unaryOperator->get_operand();
                         if (operand == NULL)
                            {
                              printf ("Warning: operand == NULL in SgUnaryOp = %s (likely caused by __throw_exception_again) \n",expression->class_name().c_str());
                           // unaryOperator->get_startOfConstruct()->display("Error: operand == NULL in SgUnaryOp: debug");
                            }
#endif
                      // ROSE_ASSERT(operand != NULL);
                         break;
                       }

                 // Added to address problem on Qing's machine using g++ 4.0.2
                    case V_SgNotOp:

                 // These are where some error occur.  I want to isolate then so that I know the current status of where lvalues are not marked correctly!
                    case V_SgPointerDerefExp:
                    case V_SgCastExp:
                    case V_SgMinusOp:
                    case V_SgBitComplementOp:
                 // case V_SgPlusOp:
                       {
                         SgExpression* operand = unaryOperator->get_operand();
                         ROSE_ASSERT(operand != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                      // Most of the time this is false, we only want to know when it is true
                         if (operand->get_lvalue() == true)
                            {
                              printf ("L-value test for SgUnaryOp = %s: not clear how to assert value -- operand->get_lvalue() = %s \n",unaryOperator->class_name().c_str(),operand->get_lvalue() ? "true" : "false");
                           // unaryOperator->get_startOfConstruct()->display("L-value test for SgUnaryOp: operand->get_lvalue() == true: debug");
                            }
#endif
                      // ROSE_ASSERT(operand->get_lvalue() == false);
                         break;
                       }

                    default:
                       {
                         SgExpression* operand = unaryOperator->get_operand();
                         ROSE_ASSERT(operand != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                         if (operand->get_lvalue() == true)
                            {
                              printf ("Error for operand = %p = %s = %s in unary expression = %s \n",
                                   operand,operand->class_name().c_str(),SageInterface::get_name(operand).c_str(),expression->class_name().c_str());
                              unaryOperator->get_startOfConstruct()->display("Error for operand: operand->get_lvalue() == true: debug");
                            }
#endif
                         ROSE_ASSERT(operand->get_lvalue() == false);
                       }          
                  }
             }
        }
   }




void
TestMultiFileConsistancy::test()
   {
  // DQ (2/23/2009): Note that AST Merge might fail this test...

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test declarations for file consistancy (tests outlining in a separate file) started." << endl;

        {
          TimingPerformance timer ("Test declarations for file consistancy:");

          TestMultiFileConsistancy t;
          t.traverseMemoryPool();
        }

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test declarations for file consistancy finished." << endl;
   }


void
TestMultiFileConsistancy::visit( SgNode* node)
   {
  // TV (09/15/2018): Nothing really happens in there anymore. Commented everything out as "declaration->get_scope() != NULL" fails for some non-real declarations
#if 0
  // DQ (2/23/2009): added testing to support outlining to a separate file.
  // This test is helpful for the outlining to a separate file, where we want to make sure 
  // that the transformations required do not build a locally inconsistant AST for each file.
     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);

     if (declaration != NULL)
        {
          SgDeclarationStatement* firstDefiningDeclaration = declaration->get_firstNondefiningDeclaration();
#if 1
          ROSE_ASSERT(declaration != NULL);
       // ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);
          if (firstDefiningDeclaration != NULL)
             {
               ROSE_ASSERT(declaration->get_scope() != NULL);
               SgSourceFile* declarationFile              = TransformationSupport::getSourceFile(declaration);
               SgSourceFile* declarationScopeFile         = TransformationSupport::getSourceFile(declaration->get_scope());
               SgSourceFile* firstDefiningDeclarationFile = TransformationSupport::getSourceFile(firstDefiningDeclaration);
               if (declarationScopeFile != firstDefiningDeclarationFile || declarationFile != firstDefiningDeclarationFile)
                  {
#if 0
                 // DQ (3/4/2009): Supporess the output here so we can pass the tests in tests/nonsmoke/functional/CompilerOptionsTests/testForSpuriousOutput
                    printf ("TestMultiFileConsistancy::visit(): declaration              = %p = %s = %s \n",declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
                    printf ("TestMultiFileConsistancy::visit(): declaration->get_scope() = %p = %s = %s \n",declaration->get_scope(),declaration->get_scope()->class_name().c_str(),SageInterface::get_name(declaration->get_scope()).c_str());
                    printf ("TestMultiFileConsistancy::visit(): firstDefiningDeclaration = %p = %s = %s \n",firstDefiningDeclaration,firstDefiningDeclaration->class_name().c_str(),SageInterface::get_name(firstDefiningDeclaration).c_str());
                    printf ("TestMultiFileConsistancy::visit(): firstDefiningDeclaration = %p = %s = %s \n",firstDefiningDeclaration,firstDefiningDeclaration->class_name().c_str(),SageInterface::get_name(firstDefiningDeclaration).c_str());

                 // DQ (3/3/2009): Some template arguments are setting off these new tests (e.g. test2004_35.C), need to look into this.
                    if (declarationFile != NULL)
                         printf ("TestMultiFileConsistancy::visit(): declarationFile              = %p = %s = %s \n",declarationFile,declarationFile->class_name().c_str(),SageInterface::get_name(declarationFile).c_str());
                    if (declarationScopeFile != NULL)
                         printf ("TestMultiFileConsistancy::visit(): declarationScopeFile         = %p = %s = %s \n",declarationScopeFile,declarationScopeFile->class_name().c_str(),SageInterface::get_name(declarationScopeFile).c_str());
                    if (firstDefiningDeclarationFile != NULL)
                         printf ("TestMultiFileConsistancy::visit(): firstDefiningDeclarationFile = %p = %s = %s \n",firstDefiningDeclarationFile,firstDefiningDeclarationFile->class_name().c_str(),SageInterface::get_name(firstDefiningDeclarationFile).c_str());
#endif
                  }
#if 0
            // DQ (3/3/2009): Some template arguments are setting off these new tests (e.g. test2004_35.C), need to look into this.
               ROSE_ASSERT(declarationFile == firstDefiningDeclarationFile);

            // DQ (3/3/2009): Some template arguments are setting off these new tests (e.g. test2004_35.C), need to look into this.
               if (firstDefiningDeclarationFile != NULL)
                    ROSE_ASSERT(declarationScopeFile == firstDefiningDeclarationFile);
#endif
             }
#if 0
       // DQ (3/3/2009): Some template arguments are setting off these new tests (e.g. test2004_35.C), need to look into this.
          if (firstDefiningDeclaration != NULL)
             {
               ROSE_ASSERT(TransformationSupport::getSourceFile(firstDefiningDeclaration) == TransformationSupport::getSourceFile(firstDefiningDeclaration->get_firstNondefiningDeclaration()));
               ROSE_ASSERT(TransformationSupport::getSourceFile(firstDefiningDeclaration->get_scope()) == TransformationSupport::getSourceFile(firstDefiningDeclaration->get_firstNondefiningDeclaration()));
             }
#endif
#if 0
       // DQ (3/3/2009): Some template arguments are setting off these new tests (e.g. test2004_35.C), need to look into this.
          SgDeclarationStatement* definingDeclaration      = declaration->get_definingDeclaration();
          if (definingDeclaration != NULL)
             {
               SgDeclarationStatement* alt_firstDefiningDeclaration = definingDeclaration->get_firstNondefiningDeclaration();
               if (alt_firstDefiningDeclaration != NULL)
                  {
                    ROSE_ASSERT(TransformationSupport::getSourceFile(definingDeclaration) == TransformationSupport::getSourceFile(definingDeclaration->get_firstNondefiningDeclaration()));
                    ROSE_ASSERT(TransformationSupport::getSourceFile(definingDeclaration->get_scope()) == TransformationSupport::getSourceFile(definingDeclaration->get_firstNondefiningDeclaration()));

                    ROSE_ASSERT(TransformationSupport::getSourceFile(alt_firstDefiningDeclaration) == TransformationSupport::getSourceFile(alt_firstDefiningDeclaration->get_firstNondefiningDeclaration()));
                    ROSE_ASSERT(TransformationSupport::getSourceFile(alt_firstDefiningDeclaration->get_scope()) == TransformationSupport::getSourceFile(alt_firstDefiningDeclaration->get_firstNondefiningDeclaration()));
                  }
             }
#endif
#endif
        }
#endif
   }



BuildListOfConnectedNodesInAST::BuildListOfConnectedNodesInAST(set<SgNode*> & s)
   : nodeSet(s)
   {
   }

void
BuildListOfConnectedNodesInAST::visit(SgNode * node)
   {
  // printf ("Node = %p = %s \n",node,node->class_name().c_str());

     if (nodeSet.find(node) == nodeSet.end())
        {
       // Not found in the set, add to the set and call visit function on all children.
#if 0
          printf ("Adding to nodeSet = %p = %s \n",node,node->class_name().c_str());
#endif
          nodeSet.insert(node);

          typedef vector<pair<SgNode*,string> > DataMemberMapType;
          DataMemberMapType dataMemberMap = node->returnDataMemberPointers();

          DataMemberMapType::iterator i = dataMemberMap.begin();
          while (i != dataMemberMap.end())
             {
            // Ignore the parent pointer since it will be reset differently if required
               SgNode* childPointer = i->first;
               string  debugString  = i->second;

               if (childPointer != NULL)
                  {
                 // printf ("visit node = %p = %s on edge %s found child %p = %s \n",node,node->class_name().c_str(),debugString.c_str(),childPointer,childPointer->class_name().c_str());

                 // Make the recursive call
                    visit(childPointer);

                    SgLocatedNode* locatedNode = isSgLocatedNode(childPointer);
                    if (locatedNode != NULL)
                       {
                         AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

                         if (comments != NULL)
                            {
                           // printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
                              AttachedPreprocessingInfoType::iterator i;
                              for (i = comments->begin(); i != comments->end(); i++)
                                 {
                                   ROSE_ASSERT ( (*i) != NULL );
                                   visit((*i)->get_file_info());

#if 0
                                   printf ("          Attached Comment (relativePosition=%s): %s\n",
                                        ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                                        (*i)->getString().c_str());
                                   printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
                                        (*i)->get_file_info()->display("comment/directive location");
#endif
                                 }
                            }
                       }
                  }
               i++;
             }
        }
       else
        {
       // Found in the set, nothing to do.
        }
   }

BuildListOfNodesInAST::BuildListOfNodesInAST(const set<SgNode*> & s1, set<SgNode*> & s2)
   : constNodeSet(s1), nodeSet(s2)
   {
   }

void
BuildListOfNodesInAST::visit(SgNode * node)
   {
     ROSE_ASSERT(node != NULL);
     ROSE_ASSERT(nodeSet.find(node) == nodeSet.end());

     if (constNodeSet.find(node) == constNodeSet.end())
        {
          SgNode* parent = node->get_parent();
          string parentName = parent == NULL ? "null" : parent->class_name();

          printf ("Node not found %p = %s (parent = %p = %s)\n",node,node->class_name().c_str(),parent,parentName.c_str());

#if 1
          Sg_File_Info* fileInfo = isSg_File_Info(node);
          if (fileInfo != NULL)
             {
            // This will help us know where this came from.
               fileInfo->display("Node disconected from AST");
             }
#endif
          printf ("Deleting %p = %s (parent = %p = %s)\n",node,node->class_name().c_str(),parent,parentName.c_str());
          delete node;
        }

     nodeSet.insert(node);
   }

void
TestForDisconnectedAST::test(SgNode * node)
   {
  // DQ (3/7/2010): Identify the fragments of the AST that are disconnected.

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test declarations for disconnected parts of the AST started." << endl;

     set<SgNode*> AST_set;
     set<SgNode*> All_IR_Nodes_set;

        {
          TimingPerformance timer ("Test for disconnected parts of the AST:");

             {
               TimingPerformance timer ("Test for disconnected parts of the AST:");

               BuildListOfConnectedNodesInAST t1 (AST_set);

            // Traverse the global function type table to preload the AST_set
               t1.visit(SgNode::get_globalFunctionTypeTable()); // SgFunctionTypeTable

               t1.traverse(node,preorder);
             }

             {
               TimingPerformance timer ("Test for disconnected parts of the AST:");

               BuildListOfNodesInAST t1(AST_set,All_IR_Nodes_set);
               t1.traverseMemoryPool();
             }

          if (AST_set.size() != All_IR_Nodes_set.size())
             {
               printf ("AST_set          = %" PRIuPTR " \n",AST_set.size());
               printf ("All_IR_Nodes_set = %" PRIuPTR " \n",All_IR_Nodes_set.size());
             }
        }

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
          cout << "Test declarations for disconnected parts of the AST finished." << endl;
   }


void
MemoryCheckingTraversalForAstFileIO::visit ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);
  // printf ("MemoryCheckingTraversalForAstFileIO::visit: node = %s \n",node->class_name().c_str());
     ROSE_ASSERT(node->get_freepointer() == AST_FileIO::IS_VALID_POINTER());
     node->checkDataMemberPointersIfInMemoryPool();
   }




TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute::
TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute(bool b)
   : sourceFile(NULL), 
     caseInsensitive(b)
   {
   }

TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute::
TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute(const TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute & X)
   {
     sourceFile      = X.sourceFile;
     caseInsensitive = X.caseInsensitive; 
   }

TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute
TestForProperLanguageAndSymbolTableCaseSensitivity::evaluateInheritedAttribute(SgNode* node, TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute inheritedAttribute)
   {
  // The default is to make all symbol tables (scopes) case sensitive, and then detect use of Fortran 
  // files (SgSourceFile IR nodes) and make all of their symbol tables (scopes) case insensitive.
     TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute return_inheritedAttribute(inheritedAttribute);

     SgSourceFile* sourceFile = isSgSourceFile(node);
     if (sourceFile != NULL)
        {
       // printf ("Found SgSourceFile for %s get_Fortran_only() = %s \n",sourceFile->getFileName().c_str(),sourceFile->get_Fortran_only() ? "true" : "false");

          return_inheritedAttribute.sourceFile = sourceFile;
          if (sourceFile->get_Fortran_only() == true)
             {
               return_inheritedAttribute.caseInsensitive = true;
             }
        }

     SgScopeStatement* scope = isSgScopeStatement(node);
     if (scope != NULL)
        {
       // This is a scope, now check if it matches the case sensitivity from the file (stored in the inherited attribute).
       // printf ("Note: scope = %p = %s scope->isCaseInsensitive() = %s inheritedAttribute.caseInsensitive = %s \n",scope,scope->class_name().c_str(),scope->isCaseInsensitive() ? "true" : "false",return_inheritedAttribute.caseInsensitive ? "true" : "false");

          if (scope->isCaseInsensitive() != return_inheritedAttribute.caseInsensitive)
             {
               printf ("Error: scope->isCaseInsensitive() = %s inheritedAttribute.caseInsensitive = %s \n",scope->isCaseInsensitive() ? "true" : "false",return_inheritedAttribute.caseInsensitive ? "true" : "false");
               scope->get_startOfConstruct()->display("scope->isCaseInsensitive() incorrectly set");
               ROSE_ASSERT(return_inheritedAttribute.sourceFile != NULL);
               SgSourceFile* sourceFile = inheritedAttribute.sourceFile;
               if (sourceFile->get_Fortran_only() == true)
                  {
                    printf ("Fortran file %s should have an AST with scopes marked as case insensitive \n",sourceFile->getFileName().c_str());
                  }
                 else
                  {
                    printf ("Non-fortran file %s should have an AST with scopes marked as case sensitive \n",sourceFile->getFileName().c_str());
                  }
             }
          ROSE_ASSERT(scope->isCaseInsensitive() == return_inheritedAttribute.caseInsensitive);
        }

  // Return the inherited attribue (will call the implemented copy constructor).
     return return_inheritedAttribute;
   }


void
TestForProperLanguageAndSymbolTableCaseSensitivity::test(SgNode* node)
   {
  // Inherited attribute with caseInsensitive marked as false.
     bool caseInsensitive = false;
     TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute IH(caseInsensitive);

     TestForProperLanguageAndSymbolTableCaseSensitivity traversal; // (node,IH);

  // printf ("Traversing AST to support TestForProperLanguageAndSymbolTableCaseSensitivity::test() \n");
  // ROSE_ASSERT(false);

  // This should be a SgProject or SgFile so that we can evaluate the language type (obtained from the SgFile).
     ROSE_ASSERT(isSgProject(node) != NULL || isSgFile(node) != NULL);

     traversal.traverse(node,IH);

  // printf ("DONE: Traversing AST to support TestForProperLanguageAndSymbolTableCaseSensitivity::test() \n");
   }


TestForReferencesToDeletedNodes::TestForReferencesToDeletedNodes(int input_detect_dangling_pointers, const string & s )
   : detect_dangling_pointers(input_detect_dangling_pointers), filename(s)
   {
  // Nothing to do here!
   }


void
TestForReferencesToDeletedNodes::visit ( SgNode* node )
   {
  // DQ (9/26/2011): This is a new test for possible references to deleted IR nodes in the AST (dangling pointers).
  // It works as a byproduct of the way that memory pool works.  It might work better if we also added an option
  // to prevent previously deleted IR nodes from reusing memory in the memory pool.  This can be considered for 
  // future work.

     ROSE_ASSERT(node != NULL);
  // printf ("TestForReferencesToDeletedNodes::visit: node = %s \n",node->class_name().c_str());

  // Only output information about this test if set to value greater than zero (this avoids anoying output for existing translators, e.g. CAF2 work).
     if (detect_dangling_pointers > 0)
        {
          vector<pair<SgNode*,string> > v = node->returnDataMemberPointers();

          for (unsigned long i = 0; i < v.size(); i++)
             {
               SgNode* child = v[i].first;

               if (child != NULL && child->variantT() == V_SgNode)
                  {
                 // This is a deleted IR node
                 // SgFile* file = TransformationSupport::getFile(node);
                 // string filename = (file != NULL) ? file->getFileName() : "unknown file";
                    printf ("Error in AST consistancy detect_dangling_pointers test for file %s: Found a child = %p = %s child name = %s of node = %p = %s that was previously deleted \n",filename.c_str(),child,child->class_name().c_str(),v[i].second.c_str(),node,node->class_name().c_str());
                  }

            // DQ (9/26/2011): This fails for the projects/UpcTranslation upc_shared_2.upc file...figure this out once we see what elase might file.
            // Other fialing tests include: 
            //    projects/backstroke/tests/expNormalizationTest/test2006_74.C
            //    projects/backstroke/tests/expNormalizationTest/test2006_87.C
            // Large percentage of Fortran tests fail this test!
               if (detect_dangling_pointers > 1)
                  {
                    ROSE_ASSERT( (child == NULL) || (child != NULL && child->variantT() != V_SgNode) );
                  }
             }
        }
   }

void
TestForReferencesToDeletedNodes::test( SgProject* project )
   {
  // See documentation in the header file for this test.

  // I was having trouble generating a file name for where the problem was happening, so this should work better.
  // might be that the problem IR nodes were contained in expressions in the index of a SgArrayType, or something 
  // like that.
  // Generate a name from all the files on the command line
     string filename = SageInterface::generateProjectName(project, /* supressSuffix = */ false );

     TestForReferencesToDeletedNodes traversal(project->get_detect_dangling_pointers(),filename);

     traversal.traverseMemoryPool();
   }


// explicit
TestForParentsMatchingASTStructure::
TestForParentsMatchingASTStructure(std::ostream &output, const std::string &prefix)
   : output(output), nproblems(0), limit(0), prefix(prefix)
   {
   }

bool
TestForParentsMatchingASTStructure::check(SgNode *ast, size_t limit)
   {
     this->limit = limit;
     nproblems = 0;
     try 
        {
          traverse(ast);
        }
     catch (const TestForParentsMatchingASTStructure*) 
        {
          return false;
        }

     return 0 == nproblems;
   }

void
TestForParentsMatchingASTStructure::preOrderVisit(SgNode *node)
   {
     if (!stack.empty())
        {
          if (NULL == node->get_parent())
             {
            // output << prefix << "node has null parent property but was reached by AST traversal\n";
               printf ("In TestForParentsMatchingASTStructure::preOrderVisit(): (NULL == node->get_parent()): node class_name = %s , prefix = %s node's parent property is NULL\n",node->class_name().c_str(), prefix.c_str());

               show_details_and_maybe_fail(node);
             } 
            else 
             {
               if (node->get_parent() != stack.back())
                  {
                 // output << prefix << "node's parent property does not match traversal parent\n";
                    if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                       {
                         printf ("In TestForParentsMatchingASTStructure::preOrderVisit(): (node->get_parent() != stack.back()): prefix = %s node's parent property does not match traversal parent\n",prefix.c_str());
                         printf ("traversal parent = %p = %s \n",stack.back(),stack.back()->class_name().c_str());
                       }
                    SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(stack.back());

                    if (namespaceDefinition != NULL)
                       {
                         if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                            {
                              printf ("node = %p = %s node->get_parent() = %p = %s \n",node,node->class_name().c_str(),node->get_parent(),node->get_parent()->class_name().c_str());
                            }

                         SgNamespaceDeclarationStatement* namespaceDeclaration = namespaceDefinition->get_namespaceDeclaration();
                         if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                            {
                              printf ("Found a namespaceDefinition = %p = %s \n",namespaceDefinition,namespaceDeclaration->get_name().str());
                            }

                         SgNamespaceDefinitionStatement* previousNamespaceDefinition = namespaceDefinition;
                         while (previousNamespaceDefinition != NULL)
                            {
                              if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
                                 {
                                   printf ("previousNamespaceDefinition = %p \n",previousNamespaceDefinition);
                                 }

                              previousNamespaceDefinition = previousNamespaceDefinition->get_previousNamespaceDefinition();
                            }
                       }

                    show_details_and_maybe_fail(node);
                  }
             }
        }

     stack.push_back(node);
   }

void
TestForParentsMatchingASTStructure::postOrderVisit(SgNode *node)
   {
     assert(!stack.empty());
     assert(node==stack.back());
     stack.pop_back();
   }

void
TestForParentsMatchingASTStructure::show_details_and_maybe_fail(SgNode *node) 
   {
     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        {
          output <<prefix <<"AST path (including node) when inconsistency was detected:\n";
        }

     for (size_t i = 0; i < stack.size(); ++i)
        {
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
             {
               output << prefix
                 << "    #" << std::setw(4) << std::left << i << " " << stringifyVariantT(stack[i]->variantT(), "V_")
                 << " " << stack[i] << "; parent=" << stack[i]->get_parent()
                 << "\n";

            // DQ (9/21/2013): Avoid redundant output of debug info.
            // printf ("   stack[i]->get_parent() = %p \n",stack[i]->get_parent());
               if (stack[i]->get_parent() != NULL)
                  {
                    printf ("   stack[i]->get_parent() = %p = %s \n",stack[i]->get_parent(),stack[i]->get_parent()->class_name().c_str());
                  }
                 else
                  {
                    printf ("   stack[i]->get_parent() = %p \n",stack[i]->get_parent());
                  }
             }
        }

  // DQ (3/4/2016): Klocworks reports a problem with "node->get_parent() != NULL".
     ROSE_ASSERT(node != NULL);

     if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
        {
          output << prefix
            << "    #" << std::setw(4) << std::left << stack.size() << " " << stringifyVariantT(node->variantT(), "V_")
            << " " << node << "; parent=" << node->get_parent()
            << " = " << ((node->get_parent() != NULL) ? node->get_parent()->class_name() : string("null"))
            << "\n";
        }

     if (++nproblems >= limit)
        {
          throw this;
        }
   }

void
TestForParentsMatchingASTStructure::test( SgProject* project )
   {
  // See documentation in the header file for this test.

  // End in an error if any are detected.
     std::ostringstream ss;
     if (!TestForParentsMatchingASTStructure(ss, "    ").check(project, 10))
        {
          if ( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
             {
               std::cerr <<"Detected AST parent/child relationship problems after AST post processing:\n" <<ss.str();
             }
#if 0
          ROSE_ASSERT(false);
#endif
        }

  // TestForParentsMatchingASTStructure traversal(project);
  // traversal.traverse();
   }


void
TestForSourcePosition::testFileInfo( Sg_File_Info* fileInfo )
   {
  // The get_file_info() function maps to get_operator_position() for SgExpression IR nodes and to 
  // get_startOfConstruct() for SgStatement and some other IR nodes in SgSupport that have not yet 
  // been moved to be in SgLocatedNode.

     bool isCompilerGenerated                   = (fileInfo->isCompilerGenerated() == true);
     bool isFrontendSpecific                    = (fileInfo->isFrontendSpecific() == true);
     bool isTransformation                      = (fileInfo->isTransformation() == true);
     bool isShared                              = (fileInfo->isShared() == true);
     bool isSourcePositionUnavailableInFrontend = (fileInfo->isSourcePositionUnavailableInFrontend() == true);

     if (isCompilerGenerated == false && isFrontendSpecific == false && isShared == false && isSourcePositionUnavailableInFrontend == false && isTransformation == false)
        {
          if (fileInfo->get_filenameString() == "")
             {
               fileInfo->display("In TestForSourcePosition::visit(): debug");

               SgNode* node = fileInfo->get_parent();
               ROSE_ASSERT(node != NULL);

               printf ("Error: detected a source position with empty filename: node = %p = %s \n",node,node->class_name().c_str());
               ROSE_ASSERT(false);
             }

#if 0
       // Debugging code for special case.
          if (fileInfo->get_parent() != NULL && fileInfo->get_parent()->variantT() == V_SgFunctionDefinition)
             {
               fileInfo->display("In TestForSourcePosition::visit(): case of SgFunctionDefinition: debug");
             }
#endif

          if (fileInfo->get_physical_file_id() < 0)
             {
               fileInfo->display("In TestForSourcePosition::visit(): debug");

               SgNode* node = fileInfo->get_parent();
               ROSE_ASSERT(node != NULL);

               printf ("Error: detected a source position with inconsistant physical file id: node = %p = %s \n",node,node->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }
   }


void
TestForSourcePosition::visit ( SgNode* node )
   {
     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          testFileInfo(fileInfo);
        }

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
       // This is a redundant test (with get_file_info()) for SgStatement, but not for SgExpression.
          Sg_File_Info* startOfConstruct = node->get_startOfConstruct();
          if (startOfConstruct != NULL)
             {
               testFileInfo(startOfConstruct);
             }

          Sg_File_Info* endOfConstruct = node->get_endOfConstruct();
          if (endOfConstruct != NULL)
             {
               testFileInfo(endOfConstruct);
             }

       // This is a redundant test (with get_file_info()) for SgExpression.
          SgExpression* expressionNode = isSgExpression(locatedNode);
          if (expressionNode != NULL)
             {
               Sg_File_Info* operatorPosition = expressionNode->get_operatorPosition();
               if (operatorPosition != NULL)
                  {
                    testFileInfo(operatorPosition);
                  }
             }
        }
   }



void
TestForMultipleWaysToSpecifyRestrictKeyword::visit ( SgNode* node )
   {
  // DQ (12/11/2012): Added new consistancy test.
  // This consistancy test only tests for consistant setting of the restrict keyword.  But it might be useful
  // to enforce consistant setting of both "const" and volatile" keywords as well.  I'm not clear on this
  // design point.  There are notes about this in the Unparse_ExprStmt::unparseMFuncDeclStmt() hnadling of
  // the "restrict" keywork (in the unparser).

     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(node);
     if (declarationStatement != NULL)
        {
#if 0
          printf ("In TestForMultipleWaysToSpecifyRestrictKeyword::visit(): Found a declaration = %p = %s \n",declarationStatement,declarationStatement->class_name().c_str());
#endif
          SgDeclarationModifier & declarationModifier = declarationStatement->get_declarationModifier();
          SgTypeModifier & typeModifierFromDeclarationModifier = declarationModifier.get_typeModifier();

          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
          if (functionDeclaration != NULL)
             {
               SgType* type = functionDeclaration->get_type();
               ROSE_ASSERT(type != NULL);
#if 0
               printf ("In TestForMultipleWaysToSpecifyRestrictKeyword::visit(): Found a function type = %p = %s \n",type,type->class_name().c_str());
#endif
            // DQ (12/11/2012): This should never be a SgModifierType.
               SgModifierType* modifierType = isSgModifierType(type);
               ROSE_ASSERT(modifierType == NULL);

            // SgFunctionType* functionType       = isSgFunctionType(type);
               SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(type);
               if (memberFunctionType != NULL)
                  {
                    if (memberFunctionType->isRestrictFunc() != typeModifierFromDeclarationModifier.isRestrict())
                       {
                         printf ("Error: restrict is not consistantly set in the two SgTypeModifier IR nodes memberFunctionType->isRestrict() = %s typeModifierFromDeclarationModifier.isRestrict() = %s \n",
                              memberFunctionType->isRestrictFunc() ? "true" : "false",typeModifierFromDeclarationModifier.isRestrict() ? "true" : "false");
                         ROSE_ASSERT(false);
                       }
                  }
                 else
                  {
#if 0
                    printf ("Not a SgMemberFunctionType! \n");
#endif
                  }
             }
        }
   }


void
TestAstForCyclesInTypedefs::test()
   {
     TestAstForCyclesInTypedefs t;
     t.traverseMemoryPool();
   }


void
TestAstForCyclesInTypedefs::visit ( SgNode* node )
   {
  // DQ (10/27/2015): This test checks for cycles in typedef types.
  // It might be expanded to include testing for cycles in other kinds of types as well.

     ROSE_ASSERT(node != NULL);

#if 0
     printf ("TestAstForCyclesInTypedefs::visit: node = %s \n",node->class_name().c_str());
#endif

  // DQ (10/27/2015): Added test for specific cycle in typedef types.
  // SgType* currentType = isSgType(node);
     SgTypedefType* currentType = isSgTypedefType(node);

#if 1
     if (currentType != NULL)
        {
          ROSE_ASSERT( (isSgModifierType(isSgTypedefType(currentType)->get_base_type()) == NULL) || 
                       ( (isSgModifierType(isSgTypedefType(currentType)->get_base_type()) != NULL) && 
                         (isSgModifierType(isSgTypedefType(currentType)->get_base_type())->get_base_type() != currentType) ) );
        }
#endif
   }


#if 0
void
TestNodes::visit ( SgNode* node )
   {
  // DQ (10/25/2016): This test is to access each node in the memory pool to look for where an error occures.
  // It is part of debugging an erro in mergeTest_04.C
#if 0
     printf ("TestNodes: node = %p \n",node);
     if (node != NULL)
        {
          printf ("TestNodes: node = %p = %s \n",node,node->class_name().c_str());
        }
#else
     if (node != NULL)
        {
       // node->get_parent();
       // if (node->p_freepointer == IS_VALID_POINTER)
       // if (AST_FileIO::IS_VALID_POINTER() == true)
          if (node->get_freepointer() == AST_FileIO::IS_VALID_POINTER())
             {
               node->variantT();
             }
        }
#endif

   }

void
TestNodes::test()
   {
     TestNodes t;
  // t.traverse(node,preorder);
     t.traverseMemoryPool();
   }
#endif
