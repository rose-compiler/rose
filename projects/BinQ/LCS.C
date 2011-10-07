#include "rose.h"
#include "LCS.h"
#include "BinQSupport.h"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace LCS;
#if 0
  std::string
LCS::unparseInstrFast(SgAsmInstruction* iA)
{
   std::map<SgAsmInstruction*,std::string> strMap;

  std::map<SgAsmInstruction*,std::string>::iterator iItr =
    strMap.find(iA);
  std::string value = "";
  if(iItr == strMap.end() )
  {

    // Unparse the normalized forms of the instructions
    string mne = iA->get_mnemonic();
    boost::to_lower(mne);

    value += mne;
    const SgAsmExpressionPtrList& operands = getOperands(iA);
    // Add to total for this variant
    // Add to total for each kind of operand
    size_t operandCount = operands.size();

    for (size_t i = 0; i < operandCount; ++i) {
      SgAsmExpression* operand = operands[i];
      value += (  isSgAsmRegisterReferenceExpression(operand) ? " R" : isSgAsmMemoryReferenceExpression(operand) ? " M" : " V");
    }

    strMap[iA] = value;
  }else
    value = iItr->second;
  return value;
};
#endif
  
std::string
LCS::unparseInstrFast(SgAsmInstruction* iA)
{

  std::string value;
  // Unparse the normalized forms of the instructions
  value += iA->get_mnemonic();
  const SgAsmExpressionPtrList& operands = getOperands(iA);
  // Add to total for this variant
  // Add to total for each kind of operand
  size_t operandCount = operands.size();

  for (size_t i = 0; i < operandCount; ++i) {
    SgAsmExpression* operand = operands[i];
    value += (  isSgAsmRegisterReferenceExpression(operand) ? " R" : isSgAsmMemoryReferenceExpression(operand) ? " M" : " V");
  }

  return value;
};


  bool
LCS::isEqual(SgNode* A, SgNode* B)
{
  if(A==NULL || B == NULL) return false;
  SgAsmInstruction* iA = isSgAsmx86Instruction(A);
  SgAsmInstruction* iB = isSgAsmx86Instruction(B);
  SgAsmFunction* fA = isSgAsmFunction(A);
  SgAsmFunction* fB = isSgAsmFunction(B);

  bool isTheSame = false;
  if(iA != NULL && iB != NULL)
    isTheSame = unparseInstrFast(iA) == unparseInstrFast(iB) ? true : false;
  if(fA != NULL && fB != NULL)
    isTheSame = fA->get_name() == fB->get_name() ? true : false;

  return isTheSame;
}


  void 
LCS::LCSLength( scoped_array<scoped_array<size_t> >& C  ,vector_start_at_one<SgNode*>& A, vector_start_at_one<SgNode*>& B )
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


void 
printDiff( scoped_array<scoped_array<size_t> >& C,
    vector_start_at_one<SgNode*>& A, vector_start_at_one<SgNode*>& B, int i, int j,
    std::vector<int >& addInstr, std::vector<int >& minusInstr
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
    //std::cout << "+ " << j << " " << unparseInstrFast( (SgAsmInstruction*) B[j]) <<std::endl;
    ROSE_ASSERT(j!=0);
    addInstr.push_back(j);
  }else  if(i > 0 && (j == 0 || C[i][j-1] < C[i-1][j]))
  {
    printDiff(C, A, B, i-1, j,addInstr, minusInstr);
    //   print "- " + X[i]
    //std::cout << "- " << i << " " << unparseInstrFast((SgAsmInstruction*)A[i]) << std::endl;
    ROSE_ASSERT(i != 0);
    minusInstr.push_back(i);
  }
}


void 
LCS::printDiff( vector_start_at_one<SgNode*>& A, vector_start_at_one<SgNode*>& B, 
       std::vector<int>& addInstr, std::vector<int >& minusInst
      )
{
  scoped_array<scoped_array<size_t> > C;
  LCSLength(C,A,B);
  printDiff(C,A, B,A.size(),B.size(),addInstr,minusInst);
};

