// #include <map>
// #include <string>
#include "rose.h"
// #include "AstPDFGeneration.h"
// #include "AstStatistics.h"
// #include "AstTextAttributesHandling.h"
// #include "DOTGeneration.h"
// #include "AstDOTGeneration.h"
// #include "AstConsistencyTests.h"
// #include "AstFixes.h"
// #include "rewrite.h"
// #include "AstProcessing.h"
// #include "AgProcessing.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include "AstRestructure.h"
#include "BodyTransformation.h"
// #include "AstStatistics.h"
// #include "AstClearVisitFlags.h"

// Analaysis
#include "TestParallelLoop.h"

// NEW TRAVERSALS
#include "AstReverseProcessing.h"

extern AstRestructure* subst;
AstRestructure* subst;

extern TestParallelLoop* analysis;
TestParallelLoop* analysis;

class AstReversePrefixInhSynProcessingTest : public AstReversePrefixInhSynProcessing<bool,bool> {
  //virtual void setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer) {
  //  succContainer=node->get_traversalSuccessorContainer();
  // }
protected:
  bool evaluateInheritedAttribute(SgNode* astNode, bool inh) { 
    cout << "  INHX: RPrefixProcessing @ " << astNode->sage_class_name() << endl;
    return inh; 
  }
  bool evaluateSynthesizedAttribute(SgNode* astNode, bool iha, SynthesizedAttributesList l) {
    cout << "  SYNX:RPrefixProcessing @ " << astNode->sage_class_name() << ":";
    cout << " #= " << l.size() << endl;
    bool syn; return syn;
  }
};

class AstReverseSourceSequenceInhSynProcessingTest : public AstReverseBranchInhSynProcessing<bool,bool> {
  //virtual void setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer) {
  //  succContainer=node->get_traversalSuccessorContainer();
  // }
protected:
  bool evaluateInheritedAttribute(SgNode* astNode, bool inh) { 
    cout << "  INH:RBranchProcessing @ " << astNode->sage_class_name() << endl;
    return inh; 
  }
  bool evaluateSynthesizedAttribute(SgNode* astNode, bool iha, SynthesizedAttributesList l) {
    cout << "  SYN:RBranchProcessing @ " << astNode->sage_class_name() << ":";
    cout << " #= " << l.size() << endl;
    bool syn; return syn;
  }
};

// Test of above class;
class AstRevTestDriver : public AstSimpleProcessing {
public:
  AstRevTestDriver(SgProject* p):sageProject(p) { r=new AstRestructure(p); }
  ~AstRevTestDriver() { delete r; }
  AstRestructure* r;
private:
  SgProject* sageProject;
protected:
  virtual void visit(SgNode* node) {
    cout << "MAIN TRAVERSAL @ " << node->sage_class_name() << endl;

    /*
    {
    cout << "  RevPrefixProcessing started" << endl;
    AstReversePrefixInhSynProcessingTest rbp;
    bool inh;
    rbp.traverse(node,inh);
    cout << "  RevPrefixProcessing finished" << endl;
    }
    */

    /*
    {
    cout << "MAIN TRAVERSAL @ " << node->sage_class_name() << endl;
    cout << "  RevSSProcessing started" << endl;
    AstReverseSourceSequenceInhSynProcessingTest rssp;
    bool inh;
    rssp.traverse(node,inh);
    cout << "  RevSSProcessing finished" << endl;
    }
    */
    //r->lock(node);
    if(SgForStatement* nnode=dynamic_cast<SgForStatement*>(node)) {
      cout << "SgFor found." << endl;
    cout << "\nBodyTransformation started." << endl;
    BodyTransformation ag2;
    string iVarName="i", iContName="l";
    string s=ag2.transform(nnode,iVarName,iContName);
    cout << "\nBodyTransformation finished." << endl;      
      //r->replace(node,"for(;;){} ");
      
    }
    //r->unlock(node);
  }
};


// NEW ATTRIBUTE EVALUATION
#include "OmpLoopRATransformationParser.h"
//#include "BodyTransformationParser.h"

//////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
#if 0
class MeasureIA : public AST_Rewrite::InheritedAttribute {
public:
  MeasureIA(SgNode* n)
    : AST_Rewrite::InheritedAttribute(n) {}
  MeasureIA ( const MeasureIA& X, SgNode* astNode )
    : AST_Rewrite::InheritedAttribute (X,astNode) {};
};


class MeasureSA : public AST_Rewrite::SynthesizedAttribute {
public:
  MeasureSA() {}
  MeasureSA(SgNode* n):AST_Rewrite::SynthesizedAttribute (n) {}
};



class Measure : public AST_Rewrite::RewriteTreeTraversal<MeasureIA,MeasureSA> {
public:
  Measure(SgProject& project): AST_Rewrite::RewriteTreeTraversal<MeasureIA,MeasureSA>(project) {};

  ~Measure() {}

  MeasureIA
  evaluateRewriteInheritedAttribute(SgNode* astNode,
				    MeasureIA inheritedValue) {
    //return MeasureIA(inheritedValue,astNode);
    return inheritedValue;
  }

  MeasureSA 
  evaluateRewriteSynthesizedAttribute(SgNode* astNode,
				      MeasureIA ia,
		     SubTreeSynthesizedAttributes synList ) {
    cout << "TRANSSTART:" << endl;
    MeasureSA sa(astNode);
    for (SubTreeSynthesizedAttributes::iterator i = synList.begin(); i != synList.end(); i++) {
      sa+=*i;
    }

    /*
    if(dynamic_cast<SgExpression*>(astNode)) {
      cout << astNode->unparseToString() << endl;
    }
    */

      /*
    if(dynamic_cast<SgForStatement*>(astNode)) {
      string sourceFragment;
      sourceFragment=astNode->unparseToString();
      AST_Rewrite::AST_FragmentString sourceFragmentSpecifier(sourceFragment,ia,
							      AST_Rewrite::LocalScope,
							      AST_Rewrite::ReplaceCurrentPosition,
							      AST_Rewrite::TransformationString,
							      false);
      //sa.addSourceCodeString(sourceFragmentSpecifier);      
      cout << "\nREPLACED:" << sourceFragment << endl << endl;
    }
      */
    return sa;
  }
};
#endif 

class EnhancedCppToDotTranslator : public CppToCppTranslator {
  void midend();
};

void EnhancedCppToDotTranslator::midend() {

  /*
  // test general class 
  DOTGeneration<SgNode*> dotgen;
  //dotgen.generate("test", getAstRoot());

  // test ROSE specific class
  AstDOTGeneration astdotgen;
  astdotgen.generate("asttest", getAstRoot());
  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::PREORDER,"Preorder.");
  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::POSTORDER,"Postorder.");
  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::TOPDOWN,"TopDown.");
  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::BOTTOMUP,"BottomUp.");
  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::TOPDOWNBOTTOMUP,"TopDownBottomUp.");
  */

  //AstTextAttributesHandling textAttributes(getAstRoot());
  //textAttributes.setAll();
  //AstPDFGeneration pdfgen;
  //pdfgen.generate("test",getAstRoot());
  //pdfgen.generateInputFiles(getAstRoot());

  //textAttributes.removeAll();

  // FAILING REWRITE TESTS
  /*
  AstNodeStatistics stat;
  cout << stat.toString(getAstRoot());
  MeasureIA ia(getAstRoot());
  MeasureSA sa(getAstRoot());
  Measure m(*getAstRoot());
  m.traverseInputFiles(getAstRoot(),ia);
  */
  //AstTests::runAllTests(getAstRoot());

  //SgProject* projectNode=frontend(argc,argv);


  //subst=new AstRestructure(getAstRoot()); // global value (!)
  string preamble="#include \"/home/markus/ROSE/msDevelopment/UserDefList.h\"\n int __dummy;\n";
  subst=new AstRestructure(getAstRoot(),preamble); // global value (!)

  AgProcessing<OmpLoopRATransformationParser> ag;
  ag.evaluate(getAstRoot());

  delete subst;

  /*
  AstNodeStatistics x1;
  cout << x1.toString(getAstRoot());
  AstRevTestDriver rbtest(getAstRoot());
  rbtest.traverseInputFiles(getAstRoot(),preorder);
  AstNodeStatistics x2;
  cout << x2.toString(getAstRoot());
  */
}

int
main( int argc, char* argv[] ) {

  // Initialize Qing's analysis
  analysis= new TestParallelLoop(argc, argv);

  EnhancedCppToDotTranslator c;
  c.translate(argc,argv);
}

//#include "AstClearVisitFlags.C"
