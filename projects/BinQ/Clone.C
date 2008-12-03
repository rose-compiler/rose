#include "BinQGui.h"
#include "BinQSupport.h"
#include "LCS.h"
#include "Clone.h"

using namespace boost;
using namespace std;
using namespace qrs;
using namespace LCS;


std::string DiffAlgo::name() {
  return "BinaryDiff";
}



std::string 
DiffAlgo::getDescription() {
  return "This Analysis detects the diff between two binary files. It marks the additions and removals between them.";
}


void
DiffAlgo::run() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
    // this part is to find the added and removed code (from Andreas)
    FindInstructionsVisitor vis;
   
    LCS::vector_start_at_one<SgNode*> insnsA;
    AstQueryNamespace::querySubTree(instance->fileA, std::bind2nd( vis, &insnsA ));
    LCS::vector_start_at_one<SgNode*> insnsB;
    AstQueryNamespace::querySubTree(instance->fileB, std::bind2nd( vis, &insnsB ));

    std::vector<pair<int,int> > addInstr,minusInst;

    printDiff(insnsA, insnsB,addInstr,minusInst);

    //    cerr << " found adds on left side : " << addInstr.size() << endl;
    //cerr << " found subbs on left side : " << minusInst.size() << endl;


    QString res = QString("Found adds:  %1.  Found subbs: %2. ")
      .arg(addInstr.size())
      .arg(minusInst.size());
    instance->analysisTab->setCurrentIndex(1);
    instance->analysisResult->append(res);  

    colorTable(instance, addInstr, minusInst, insnsA, insnsB);
};

void
DiffAlgo::test(SgNode* fileA, SgNode* fileB) {
    // this part is to find the added and removed code (from Andreas)
    FindInstructionsVisitor vis;
   
    LCS::vector_start_at_one<SgNode*> insnsA;
    AstQueryNamespace::querySubTree(fileA, std::bind2nd( vis, &insnsA ));
    LCS::vector_start_at_one<SgNode*> insnsB;
    AstQueryNamespace::querySubTree(fileB, std::bind2nd( vis, &insnsB ));

    std::vector<pair<int,int> > addInstr,minusInst;

    printDiff(insnsA, insnsB,addInstr,minusInst);

};

void 
colorTable(BinQGUI* instance,  const std::vector<pair<int,int> >& addInstr,  const std::vector<pair<int,int> >&  minusInst,
           vector_start_at_one<SgNode*>& insnsA, vector_start_at_one<SgNode*>& insnsB 
    )
{

  const char* results [] = {"PLUS", "MINUS"};
  std::vector<QColor> colors;
  colors.push_back( QColor(233,150,122)  );
  colors.push_back( QColor(135,206,255)  );

  for( int choice = 0; choice < sizeof(results)/sizeof(char*) ; choice++ )
  {

    std::string currentName(results[choice]);
    const std::vector< pair<int,int> >& currentResults = ( choice == 0 ? addInstr : minusInst ); 
    QColor& color = colors[choice];


    for (unsigned int k=0;k<currentResults.size();++k) {
      std::pair<int,int> p = currentResults[k];
      int a = p.first;
      int b = p.second;
      SgAsmInstruction* instA = isSgAsmInstruction(insnsA[a]);
      SgAsmInstruction* instB = isSgAsmInstruction(insnsB[b]);
#if 0
      cerr << i << " Found " << currentName << " in A  (a:" << a <<",b:"<<b<<") : " << endl << 
                                                                                       "     " << RoseBin_support::HexToString(instA->get_address()) << "  " <<
                                                                                         instA->get_mnemonic() <<endl <<
                                                                                         "     " << RoseBin_support::HexToString(instB->get_address()) << "  " <<
                                                                                         instB->get_mnemonic() <<endl;
#endif

      int myPosA=0;
      int myPosB=0;
      for(size_t i=0; i < instance->itemsFileA.size(); i++ )    {
        SgAsmStatement* stmts = isSgAsmStatement(instance->itemsFileA[i]->statement);
        //	ROSE_ASSERT(stmts);
        SgAsmInstruction* inst = isSgAsmInstruction(stmts);
        if (inst && inst->get_address()==instA->get_address()) {
          myPosA=instance->itemsFileA[i]->row;
          //  instance->itemsFileA[i]->plus=true;
          instance->itemsFileA[i]->bg=color;
          for (int j=1;j<instance->maxrows;j++)
            instance->codeTableWidget->setBgColor(instance->itemsFileA[i]->bg,j,i);
        }
      }
      for(size_t i=0; i < instance->itemsFileB.size(); i++ )    {
        SgNode* stmts = instance->itemsFileB[i]->statement;
        SgAsmInstruction* inst = isSgAsmInstruction(stmts);
        if (inst && inst->get_address()==instB->get_address()) {
          myPosB=instance->itemsFileB[i]->row;
          instance->itemsFileA[i]->bg=color;
          for (int j=1;j<instance->maxrows;j++)
            instance->codeTableWidget2->setBgColor(instance->itemsFileB[i]->bg,j,i);
        }
      }

      std::string resultsString ="%1 Found " + currentName + " in A  (a:%2,b:%3) (a:%4,b:%5)  %6 %7   %8 %9";
      QString res = QString( resultsString.c_str())
        .arg(k)
        .arg(a)
        .arg(b)
        .arg(myPosA)
        .arg(myPosB)
        .arg(QString(RoseBin_support::HexToString(instA->get_address()).c_str()))
        .arg(QString(instA->get_mnemonic().c_str()))
        .arg(QString(RoseBin_support::HexToString(instB->get_address()).c_str()))
        .arg(QString(instB->get_mnemonic().c_str()));
      instance->analysisResult->append(res);  

    }

  }


}
