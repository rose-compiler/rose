#include "rose.h"
#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>

#include <iostream>
#include <list>

#include "sqlite3x.h"


using namespace std;
using namespace sqlite3x;
using namespace boost;

using namespace boost::filesystem;
using namespace boost::program_options;


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




void find_tsv_directories( const path & dir_path, std::list<path>& tsvDirectories )
{
  if ( !exists( dir_path ) ) return;

  directory_iterator end_itr; // default construction yields past-the-end
  for ( directory_iterator itr( dir_path );
      itr != end_itr;
      ++itr )
  {
    if ( is_directory(itr->status()) )
    {
      if ( itr->leaf().size() >= 4 && itr->leaf().substr(itr->leaf().size()-4) == "-tsv" )
        tsvDirectories.push_back(itr->path());
      else  
        find_tsv_directories( itr->path(), tsvDirectories );
    }
  }
}

class DeleteAST : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

void
DeleteAST::visit(SgNode* node)
   {
     delete node;
   }

void DeleteSgTree( SgNode* root)
{
  DeleteAST deleteTree;
  deleteTree.traverse(root,postorder);
}

SgNode* disassembleFile(std::string disassembleName)
{

    RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
    RoseBin_Arch::arch=RoseBin_Arch::bit32;
    RoseBin_OS::os_sys=RoseBin_OS::linux_op;
    RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;


    RoseFile* roseBin = new RoseFile( (char*)disassembleName.c_str()  );

    cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
    // query the DB to retrieve all data

    SgNode* globalBlock = roseBin->retrieve_DB();

    // traverse the AST and test it
    roseBin->test();


    return globalBlock;

}

void createDatabases(sqlite3_connection& con) {

  try {
	  con.executenonquery("create table IF NOT EXISTS timing(property_name TEXT, total_wallclock FLOAT, total_usertime FLOAT, total_systime FLOAT, wallclock FLOAT, usertime FLOAT, systime FLOAT )");
  }
  catch(std::exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
    // Function IDs are only unique within a file
	  con.executenonquery("create table IF NOT EXISTS function_statistics(row_number INTEGER PRIMARY KEY, function_id INTEGER, num_instructions INTEGER )");
  }
  catch(std::exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS function_ids(row_number INTEGER PRIMARY KEY, file TEXT, function_name TEXT)");
  }
  catch(std::exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  
  try {
	  con.executenonquery("create table IF NOT EXISTS vectors(row_number INTEGER PRIMARY KEY, function_id INTEGER,  index_within_function INTEGER, line INTEGER, offset INTEGER, sum_of_counts INTEGER, counts BLOB, instr_seq BLOB)");

  }
  catch(std::exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS clusters(row_number INTEGER PRIMARY KEY, cluster INTEGER, function_id INTEGER, index_within_function INTEGER, vectors_row INTEGER, dist INTEGER)");


  }
  catch(std::exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

}


void addVectorToDatabase(sqlite3_connection& con,/* const CloneDetectionVector& vec,*/ const std::string& functionName, size_t functionId, size_t indexWithinFunction, const std::string& normalizedUnparsedInstructions, SgAsmx86Instruction* firstInsn[], const std::string& filename, size_t windowSize, size_t stride) {
  //++numVectorsGenerated;

  vector<uint8_t> compressedCounts;/* = compressVector(vec.getBase(), CloneDetectionVector::Size);*/
  size_t vectorSum = 0;
  /*
  for (size_t i = 0; i < CloneDetectionVector::Size; ++i) {
    vectorSum += vec[i];
  }*/

  string db_select_n = "INSERT INTO vectors( function_id,  index_within_function, line, offset, sum_of_counts, counts, instr_seq ) VALUES(?,?,?,?,?,?,?)";
  string line = boost::lexical_cast<string>(isSgAsmStatement(firstInsn[0])->get_address());
  string offset = boost::lexical_cast<string>(isSgAsmStatement(firstInsn[windowSize - 1])->get_address());


  unsigned char md[16];
/*  MD5( (const unsigned char*) normalizedUnparsedInstructions.data() , normalizedUnparsedInstructions.size(), md ) ;*/
  sqlite3_command cmd(con, db_select_n.c_str());
  cmd.bind(1, (int)functionId );
  cmd.bind(2, (int)indexWithinFunction );
  cmd.bind(3, line);
  cmd.bind(4, offset);
  cmd.bind(5, boost::lexical_cast<string>(vectorSum));
  cmd.bind(6, &compressedCounts[0], compressedCounts.size());
  cmd.bind(7, md,16);
  //cmd.bind(7, "");

  cmd.executenonquery();
}

void insert_into_clusters(
    sqlite3x::sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int index_within_function,
    int vectors_row, 
    double dist 
    ) {
  std::string db_select_n = "INSERT INTO clusters(cluster, function_id, index_within_function, vectors_row, dist) VALUES(?,?,?,?,?)";
  try{
    //sqlite3_transaction trans2(con);
    {
      sqlite3x::sqlite3_command cmd(con, db_select_n.c_str());
      cmd.bind(1,cluster);
      cmd.bind(2,function_id);
      cmd.bind(3,index_within_function);
      cmd.bind(4,vectors_row);
      cmd.bind(5,dist);
      cmd.executenonquery();
    }
    //trans2.commit();
  }
  catch(std::exception &ex) {
    std::cerr << "Exception Occurred: " << ex.what() << std::endl;
  }
}

static string htmlEscape(const string& s) {
  string s2;
  for (size_t i = 0; i < s.size(); ++i) {
    switch (s[i]) {
      case '<': s2 += "&lt;"; break;
      case '>': s2 += "&gt;"; break;
      case '&': s2 += "&amp;"; break;
      default: s2 += s[i]; break;
    }
  }
  return s2;
}


enum ExpressionCategory {ec_reg = 0, ec_mem = 1, ec_val = 2};

static const size_t numberOfInstructionKinds = x86_last_instruction;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->get_kind();}



inline ExpressionCategory getCategory(SgAsmExpression* e) {
  if (isSgAsmValueExpression(e)) {
    return ec_val;
  } else if (isSgAsmRegisterReferenceExpression(e)) {
    return ec_reg;
  } else if (isSgAsmMemoryReferenceExpression(e)) {
    return ec_mem;
  } else {
    abort();
  }
}

SgAsmExpressionPtrList& getOperands(SgAsmInstruction* insn) {
  SgAsmOperandList* ol = insn->get_operandList();
  SgAsmExpressionPtrList& operands = ol->get_operands();
  return operands;
}
static map<string, void*> internTable;

inline void* intern(const std::string& s) {
  map<string, void*>::const_iterator i = internTable.find(s);
  if (i == internTable.end()) {
    void* sCopy = new string(s);
    internTable.insert(std::make_pair(s, sCopy));
    return sCopy;
  } else {
    return i->second;
  }
}
static map<SgAsmExpression*, void*> unparseAndInternTable;

inline void* unparseAndIntern(SgAsmExpression* e) {
  map<SgAsmExpression*, void*>::const_iterator i = unparseAndInternTable.find(e);
  if (i == unparseAndInternTable.end()) {
    void* sPtr = intern(unparseExpression(e));
    unparseAndInternTable.insert(std::make_pair(e, sPtr));
    return sPtr;
  } else {
    return i->second;
  }
}
 
void numberOperands(std::vector<SgAsmx86Instruction*>::iterator beg,
        std::vector<SgAsmx86Instruction*>::iterator end, map<SgAsmExpression*, size_t> numbers[3]) {
  map<void*, size_t> stringNumbers[3];
  for (; beg != end; ++beg) {
    SgAsmx86Instruction* insn = *beg;
    const SgAsmExpressionPtrList& operands = getOperands(insn);
    //size_t operandCount = operands.size();
    for (size_t j = 0; j < operands.size(); ++j) {
      SgAsmExpression* e = operands[j];
      ExpressionCategory cat = getCategory(e);
      void* str = unparseAndIntern(e);
      map<void*, size_t>& currentStringNums = stringNumbers[(int)cat];
      map<void*, size_t>::const_iterator stringNumIter = currentStringNums.find(str);
      size_t num = (stringNumIter == currentStringNums.end() ? currentStringNums.size() : stringNumIter->second);
      if (stringNumIter == currentStringNums.end()) currentStringNums.insert(std::make_pair(str, num));
      numbers[(int)cat][e] = num;
    }
  }
}
std::string normalizeInstructionsToHTML(std::vector<SgAsmx86Instruction*>::iterator beg, 
    std::vector<SgAsmx86Instruction*>::iterator end)
{
    string normalizedUnparsedInstructions;
    map<SgAsmExpression*, size_t> valueNumbers[3];
    numberOperands( beg,end, valueNumbers);

    // Unparse the normalized forms of the instructions
    for (; beg != end; ++beg ) {
      SgAsmx86Instruction* insn = *beg;
      string mne = insn->get_mnemonic();
      boost::to_lower(mne);
      mne = "<font color=\"red\">" + htmlEscape(mne)+"</font>";

      normalizedUnparsedInstructions += mne;
      const SgAsmExpressionPtrList& operands = getOperands(insn);
      // Add to total for this variant
      // Add to total for each kind of operand
      size_t operandCount = operands.size();

      normalizedUnparsedInstructions += "<font color=\"blue\">";
      for (size_t i = 0; i < operandCount; ++i) {
        SgAsmExpression* operand = operands[i];
        ExpressionCategory cat = getCategory(operand);
        map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
        assert (numIter != valueNumbers[(int)cat].end());
        size_t num = numIter->second;

        normalizedUnparsedInstructions += (cat == ec_reg ? " R" : cat == ec_mem ? " M" : " V") + boost::lexical_cast<string>(num);
      }
      normalizedUnparsedInstructions += "; </font> <br> ";
  
    }
   
    return normalizedUnparsedInstructions;
};

void normalizeInstructionInSubTree(SgNode* topNode ){
  vector<SgAsmx86Instruction*> insns;
  FindInstructionsVisitor vis;
  AstQueryNamespace::querySubTree(topNode, std::bind2nd( vis, &insns ));

  for(std::vector<SgAsmx86Instruction*>::iterator iItr = insns.begin(); iItr !=  insns.end();
      ++iItr)
  {
      SgAsmx86Instruction* insn = *iItr;
       SgAsmExpressionPtrList& operands = getOperands(insn);
      // Add to total for this variant
      // Add to total for each kind of operand
      size_t operandCount = operands.size();

      for (size_t i = 0; i < operandCount; ++i) {
        SgAsmExpression* operand = operands[i];
        SgAsmExpression* newOperand;
        if( isSgAsmMemoryReferenceExpression(operand) )
        {
          SgAsmMemoryReferenceExpression* memRefExp = new SgAsmMemoryReferenceExpression;
          SgAsmWordValueExpression* wordVal = new SgAsmWordValueExpression;
          wordVal->set_value(0);

          memRefExp->set_segment(wordVal);
          memRefExp->set_address(wordVal);
          memRefExp->set_type(new SgAsmTypeWord);
          newOperand = memRefExp;
        }else if(isSgAsmRegisterReferenceExpression(operand) ){
          SgAsmx86RegisterReferenceExpression* regRef = new SgAsmx86RegisterReferenceExpression;
          regRef->get_descriptor().set_major(x86_regclass_mm);
          regRef->get_descriptor().set_minor(0);

          newOperand = regRef;

        }else{
          SgAsmWordValueExpression* wordVal = new SgAsmWordValueExpression;
          wordVal->set_value(0);
          newOperand = wordVal;
          
        }

        newOperand->set_parent(operand->get_parent());

        DeleteSgTree(operands[i]);
        operands[i]=newOperand;
      }
        //std::cout << "Unparsed: " <<unparseX86Instruction(insn)<< std::endl;

  }







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
    std::cout << "+ " << j << " " << unparseInstrFast( (SgAsmInstruction*) B[j]) <<std::endl;
  }else  if(i > 0 && (j == 0 || C[i][j-1] < C[i-1][j]))
  {
    printDiff(C, A, B, i-1, j);
    //   print "- " + X[i]
    std::cout << "- " << i << " " << unparseInstrFast((SgAsmInstruction*)A[i]) << std::endl;
  }
}

void printDiff( scoped_array<scoped_array<size_t> >& C,  
    vector_start_at_zero<SgNode*>& A, vector_start_at_zero<SgNode*>& B, int i, int j,
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




int main(int argc, char** argv)
{

#if 0
//  SgNode* disFile = disassembleFile(argv[1]);

  {
  SgProject* disFile = frontend(argc,argv);
//  normalizeInstructionInSubTree(disFile);
  
  FindInstructionsVisitor vis;

  //  backend((SgProject*)disFile);
  vector<SgNode*> insnsA;

  AstQueryNamespace::querySubTree(disFile, std::bind2nd( vis, &insnsA ));


  {
    ofstream myfile;
    std::string outputFile = string(argv[1])+".roseTxt";
    myfile.open (outputFile.c_str());

    std::vector<SgAsmx86Instruction*> instructions;
    for(int i = 0; i < insnsA.size(); i++ )
      if(isSgAsmx86Instruction(insnsA[i]))
      {
        normalizeInstructionInSubTree(insnsA[i]);
        myfile <<   unparseInstruction((SgAsmInstruction*)insnsA[i]) << "\n";
      }
    myfile.close();
  }

  {
    ofstream myfile;
    std::string outputFile = string(argv[1])+".roseTxtNoEndl";
    myfile.open (outputFile.c_str());

    std::vector<SgAsmx86Instruction*> instructions;
    for(int i = 0; i < insnsA.size(); i++ )
      if(isSgAsmx86Instruction(insnsA[i]))
      {
  //      normalizeInstructionInSubTree(insnsA[i]);
        myfile <<   unparseInstruction((SgAsmInstruction*)insnsA[i]) ;
      }

    myfile.close();

  }

 //     instructions.push_back(isSgAsmx86Instruction(insnsA[i]));


//  myfile << normalizeInstructionsToHTML(instructions.begin(), instructions.end());


  exit(0);

  }
#endif
  std::vector<std::string> dir_paths;

  std::string originalFile, patchedFile;

  std::string database;

  sqlite3_connection con;


  //Getting the program options
  
  options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("searchPath,p", value< std::vector< std::string > >(), "path where tsv-directories are located")
    ("original,o", value< std::string  >(), "original file")
    ("patched,n", value<  std::string  >(), "new, patched file")
    ("database,d", value<  std::string  >(), "the output database")
    ;

  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);    

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  if (vm.count("searchPath")) {
//    cout << "The search paths for tsv-directories are: " 
//      << vm["searchPath"].as< vector<string> >() << "\n";
    dir_paths = vm["searchPath"].as< vector<string> >();


  } else {
    cerr << "Error: Please set search paths for tsv-directories.\n";
    exit(1);
  }

  std::cout << "original" << std::endl;

  if(vm.count("original"))
  {
    originalFile = vm["original"].as<string>();
  }else{
    std::cerr << "Please specify the name of the original file" << std::endl;
    exit(1);
  }

  std::cout << "patched" << std::endl;

  if(vm.count("patched"))
  {
    patchedFile = vm["patched"].as<string>();
  }else{
    std::cerr << "Please specify the name of the patched file" << std::endl;
    exit(1);
  }

  std::cout << "database" << std::endl;

  if(vm.count("database"))
  {
    database = vm["database"].as<string>();
  }else{
    std::cerr << "Please specify the name of the output database" << std::endl;
    exit(1);
  }


  std::cout << "database" << std::endl;

  con.open(database.c_str());
  createDatabases(con);


  //Getting all tsv directories (IDA representation of a binary)
  std::list<path> tsvDirectories; 

  for(std::vector<std::string>::iterator pItr = dir_paths.begin();
      pItr != dir_paths.end(); ++pItr )
  {
    path dir_path(*pItr);
    find_tsv_directories(dir_path, tsvDirectories);
  }
  tsvDirectories.sort();
  tsvDirectories.unique();


  std::vector<char*> newArgs1;

  newArgs1.push_back(strdup(argv[0]));
  newArgs1.push_back(strdup(originalFile.c_str()));
   std::vector<char*> newArgs2;

  newArgs2.push_back(strdup(argv[0]));

  newArgs2.push_back(strdup(patchedFile.c_str()));


//  SgNode* original = disassembleFile(originalFile);
//  SgNode* patched  = disassembleFile(patchedFile);
  
  SgNode* original = frontend(newArgs1.size(), &newArgs1[0]) ;
  SgNode* patched  = frontend(newArgs2.size(), &newArgs2[0]) ;

  normalizeInstructionInSubTree(original);
  normalizeInstructionInSubTree(patched);

  backend((SgProject*)original);
  backend((SgProject*)patched);

  vector<SgAsmFunctionDeclaration*> funcsOrig, funcsPatched;
  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(original, std::bind2nd( funcVis, &funcsOrig ));
  AstQueryNamespace::querySubTree(patched, std::bind2nd( funcVis, &funcsPatched ));


  FindInstructionsVisitor vis;


  vector<std::string> patternsToLookFor;

  for(std::vector<SgAsmFunctionDeclaration*>::iterator fItr = funcsOrig.begin();
      fItr != funcsOrig.end(); ++fItr )
  {
    for(std::vector<SgAsmFunctionDeclaration*>::iterator f2Itr = funcsPatched.begin();
        f2Itr != funcsPatched.end(); ++f2Itr )
    {
      if((*fItr)->get_name() == (*f2Itr)->get_name())
      {
        std::cout << "comparing " << (*fItr)->get_name()  << " with " << (*f2Itr)->get_name() << std::endl;
        scoped_array<scoped_array<size_t> > C;
        vector_start_at_zero<SgNode*> insnsA;
        AstQueryNamespace::querySubTree(*fItr, std::bind2nd( vis, &insnsA ));
        vector_start_at_zero<SgNode*> insnsB;
        AstQueryNamespace::querySubTree(*f2Itr, std::bind2nd( vis, &insnsB ));

        LCSLength(C,insnsA,insnsB);

        std::vector<pair<int,int> > addInstr,minusInst;

        printDiff(C,insnsA, insnsB,insnsA.size(),insnsB.size(),addInstr,minusInst);

        std::list<int> changeAt;
        for( std::vector<pair<int,int> >::iterator addItr = addInstr.begin();
            addItr != addInstr.end(); ++ addItr )
          changeAt.push_back(addItr->first);

        for( std::vector<pair<int,int> >::iterator minusItr = minusInst.begin();
            minusItr != minusInst.end(); ++ minusItr )
          changeAt.push_back(minusItr->first);

        changeAt.sort();
        changeAt.unique();

        int begChange=0;
        int endChange=0;

        int bounderyAroundChange=20;

        for(std::list<int>::iterator iItr = changeAt.begin();
            iItr != changeAt.end(); ++iItr)
        {
          int insnAindex = *iItr;
          if(insnAindex-bounderyAroundChange < 0 )
            begChange =0;
          else
            begChange = insnAindex-bounderyAroundChange;

          endChange = begChange+bounderyAroundChange;

          std::string changeString;
          for(int i = begChange; i <= endChange ; i++ )
            changeString += unparseInstrFast((SgAsmInstruction*)insnsA[i]);

          patternsToLookFor.push_back(changeString);


          //Ensure that changes are not covered twice
          for(; iItr != changeAt.end(); ++iItr)
            if(*iItr > begChange+bounderyAroundChange) break;

        }


      }


    }

  }


  std::cout << "orignal file " << originalFile << " patched " << patchedFile <<std::endl;

 
  for(std::list<path>::iterator pItr = tsvDirectories.begin();
      pItr != tsvDirectories.end(); ++pItr)
  {
    std::cout << "Currently comparing file: " << *pItr << std::endl;

    SgNode* globalBlock = disassembleFile( pItr->string() );

    /* Put in code to search for the patterns here */



    DeleteSgTree(globalBlock);
  }

}
