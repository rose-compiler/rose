/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Example code on how to use RoseBin
 ****************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include "rose.h"
#include "sqlite3x.h"
#include <boost/smart_ptr.hpp>

#include <boost/program_options.hpp>


using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;
using namespace boost;
template <typename T>
class vector_start_at_zero {
  vector<T> sa;
  size_t theSize;

  public:
  vector_start_at_zero(): sa(), theSize(0) {}

  size_t size() const {return sa.size();}
  T* get() const {return sa.get();}

  void push_back(T v){ sa.push_back(v);  }
  T& operator[](size_t i) {return sa[i-1];}
  const T& operator[](size_t i) const {return sa[i];}

  private:
  vector_start_at_zero(const vector_start_at_zero<T>&); // Not copyable
};


inline double tvToDouble(const timeval& tv) {
  return tv.tv_sec + tv.tv_usec * 1.e-6;
}




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
  SgAsmFunction* fA = isSgAsmFunction(A);
  SgAsmFunction* fB = isSgAsmFunction(B);
  
  bool isTheSame = false;
  if(iA != NULL && iB != NULL)
    isTheSame = unparseInstrFast(iA) == unparseInstrFast(iB) ? true : false;
  if(fA != NULL && fB != NULL)
    isTheSame = fA->get_name() == fB->get_name() ? true : false;

  return isTheSame;
}


/*
function printDiff(C[0..m,0..n], X[1..m], Y[1..n], i, j)
      if i > 0 and j > 0 and X[i] = Y[j]
              printDiff(C, X, Y, i-1, j-1)
          print "  " + X[i]
              else
                      if j > 0 and (i = 0 or C[i,j-1] ≥ C[i-1,j])
              printDiff(C, X, Y, i, j-1)
              print "+ " + Y[j]
                      else if i > 0 and (j = 0 or C[i,j-1] < C[i-1,j])
              printDiff(C, X, Y, i-1, j)
              print "- " + X[i]
*/            


void printDiff( scoped_array<scoped_array<size_t> >& C,  
    vector_start_at_zero<SgNode*>& A, vector_start_at_zero<SgNode*>& B, int i, int j
    )
{
  if(i> 0 && j > 0 && isEqual(A[i],B[j]))
  {
    printDiff(C,A,B,i-1,j-1);
    //print " " + X[i]
  }else if( j > 0 && (i == 0 || C[i][j-1] >= C[i-1][j]))
  {
    printDiff(C,A,B,i,j-1);
    //print "+ " + B[j]
    std::cout << "+ " << j << std::endl;
  }else  if(i > 0 && (j == 0 || C[i][j-1] < C[i-1][j]))
  {
           printDiff(C, A, B, i-1, j);
           //   print "- " + X[i]
           std::cout << "- " << i << std::endl;
  }
}


/*
  function  LCSLength(X[1..m], Y[1..n])
C = array(0..m, 0..n)
  for i := 0..m
  C[i,0] = 0
  for j := 1..n
  C[0,j] = 0
  for i := 1..m
  for j := 1..n
  if X[i] = Y[j]
  C[i,j] := C[i-1,j-1] + 1
  else:
C[i,j] := max(C[i,j-1], C[i-1,j])
  return C[m,n]
*/






void LCSLength( scoped_array<scoped_array<size_t> >& C  ,vector_start_at_zero<SgNode*>& A, vector_start_at_zero<SgNode*>& B )
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



int main(int argc, char* argv[]) {
  struct timeval start;
  gettimeofday(&start, NULL);

  std::string database;
  std::vector<std::string> tsv_directory;
  


  try {
	options_description desc("Allowed options");
	desc.add_options()
	  ("help", "produce a help message")
	  ("database", value< string >()->composing(), 
	   "the sqlite database that we are to use")
	  ("tsv-directory", value< std::vector<string> >(&tsv_directory)->composing(), 
	   "the input tsv directory")
          ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);


	if (vm.count("help")) {
	  cout << desc;           
	  exit(0);
	}
        if (vm.count("database")!=0 || vm.count("tsv-directory")==2 ) {
	  std::cerr << "Missing options. Call as: createVectorsBinary --database <database-name>"
		        << " --tsv-directory <tsv-directory>" << std::endl;
	  exit(1);

	}
  }
  catch(std::exception& e) {
	cout << e.what() << "\n";
  }

  

  // binary code analysis *******************************************************
  cerr << " Starting binary analysis ... " << endl;

  std::cout << "Done reading options" << std::endl;

  std::cout << "File A:" << tsv_directory[0] << " file B: "<< tsv_directory[1] << std::endl;
  RoseFile* fileA = new RoseFile( (char*)tsv_directory[0].c_str() );
  RoseFile* fileB = new RoseFile( (char*)tsv_directory[1].c_str() );

  RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
  RoseBin_Arch::arch=RoseBin_Arch::bit32;
  RoseBin_OS::os_sys=RoseBin_OS::linux_op;
  RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;

  cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
  // query the DB to retrieve all data
  SgNode* globalBlockA = fileA->retrieve_DB();
  SgNode* globalBlockB = fileB->retrieve_DB();

  // traverse the AST and test it
  fileA->test();
  fileB->test();
  vector_start_at_zero<SgNode*> insnsA;
  FindInstructionsVisitor vis;
  AstQueryNamespace::querySubTree(globalBlockA, std::bind2nd( vis, &insnsA ));
  vector_start_at_zero<SgNode*> insnsB;

  AstQueryNamespace::querySubTree(globalBlockB, std::bind2nd( vis, &insnsB ));

  ROSE_ASSERT(insnsA.size()>0);
   ROSE_ASSERT(insnsB.size()>0);
 
  scoped_array<scoped_array<size_t> > C;

  LCSLength(C,insnsA,insnsB);
  printDiff(C,insnsA, insnsB,insnsA.size(),insnsB.size());
  
  return 0;

} 
