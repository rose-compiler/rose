#include "BinQGui.h"
#include "BinQSupport.h"
#include "LCS.h"
#include "FunctionDiff.h"
#include "Clone.h"

using namespace boost;
using namespace std;
using namespace qrs;
using namespace LCS;


std::string FunctionDiffAlgo::name() {
  return "FunctionBinaryDiff";
}



std::string 
FunctionDiffAlgo::getDescription() {
  return "This Analysis detects the diff between two binary files where the symbols matches. It marks the additions and removals between them.";
}



void
FunctionDiffAlgo::run(SgNode* fileA, SgNode* fileB) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  // this part is to find the added and removed code (from Andreas)
  FindInstructionsVisitor vis;

  vector<SgAsmFunctionDeclaration*> funcsA, funcsB;
  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcsA ));
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVis, &funcsB ));


  int adds=0, minus=0;
  
  for(unsigned int i = 0; i < funcsA.size() ; i++)
  {
    SgAsmFunctionDeclaration* functionA = funcsA[i];
    for(unsigned int j = 0; j < funcsB.size() ; j++)
    {

      SgAsmFunctionDeclaration* functionB = funcsB[j];

      if( functionA->get_name() != functionB->get_name() ) continue;
      
      LCS::vector_start_at_one<SgNode*> insnsA;
      AstQueryNamespace::querySubTree(functionA, std::bind2nd( vis, &insnsA ));
      LCS::vector_start_at_one<SgNode*> insnsB;
      AstQueryNamespace::querySubTree(functionB, std::bind2nd( vis, &insnsB ));

      std::vector<pair<int,int> > addInstr,minusInst;

      printDiff(insnsA, insnsB,addInstr,minusInst);

      //    cerr << " found adds on left side : " << addInstr.size() << endl;
      //cerr << " found subbs on left side : " << minusInst.size() << endl;

      colorTable(instance,addInstr, minusInst, insnsA, insnsB);

      adds+=addInstr.size();
      minus+=minusInst.size();

    }
  }
  
  QString res = QString("Found adds:  %1.  Found subbs: %2. ")
    .arg(adds)
    .arg(minus);
  instance->analysisTab->setCurrentIndex(1);
  instance->analysisResult->append(res);  
   
  
};


void
FunctionDiffAlgo::test(SgNode* fileA, SgNode* fileB) {
  // this part is to find the added and removed code (from Andreas)
  FindInstructionsVisitor vis;

  vector<SgAsmFunctionDeclaration*> funcsA, funcsB;
  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcsA ));
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVis, &funcsB ));


  int adds=0, minus=0;
  
  for(unsigned int i = 0; i < funcsA.size() ; i++)
  {
    SgAsmFunctionDeclaration* functionA = funcsA[i];
    for(unsigned int j = 0; j < funcsB.size() ; j++)
    {

      SgAsmFunctionDeclaration* functionB = funcsB[j];

      if( functionA->get_name() != functionB->get_name() ) continue;
      
      LCS::vector_start_at_one<SgNode*> insnsA;
      AstQueryNamespace::querySubTree(functionA, std::bind2nd( vis, &insnsA ));
      LCS::vector_start_at_one<SgNode*> insnsB;
      AstQueryNamespace::querySubTree(functionB, std::bind2nd( vis, &insnsB ));

      std::vector<pair<int,int> > addInstr,minusInst;

      printDiff(insnsA, insnsB,addInstr,minusInst);

      //    cerr << " found adds on left side : " << addInstr.size() << endl;
      //cerr << " found subbs on left side : " << minusInst.size() << endl;



      adds+=addInstr.size();
      minus+=minusInst.size();

    }
  }
   
  
};


