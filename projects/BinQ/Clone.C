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
DiffAlgo::run(SgNode* fileA, SgNode* fileB) {
    BinQGUI *instance = NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();
  // this part is to find the added and removed code (from Andreas)
  FindInstructionsVisitor vis;
   
  LCS::vector_start_at_one<SgNode*> insnsA;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( vis, &insnsA ));
  LCS::vector_start_at_one<SgNode*> insnsB;
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( vis, &insnsB ));
  
  std::vector<int> addInstr,minusInst;
  
  printDiff(insnsA, insnsB,addInstr,minusInst);
  
  //    cerr << " found adds on left side : " << addInstr.size() << endl;
  //cerr << " found subbs on left side : " << minusInst.size() << endl;
  
  if (!testFlag) {
    QString res = QString("Found adds:  %1.  Found subbs: %2. ")
      .arg(addInstr.size())
      .arg(minusInst.size());
    instance->analysisTab->setCurrentIndex(1);
    instance->analysisResult->append(res);  
    
    colorTable(instance, addInstr, minusInst, insnsA, insnsB);
  }
};

void
DiffAlgo::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;

};

void 
colorTable(BinQGUI* instance,  const std::vector<int >& addInstr,  const std::vector<int >&  minusInst,
           vector_start_at_one<SgNode*>& insnsA, vector_start_at_one<SgNode*>& insnsB 
    )
{
  if (instance==NULL)
    return;
  const char* results [] = {"PLUS", "MINUS"};
  std::vector<QColor> colors;
  colors.push_back( QColor(233,150,122)  );
  colors.push_back( QColor(135,206,255)  );

  for( unsigned int choice = 0; choice < sizeof(results)/sizeof(char*) ; choice++ )  {
    std::string currentName(results[choice]);
    const std::vector< int >& currentResults = ( choice == 0 ? addInstr : minusInst ); 
    QColor& color = colors[choice];

    for (unsigned int k=0;k<currentResults.size();++k) {
      int insnNr = currentResults[k];
      SgAsmInstruction* instA = choice == 1 ? isSgAsmInstruction(insnsA[insnNr]) : NULL;
      SgAsmInstruction* instB = choice == 0 ? isSgAsmInstruction(insnsB[insnNr]) :NULL;
      
#if 0
      cerr << choice << " Found " << currentName << " in A  (a:" << a <<",b:"<<b<<") : " << endl << 
                               "     " << RoseBin_support::HexToString(instA->get_address()) << "  " <<
                               instA->get_mnemonic() <<endl <<
                               "     " << RoseBin_support::HexToString(instB->get_address()) << "  " <<
                               instB->get_mnemonic() <<endl;
#endif

      int myPosA=0;
      int myPosB=0;
      if(choice == 1)    {
        for(size_t i=0; i < instance->itemsFileA.size(); i++ )    {
          SgAsmStatement* stmts = isSgAsmStatement(instance->itemsFileA[i]->statement);
          //	ROSE_ASSERT(stmts);
          SgAsmInstruction* inst = isSgAsmInstruction(stmts);
          if (inst && inst->get_address()==instA->get_address()) {
            myPosA=instance->itemsFileA[i]->row;
            //  instance->itemsFileA[i]->plus=true;
            instance->itemsFileA[i]->bg=color;
#if 0
	    SgNode* node = instance->itemsFileA[i]->statement;
	    ROSE_ASSERT(node);
	    rose_addr_t rt = instance->itemsFileA[i]->addr;
	    ROSE_ASSERT(rt==instA->get_address());
	    // delete
            instance->itemsFileA[i]->bg=QColor(255,0,0);
	    QColor col = instance->itemsFileA[i]->bg;
	    string cols = col.name().toStdString();
	    //	    instance->codeTableWidget->setBgColor(instance->itemsFileA[i]->bg,0,i);
	    cerr << "Node " << node->class_name() << "  address: " <<
	      RoseBin_support::HexToString(instA->get_address()) << "  color : " << cols << endl;
#endif
            for (int j=1;j<instance->maxrows;j++) {
              instance->codeTableWidget->setBgColor(instance->itemsFileA[i]->bg,j,i);
	    }
          }
        }
      } else
        for(size_t i=0; i < instance->itemsFileB.size(); i++ )    {
          SgNode* stmts = instance->itemsFileB[i]->statement;
          SgAsmInstruction* inst = isSgAsmInstruction(stmts);
          if (inst && inst->get_address()==instB->get_address()) {
            myPosB=instance->itemsFileB[i]->row;
            instance->itemsFileB[i]->bg=color;
            for (int j=1;j<instance->maxrows;j++)
              instance->codeTableWidget2->setBgColor(instance->itemsFileB[i]->bg,j,i);
          }
        }

      std::string resultsString ="%1 Found " + currentName + " in A  (a:%2,b:%3) (a:%4,b:%5)  %6 %7   ";
      QString res = QString( resultsString.c_str())
        .arg(k)
        .arg(insnNr)
        .arg(insnNr)
        .arg(myPosA)
        .arg(myPosB)
        .arg(QString(RoseBin_support::HexToString(choice == 1 ? instA->get_address() : instB->get_address()).c_str()))
        .arg(QString( choice == 1 ? instA->get_mnemonic().c_str() : instB->get_mnemonic().c_str()));

      instance->analysisResult->append(res);  

    }

  }
  //  if (instance)
  //  instance->updateByteItemList();
}
