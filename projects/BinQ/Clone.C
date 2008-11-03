#include "BinQGui.h"
#include "BinQSupport.h"
#include "Clone.h"
using namespace boost;
using namespace std;
using namespace qrs;

std::string
unparseInstrFast(SgAsmInstruction* iA)
{
  static std::map<SgAsmInstruction*,std::string> strMap = std::map<SgAsmInstruction*,std::string>();
  std::map<SgAsmInstruction*,std::string>::iterator iItr =
    strMap.find(iA);
  std::string value = "";
  if(iItr == strMap.end() )
    {
      value = unparseInstruction(iA);
      strMap[iA] = value;
    }else
    value = iItr->second;
  return value;
};


static bool
isEqual(SgNode* A, SgNode* B)
{
  if(A==NULL || B == NULL) return false;
  SgAsmInstruction* iA = isSgAsmx86Instruction(A);
  SgAsmInstruction* iB = isSgAsmx86Instruction(B);
  SgAsmFunctionDeclaration* fA = isSgAsmFunctionDeclaration(A);
  SgAsmFunctionDeclaration* fB = isSgAsmFunctionDeclaration(B);

  bool isTheSame = false;
  if(iA != NULL && iB != NULL)
    isTheSame = unparseInstrFast(iA) == unparseInstrFast(iB) ? true : false;
  if(fA != NULL && fB != NULL)
    isTheSame = fA->get_name() == fB->get_name() ? true : false;

  return isTheSame;
}


void LCSLength( scoped_array<scoped_array<size_t> >& C  ,vector_start_at_one<SgNode*>& A, vector_start_at_one<SgNode*>& B )
{
  int m = A.size()+1;
  int n = B.size()+1;
  C.reset(new scoped_array<size_t>[m]);

  for (int i = 0 ; i < m; i++)
    C[i].reset(new size_t[n]);

  for (size_t i = 0 ; i <= A.size() ; i++)
    C[i][0]=0;
  for (size_t i = 0 ; i <= B.size() ; i++)
    C[0][i]=0;

  for (size_t i = 1 ; i <= A.size() ; i++)
    for (size_t j = 1 ; j <= B.size() ; j++)
      {
	if(isEqual(A[i],B[j]))
	  C[i][j] = C[i-1][j-1]+1;
	else
	  C[i][j] = C[i][j-1] > C[i-1][j] ? C[i][j-1] : C[i-1][j];

      }

}


void printDiff( scoped_array<scoped_array<size_t> >& C,
		vector_start_at_one<SgNode*>& A, vector_start_at_one<SgNode*>& B, int i, int j,
		std::vector<pair<int,int> >& addInstr, std::vector<pair<int,int> >& minusInstr
		)
{
  if(i> 0 && j > 0 && isEqual(A[i],B[j]))
    {
      printDiff(C,A,B,i-1,j-1,addInstr, minusInstr);
      //print " " + X[i]
    }else if( j > 0 && (i == 0 || C[i][j-1] >= C[i-1][j]))
    {
      printDiff(C,A,B,i,j-1,addInstr, minusInstr);
      //print "+ " + B[j]
      std::cout << "+ " << j << " " << unparseInstrFast( (SgAsmInstruction*) B[j]) <<std::endl;
      addInstr.push_back(pair<int,int>(i,j));
    }else  if(i > 0 && (j == 0 || C[i][j-1] < C[i-1][j]))
    {
      printDiff(C, A, B, i-1, j,addInstr, minusInstr);
      //   print "- " + X[i]
      std::cout << "- " << i << " " << unparseInstrFast((SgAsmInstruction*)A[i]) << std::endl;
      minusInstr.push_back(pair<int,int>(i,j));
    }
}



void
andreasDiff() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
    // this part is to find the added and removed code (from Andreas)
    FindInstructionsVisitor vis;
    scoped_array<scoped_array<size_t> > C;
    vector_start_at_one<SgNode*> insnsA;
    AstQueryNamespace::querySubTree(instance->fileA, std::bind2nd( vis, &insnsA ));
    vector_start_at_one<SgNode*> insnsB;
    AstQueryNamespace::querySubTree(instance->fileB, std::bind2nd( vis, &insnsB ));

    LCSLength(C,insnsA,insnsB);
    std::vector<pair<int,int> > addInstr,minusInst;
    printDiff(C,insnsA, insnsB,insnsA.size(),insnsB.size(),addInstr,minusInst);
    //    cerr << " found adds on left side : " << addInstr.size() << endl;
    //cerr << " found subbs on left side : " << minusInst.size() << endl;
    
    QString res = QString("Found adds:  %1.  Found subbs: %2. ")
      .arg(addInstr.size())
      .arg(minusInst.size());
    instance->analysisResult->append(res);  


    for (unsigned int k=0;k<addInstr.size();++k) {
      std::pair<int,int> p = addInstr[k];
      int a = p.first;
      int b = p.second;
      SgAsmInstruction* instA = isSgAsmInstruction(insnsA[a]);
      SgAsmInstruction* instB = isSgAsmInstruction(insnsB[b]);
#if 0
      cerr << i << " Found ADD in A  (a:" << a <<",b:"<<b<<") : " << endl << 
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
	  instance->itemsFileA[i]->bg=QColor(135,206,255);
	  for (int j=1;j<instance->maxrows;j++)
	    instance->codeTableWidget->setBgColor(instance->itemsFileA[i]->bg,j,i);
	}
      }
      for(size_t i=0; i < instance->itemsFileB.size(); i++ )    {
	SgNode* stmts = instance->itemsFileB[i]->statement;
	SgAsmInstruction* inst = isSgAsmInstruction(stmts);
	if (inst && inst->get_address()==instB->get_address()) {
	  myPosB=instance->itemsFileB[i]->row;
	  instance->itemsFileB[i]->bg=QColor(135,206,255);
	  for (int j=1;j<instance->maxrows;j++)
	    instance->codeTableWidget2->setBgColor(instance->itemsFileB[i]->bg,j,i);
	}
      }

      QString res = QString("%1 Found ADD in A  (a:%2,b:%3) (a:%4,b:%5)  %6 %7   %8 %9")
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
