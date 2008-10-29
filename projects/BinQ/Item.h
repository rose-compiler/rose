#ifndef ITEM_R_H
#define ITEM_R_H
#include "rose.h"

#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>

#include <iostream>
#include <list>

using namespace boost;

using namespace boost::filesystem;
using namespace boost::program_options;

using namespace std;

class Item {
 public:
  Item(bool function, SgAsmStatement* statement,
       int functionSize, int resolved, int row):function(function),
    statement(statement),functionSize(functionSize),
    resolved(resolved),row(row){}

  bool function;
  SgAsmStatement* statement;
  int functionSize;
  int resolved;
  int row;
};


template <typename T>
class vector_start_at_one {
  vector<T> sa;
  size_t theSize;

 public:
 vector_start_at_one(): sa(), theSize(0) {}

  size_t size() const {return sa.size();}
  T* get() const {return sa.get();}

  void push_back(T v){ sa.push_back(v);  }
  T& operator[](size_t i) {return sa[i-1];}
  const T& operator[](size_t i) const {return sa[i];}

 private:
  vector_start_at_one(const vector_start_at_one<T>&); // Not copyable
};









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




#endif
