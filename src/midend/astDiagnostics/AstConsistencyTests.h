// Author: Markus Schordan
// $Id: AstConsistencyTests.h,v 1.8 2008/01/25 02:25:46 dquinlan Exp $

#ifndef ASTTESTS_H
#define ASTTESTS_H

// DQ (12/7/2003): use platform independent macro defined in config.h
// #include IOSTREAM_HEADER_FILE
#include <iostream>

#include "rosedll.h"
#include "AstStatistics.h"
// #include "AstPDFGeneration.h"
#include "AstTextAttributesHandling.h"

#include "AstNodePtrs.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "AstReverseProcessing.h"
#endif

class TestAstPropertiesSA 
   {
     public:
          TestAstPropertiesSA():val(true),node(NULL) {}
          bool val;
          SgNode* node;
   };

class TestAstProperties : public AstBottomUpProcessing<TestAstPropertiesSA>
   {
 //! this function returns true if all nodes are correct (not problematic nodes)
 //! this function returns false if at least one node is a problematic node

     private:
       // DQ (8/3/2004): This list is used to record and report any IR nodes that have missing file
       // info the file info object is present, but the information is it is default information 
       // (e.g. filename = NULL_FILE, line = 0, column = 0)
          std::list<SgNode*> listOfNodesWithoutValidFileInfo;
          std::list<SgNode*> listOfNodesFileInfo;

     public:
       // DQ (10/24/2004): Track count of nodes without proper ending file info object
          int nodeWithoutFileInfoFrequencyCount[V_SgNumVariants];

          TestAstProperties();

     private:
          TestAstPropertiesSA evaluateSynthesizedAttribute(SgNode* node, SubTreeSynthesizedAttributes l);
   };

class ROSE_DLL_API AstTests 
   {
     public:
       // some handy string functions used when testing the RTI information of a SgNode
          static bool isPrefix(std::string prefix, std::string s);
          static unsigned int numPrefix(std::string prefix, std::vector<std::string> vs);

       // properties of SgNodes
          static unsigned int numSuccContainers(SgNode* node);
          static unsigned int numSingleSuccs(SgNode* node);
          static bool isProblematic(SgNode* node);

       //! Test codes that traverse the AST
          static void runAllTests(SgProject* sageProject);
          static bool isCorrectAst(SgProject* sageProject);
   };

#ifndef SWIG
// DQ (3/10/2013): Swig has a problem with this class (remove from visability to swig).

class TestAstNullPointers : public AstNodePtrs {
 public:
  TestAstNullPointers() {}
  virtual void visitWithAstNodePointersList(SgNode* node, AstNodePointersList l);
};
#endif

  // these dummy classes are just to show the interfaces of the 4 kinds of traversals
  // void dummyTests(SgProject* sageProject); // traverse AST with all 4 kinds of traversals
  class DI : AstInheritedAttribute {}; // Dummy inherited attribute
  class DS : AstSynthesizedAttribute {}; // Dummy synthesized attribute
  class DummyISTestQuery1 : public AstTopDownBottomUpProcessing<DI,DS> {
    DI evaluateInheritedAttribute(SgNode* n, DI inh) { return inh; }
    DS evaluateSynthesizedAttribute(SgNode*, DI inh, SubTreeSynthesizedAttributes st) { DS syn; return syn; }
  };
  class DummyITestQuery1 : public AstTopDownProcessing<DI> {
    DI evaluateInheritedAttribute(SgNode* n, DI inh) { return inh; }
  };
  class DummySTestQuery1 : public AstBottomUpProcessing<DS> {
    DS evaluateSynthesizedAttribute(SgNode* n, SubTreeSynthesizedAttributes st) { DS syn; return syn; }
  };
  class DummyTestQuery1 : public AstSimpleProcessing {
    void visit(SgNode* n) {}
  };

// #ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
  class DummyISTestQuery2 : public AstReversePrefixInhSynProcessing<DI,DS> {
    DI evaluateInheritedAttribute(SgNode* n, DI inh) { return inh; }
    DS evaluateSynthesizedAttribute(SgNode*, DI inh, SubTreeSynthesizedAttributes st) { DS syn; return syn; }
  };

  class DummyITestQuery2 : public AstReversePrefixInhProcessing<DI> {
    DI evaluateInheritedAttribute(SgNode* n, DI inh) { return inh; }
  };

  class DummySTestQuery2 : public AstReversePrefixSynProcessing<DS> {
    DS evaluateSynthesizedAttribute(SgNode* n, SubTreeSynthesizedAttributes st) { DS syn; return syn; }
  };
  class DummyTestQuery2 : public AstReversePrefixSimpleProcessing {
    void visit(SgNode* n) {}
  };

  class DummyISTestQuery3 : public AstReverseBranchInhSynProcessing<DI,DS> {
    DI evaluateInheritedAttribute(SgNode* n, DI inh) { return inh; }
    DS evaluateSynthesizedAttribute(SgNode*, DI inh, SubTreeSynthesizedAttributes st) { DS syn; return syn; }
  };
  class DummyITestQuery3 : public AstReverseBranchInhProcessing<DI> {
    DI evaluateInheritedAttribute(SgNode* n, DI inh) { return inh; }
  };
  class DummySTestQuery3 : public AstReverseBranchSynProcessing<DS> {
    DS evaluateSynthesizedAttribute(SgNode* n, SubTreeSynthesizedAttributes st) { DS syn; return syn; }
  };
  class DummyTestQuery3 : public AstReverseBranchSimpleProcessing {
    void visit(SgNode* n) {}
  };
// #endif

// DQ (3/30/2004): Added tests on templates!
// class TestAstTemplateProperties : public AstPreOrderTraversal
class TestAstTemplateProperties : public AstSimpleProcessing
   {
  // This class uses a traversal to test properties of template 
  // specific IR nodes in the AST.
     public:
          void visit ( SgNode* node );
   };

class TestAstForUniqueStatementsInScopes : public AstSimpleProcessing
   {
  // This class uses a traversal to test properties of AST.
  // We look for redundent entries in any single scope (not 
  // redundent entries in the AST!).
     public:
          void visit ( SgNode* node );
   };

// DQ (4/2/2012): This appears to be a test that we have not got into palce yet.
// the test code: test2012_59.C demonstrates an example where a IR node is shared
// between the global scope and a class definition scope.  This causes an error 
// in the generated code, so we want to detect this case.
class TestAstForUniqueNodesInAST : public AstSimpleProcessing
   {
  // This class uses a traversal to test properties of AST.
  // We look for redundent entries anywhere in the AST.
  // This test has to save a pointer to ever AST IR node 
  // that is traversed so it is a bit expensive in memory.

     std::set<SgNode*> astNodeSet;

     public:
          void visit ( SgNode* node );

          static void test ( SgNode* node );
   };

// DQ (4/3/2012): Simple globally visible function to call (used for debugging elsewhee in ROSE).
void testAstForUniqueNodes ( SgNode* node );


class TestAstCompilerGeneratedNodes : public AstSimpleProcessing
   {
  // This class uses a traversal to test properties of compiler generated IR nodes.

     public:
          void visit ( SgNode* node );
   };

class TestAstForProperlyMangledNames : public AstSimpleProcessing
   {
  // This class uses a traversal to test properties of namged names.

     public:
          unsigned long saved_maxMangledNameSize;
          unsigned long saved_totalMangledNameSize;
          unsigned long saved_numberOfMangledNames;
          void visit ( SgNode* node );

       // DQ (8/28/2006): Added constructor to permit data members to be set properly
          TestAstForProperlyMangledNames();

       // DQ (2/7/2006): This is Rich's function to simplify the testing 
       // (we make it static so that it can be easily called from elsewhere).
          static bool isValidMangledName (std::string name, bool java_lang = false);
   };

class TestAstForProperlySetDefiningAndNondefiningDeclarations : public AstSimpleProcessing
   {
  // This class uses a traversal to test the values of the definingDeclaration and
  // firstNondefiningDeclaration pointers in each SgDeclarationStatement.  See code for
  // details, since both of these pointers are not always set.

     public:
          void visit ( SgNode* node );
   };

class TestAstSymbolTables : public AstSimpleProcessing
   {
  // This class uses a traversal to test properties of symbol tables 
  // (global function type symbol table and local symbol tables in each scope).

     public:
          void visit ( SgNode* node );
   };

class TestAstAccessToDeclarations : public AstSimpleProcessing
   {
  // This class uses a traversal to test the get_declaration() member function on each 
  // IR node where relavant.  The goal is to verify that each member function, were
  // appropriate, returns a valid declaration.

     public:

      //! static function to do test on any IR node
          static void test(SgNode* node);

      //! visit function required for traversal
          void visit ( SgNode* node );
   };

class TestExpressionTypes : public AstSimpleProcessing
   {
  // This class uses a traversal to test the get_type() member function on each 
  // SgExpression IR node. The goal is to verify that each member function, were
  // appropriate, returns a valid type.

     public:

      //! static function to do test on any IR node
       // static void test(SgNode* node);

      //! visit function required for traversal
          void visit ( SgNode* node );
   };

class TestLValues : public AstSimpleProcessing
{
        // This class uses a traversal to test the isLValue() and isDefinable() member functions on each 
        // SgExpression IR node. The goal is to verify that each member function, were
        // appropriate, returns whether or not its argument is an lvalue or definable.

public:
        //! static function to do test on any IR node
        // static void test(SgNode* node);

        //! visit function required for traversal
        void visit ( SgNode* node );
};

// class TestMangledNames : public AstSimpleProcessing
class TestMangledNames : public ROSE_VisitTraversal
   {
  // This class uses a traversal to test the generation of mangled names.

     public:
     virtual ~TestMangledNames() {};
      //! static function to do test on any IR node
          static void test();

          unsigned long saved_maxMangledNameSize;
          unsigned long saved_totalMangledNameSize;
          unsigned long saved_numberOfMangledNames;
          unsigned long totalLongMangledNameSize;
          unsigned long totalNumberOfLongMangledNames;

       // DQ (8/28/2006): Added constructor to permit data members to be set properly
          TestMangledNames();

      //! visit function required for traversal
          void visit ( SgNode* node );
   };

#if 0
class TestParentPointersOfSymbols : public ROSE_VisitTraversal
   {
  // This class uses a traversal to test the parent pointers of symbols.

     public:

      //! static function to do test on any IR node
          static void test();

      //! visit function required for traversal
          void visit ( SgNode* node );
   };
#endif
#if 0
void testParentPointersOfSymbols();
#endif

// DQ (6/26/2006): Added test of parents of IR nodes using memory pool!
/*! \brief This traversal calles ResetParentPointersInMemoryPool Memory Pool traversal.
 */
// void testParentPointersInMemoryPool();

/*! \brief This traversal uses the Memory Pool traversal to test parent pointers.

    This traversal uses the Memory Pool traversal to test parent pointers 
    (e.g. declarations that are hidden from the AST traversal). This traversal traverses the 
    whole AST using the memory pool traversal.
 */
class TestParentPointersInMemoryPool : public ROSE_VisitTraversal
   {
     public:
          virtual ~TestParentPointersInMemoryPool() {};
      //! static function to do test on any IR node
          static void test();

      //! Required traversal function
          void visit (SgNode* node);
   };


/*! \brief This traversal uses the Memory Pool traversal to test parent pointers.

    This traversal uses the Memory Pool traversal to test parent pointers 
    (e.g. declarations that are hidden from the AST traversal). This traversal traverses the 
    whole AST using the memory pool traversal.
 */
class TestChildPointersInMemoryPool : public ROSE_VisitTraversal 
   {
     public:
          virtual ~TestChildPointersInMemoryPool() {};
      //! static function to do test on any IR node
          static void test();

          virtual void visit( SgNode * );
   };


/*! \brief This tests the first non-defining, referenced from each declaration, to make sure that 
    it is not a forward declaration (a non-defining declaration marked to be a forward declaration).

    \internal This is an issue specific to name qualification.  For functions, there are different 
    types of non-defining declarations.  Those marked forward are represented in the AST traversal 
    (the AST as defined by the traversal) and cannot be shared, while those marked as non-forward 
    are sharable non-defining declarations.  Even the forward non-defining declarations are not all
    the same (non created equal) since those appearing in scopes where the associated defining 
    declaration could be placed are \bstronger and force name qualification to be used subsequently
    (those appearing before this stronger sort fo forward declaration are not allowed to have
    name qualification.  This behavior is most dramatic for functions, but applied in what seems to
    be a slightly reduced form to classes and structs.  This needs to be tested!
 */
class TestFirstNondefiningDeclarationsForForwardMarking : public ROSE_VisitTraversal
   {
     public:
     virtual ~TestFirstNondefiningDeclarationsForForwardMarking() {};
      //! static function to do test on any IR node
          static void test();

          virtual void visit( SgNode * );
   };



/*! \brief This traversal uses the Memory Pool traversal to test declarations and if their associated symbol can be located.

    This traversal uses the Memory Pool traversal to test if sysbols can be found for all declarations.
 */
class TestMappingOfDeclarationsInMemoryPoolToSymbols : public ROSE_VisitTraversal 
   {
     public:
     virtual ~TestMappingOfDeclarationsInMemoryPoolToSymbols() {};
      //! static function to do test on any IR node
          static void test();

          virtual void visit( SgNode * );
   };

class TestLValueExpressions : public AstSimpleProcessing
   {
  // This class uses a traversal to test expressions that should be marked 
  // as lvalues and makes sure that other expressions are not marked as lvalues.

     public:

      //! static function to do test on any IR node
      //  static void test(SgNode* node);

      //! visit function required for traversal
          void visit ( SgNode* node );
   };

class TestMultiFileConsistancy : public ROSE_VisitTraversal // AstSimpleProcessing
   {
  // DQ (3/7/2010): Corrected the documentation for this class.
  // Test the declarations to make sure that defining and non-defining appear in 
  // the same file (for outlining consistency).

     public:

      //! static function to do test on any IR node
          static void test();

      //! visit function required for traversal
          void visit ( SgNode* node );
   };


class BuildListOfConnectedNodesInAST : public AstSimpleProcessing
   {
  // DQ (3/7/2010): This class is part of a test to detect disconnected 
  // parts of the AST. These are currently a problem for the AST File I/O 
  // and need to be eliminated.

     public:
         std::set<SgNode*> & nodeSet;
         BuildListOfConnectedNodesInAST(std::set<SgNode*> & s);

      //! visit function required for traversal
          void visit ( SgNode* node );
   };

class BuildListOfNodesInAST : public ROSE_VisitTraversal
   {
  // DQ (3/7/2010): This class Corrected the documentation for this class.
  // Test the declarations to make sure that defining and non-defining appear in 
  // the same file (for outlining consistency).

     public:
         const std::set<SgNode*> & constNodeSet;
         std::set<SgNode*> & nodeSet;
         BuildListOfNodesInAST(const std::set<SgNode*> & s1,std::set<SgNode*> & s2);

      //! visit function required for traversal
          void visit ( SgNode* node );
   };

class TestForDisconnectedAST 
   {
  // DQ (3/7/2010): This uses  the results form the BuildListOfConnectedNodesInAST
  // and BuildListOfNodesInAST and identifies the differences.

     public:
      //! static function to do trigger the test
          static void test(SgNode* node);
   };


class MemoryCheckingTraversalForAstFileIO : public ROSE_VisitTraversal
   {
     public:
          int counter;
          void visit ( SgNode* node );
   };





class TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute : AstInheritedAttribute
   {
     public:
       // Use the source file as a way to report better quality errors.
          SgSourceFile* sourceFile;

       // This will be set as we encounter the SgSourceFile at the top of the AST within the traversal.
          bool caseInsensitive;

       // Required constructor.
          TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute(bool b);

       // Required copy constructor.
          TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute(const TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute & X);
   };

class TestForProperLanguageAndSymbolTableCaseSensitivity : public AstTopDownProcessing<TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute> 
   {
  // DQ (11/28/2010): This class is part of a test to verify consistancy of 
  // symbol table case sensitivity with languge.  C/C++ codes should use only 
  // case sensitive symbol tables, while Fortran codes should use only case 
  // insensitive symbol table handling.

     public:
       // Overloaded pure virtual function.
          TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute evaluateInheritedAttribute(SgNode* node, TestForProperLanguageAndSymbolTableCaseSensitivity_InheritedAttribute inheritedAttribute);

       // Simple funtion to call to get the traversal started (sets up the inherited attribute, etc.).
          static void test(SgNode* node);
   };



class TestForReferencesToDeletedNodes : public ROSE_VisitTraversal
   {
  // DQ (9/26/2011): This class is part of a test to verify consistancy of 
  // the AST by verifying that no IR node referenced in the AST is a SgNode.
  // Note that in a proper AST all IR nodes are derived from an SgNode, but
  // that there should be no IR nodes in the AST that are only SgNode types.
  // It can happen that a referenced IR nodes will only be able to report 
  // that it is a SgNode, when this happens it is generally because it is 
  // and dangling pointer to an IR node that was deleted (at which point
  // it can no longer be identified by type to be more than an SgNode).
  // So this is a test for references in the AST to IR nodes that have been
  // deleted.

     private:
         int detect_dangling_pointers;
         std::string filename;

     public:
          TestForReferencesToDeletedNodes(int input_detect_dangling_pointers, const std::string & s );

       // Overloaded pure virtual function.
          void visit( SgNode* node );

       // Simple funtion to call to get the traversal started...
          static void test( SgProject* project );
   };


class TestForParentsMatchingASTStructure: public AstPrePostProcessing
   {
  // DQ (3/19/2012): This is a test from Robb that I want to use uniformally in the AST.
  // This has been used to catch several locations in the AST where parents were not set
  // as they are defined to be set in the AST (based on a traversal).  So this test is
  // an important addition to the EDG 4.3 work to fix a number of the bugs in the EDG 3.3
  // work and define a cleaner representation of the AST.

  // Check that all nodes have the correct parent.  This code is not thread safe. 

     public:
          std::vector<SgNode*> stack;                 // current path within the AST
#ifndef USE_ROSE
       // DQ (3/6/2013): Disable code that is a problem for SWIG (vesion 2.0.9).
       // This data member is a problem for SWIG, so ignore it when
       // processing using USE_ROSE which we define when using SWIG.
          std::ostream &output;                       // where to emit warning/error messages
#endif
          size_t nproblems;                           // number of problems detected
          size_t limit;                               // number of errors to allow before exit
          std::string prefix;                         // line prefix

     public:
          explicit TestForParentsMatchingASTStructure(std::ostream &output, const std::string & prefix = "");
          bool check(SgNode *ast, size_t limit = 0);
          void preOrderVisit(SgNode *node);
          void postOrderVisit(SgNode *node);
          void show_details_and_maybe_fail(SgNode *node);

       // Simple funtion to call to get the traversal started...
          static void test( SgProject* project );
   };


class TestForSourcePosition: public AstSimpleProcessing
   {
  // DQ (12/3/2012): This tests for Sg_File_Info objects that have an empty filename.
  // These have been set at an early stage in the defelopment of the edg4x work and 
  // now they are a problem.  so it is time to detect them and get rid of them.

     public:
          void testFileInfo( Sg_File_Info* fileInfo );

          void visit ( SgNode* node );
   };


class TestForMultipleWaysToSpecifyRestrictKeyword: public AstSimpleProcessing
   {
  // DQ (12/11/2012): This tests for the two different ways in which const-volitile-restrict 
  // modifiers can be specified.  It is a consiquence of the CV-modifier (SgTypeModifier) being 
  // a part of the type declaration modifier and also the SgModifierType and the SgDeclarationModifier.
  // Both are required and as a result it can be confusing that there are two locations to set
  // these.  Historically in the EDG 3.3 version we used the SgModifierType for C-V, but the 
  // SgDeclarationModifier for the restrict keyword.  In the edg 4.x version of ROSE, we now want 
  // to make this more uniform and use the SgModifierType everywhere. To address the inconsistancy, 
  // we want to check that both are always set consistanly.

     public:
          void visit ( SgNode* node );
   };


// DQ (10/27/2015): This test is part of debugging test2015_97.C which
// is a reduced version of the 600K line ROSEExample_test_01.C file.
// The issue is that we have previously generated a cycle in typedef types
// and so we want to detect these as errors.
class TestAstForCyclesInTypedefs : public ROSE_VisitTraversal
   {
  // This class uses a traversal to test properties of AST.
  // We look for redundent entries anywhere in the AST.
  // This test has to save a pointer to ever AST IR node 
  // that is traversed so it is a bit expensive in memory.

  // std::set<SgNode*> astNodeSet;

     public:
          virtual ~TestAstForCyclesInTypedefs() {};

       // Overloaded pure virtual function.
          void visit( SgNode* node );

       // Simple funtion to call to get the traversal started...
          static void test();
   };



#endif
