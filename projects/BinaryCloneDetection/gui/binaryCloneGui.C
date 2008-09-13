#include <boost/algorithm/string.hpp>
#include "binaryCloneGui.h"
#include <ext/hash_map>


#include <sstream>
#include <QFileDialog>

#include <unistd.h>

#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "icons.h"

//#include "disks.xpm"
#include "folder.xpm"

#define EMACS

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace sqlite3x;
using namespace __gnu_cxx;


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




void toolbarClick(int action)
{
  BinaryCloneGui *instance = QROSE::cbData<BinaryCloneGui *>();

  switch(action){
    case 0:
      instance->run();
      break;
 
    case 2: 		
      instance->open();     
      break;
    case 3: 		
      QROSE::exit(0); break;
      break;

    default:
      eAssert(0, ("internal error: invalid selection type!"));

  }
  
}


static void tableCellActivated(int col, int row, int oldCol, int oldRow) 
{

  BinaryCloneGui *instance = QROSE::cbData<BinaryCloneGui *>();
  instance->unhighlightRow(oldRow);
  instance->highlightRow(row);

  return;

} //tableCellActivated(int col, int row, int oldCol, int oldRow)


BinaryCloneGui::BinaryCloneGui( ) :
  window(0)
{
  window = new QRWindow( "mainWindow", QROSE::TopDown );

  {
    //--------------------------------------------------------------------------

    QRToolBar *toolbar = (*window)["toolbar"] << new QRToolBar(QROSE::LeftRight, true, true, true);

    // icons are defined in <icons.h>
    toolbar->addButton("run");   toolbar->setPicture(0, iconRun); 
    toolbar->addButton("reset"); toolbar->setPicture(1, iconReset);
    toolbar->addSeparator();
    toolbar->addButton("open"); toolbar->setPicture(2, folder_xpm);
    toolbar->addButton("quit"); toolbar->setPicture(3, iconQuit);

    QROSE::link(toolbar, SIGNAL(clicked(int)), &toolbarClick, this);

  } //window 



  tableWidget = *window << new QRTable( 8, "file A", "function A", "begin address", "end address", "file B", "function B", "begin address", "end address" );
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableCellActivated, this);

  codeWidget = *window << new QTextEdit;//new QREdit(QREdit::Box);
  codeWidget->setReadOnly(true);
  codeWidget->setPlainText(QString("foobar\nBar\nFoobar"));

  
  window->setGeometry(0,0,1280,800);
  window->setTitle("BinaryCloneMainGui (QROSE)");

} //BinaryCloneGui::BinaryCloneGui()

void BinaryCloneGui::open()
{
  char buf[4096] = "\0";

  std::string database = QFileDialog::getOpenFileName( 0, "Open As", getcwd(buf, 4096), "ASCII (*.sql)").toStdString();

  if( database.empty() ) return;


  if(exists(database) == true) 
  {
     con.close();
     codeWidget->setReadOnly(true);
     codeWidget->setPlainText(QString("foobar\nBar\nFoobari3"));
     con.open(database.c_str());
  }
  

} //CompassGui::open()


void
BinaryCloneGui::run( ) 
{
    codeWidget->setPlainText(QString("foobar\nBar\nFoobar2"));

    //Fill in table
    // unlink activation callback, which otherwise would trigger each
    // time we add a row in the table.
    QROSE::unlink(tableWidget, SIGNAL(activated(int, int, int, int)));

    while(tableWidget->rowCount()) tableWidget->removeRow(0);

    size_t eltCount = 0;

    try {
      eltCount = boost::lexical_cast<size_t>(con.executestring("select max(row_number) from largest_clones"));
    } catch (exception& e) { 
      eAssert(0, ("internal error: No largest_clones table found -- invalid database?!"));
    }

    if (eltCount == 0) {
      eAssert(0, ("internal error: No largest_clones table found -- invalid database?!"));
      exit (1);
    }

    vectorOfClones.allocate(eltCount);


    //Read whole dataset from database
    try{
      std::string selectSeparateDatasets ="select   function_id_A, function_id_B, begin_index_within_function_A, end_index_within_function_A, begin_index_within_function_B, end_index_within_function_B, f1.file, f1.function_name, f2.file, f2.function_name from largest_clones c join function_ids f1 on f1.row_number=c.function_id_A join function_ids f2 on f2.row_number=c.function_id_B";

      sqlite3_command cmd(con, selectSeparateDatasets.c_str());
      sqlite3_reader datasets=cmd.executereader();

      int row = 0;

      while(datasets.read())
      {

        Element cur_elem;
        cur_elem.function_A = boost::lexical_cast<uint64_t>(datasets.getstring(0));
        cur_elem.function_B = boost::lexical_cast<uint64_t>(datasets.getstring(1));
        cur_elem.begin_index_within_function_A = boost::lexical_cast<uint64_t>(datasets.getstring(2));
        cur_elem.end_index_within_function_A   = boost::lexical_cast<uint64_t>(datasets.getstring(3));
        cur_elem.begin_index_within_function_B = boost::lexical_cast<uint64_t>(datasets.getstring(4));
        cur_elem.end_index_within_function_B   = boost::lexical_cast<uint64_t>(datasets.getstring(5));
        cur_elem.file_A = datasets.getstring(6);
        cur_elem.function_name_A = datasets.getstring(7);
        cur_elem.file_B = datasets.getstring(8);
        cur_elem.function_name_B = datasets.getstring(9);



        tableWidget->addRows(1);

        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.file_A), 0, row);

        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.function_name_A), 1, row);
        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.begin_index_within_function_A), 2, row);
        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.end_index_within_function_A), 3, row);
        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.file_B), 4, row);

        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.function_name_B), 5, row);
        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.begin_index_within_function_B), 6, row);
        tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.end_index_within_function_B), 7, row);

        tableWidget->setHAlignment(true, false, 0); // left horizontal alignment

        vectorOfClones[row] = cur_elem;
        row++;
      }
    }
    catch(exception& e) {
	//cout << e.what() << "\n";
  }

    QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), 
        &tableCellActivated, this);

}

SgNode* BinaryCloneGui::disassembleFile(std::string tsv_directory){
  SgNode* globalBlock;

  if(exists(tsv_directory) == false)
  {

    char buf[4096] = "\0";

    int i = 0; 
    while( exists ( relativePathPrefix +"/"+ tsv_directory) == false ) 
    {
      
      if(i>10){
        std::string error_message = "user error: Relative Path to  ";
        error_message+=tsv_directory;
        error_message+=" not selected in " ;
        error_message+= boost::lexical_cast<std::string>(i);
        error_message+=" attempts.";
        eAssert(0,  (error_message.c_str()) );
      }

      QFileDialog dialog;
      dialog.setFileMode(QFileDialog::DirectoryOnly);
      //relativePathPrefix = dialog.getOpenFileName( 0, "Relative Path To Binaries", getcwd(buf, 4096), "ASCII (*)").toStdString();
      relativePathPrefix = dialog.getExistingDirectory(0,  "get existing directory", getcwd(buf, 4096)).toStdString();
      i++;
    }

    tsv_directory = relativePathPrefix +"/"+tsv_directory;

  };
  
  if(is_directory( tsv_directory  ) == true )
  {
    RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
    RoseBin_Arch::arch=RoseBin_Arch::bit32;
    RoseBin_OS::os_sys=RoseBin_OS::linux_op;
    RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;


    RoseFile* roseBin = new RoseFile( (char*)tsv_directory.c_str() );

    cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
    // query the DB to retrieve all data

    globalBlock = roseBin->retrieve_DB();

    // traverse the AST and test it
    roseBin->test();
  }else{
    vector<char*> args;
    args.push_back(strdup(""));
    args.push_back(strdup(tsv_directory.c_str()));
    args.push_back(0);

    ostringstream outStr; 

    for(vector<char*>::iterator iItr = args.begin(); iItr != args.end();
        ++iItr )
    {
      outStr << *iItr << " ";
    }     
    ;
    std::cout << "Calling " << outStr.str() << std::endl;

    globalBlock =  frontend(args.size()-1,&args[0]);
    
  }
  return globalBlock;
};


std::string BinaryCloneGui::getAddressFromVectorsTable(uint64_t function_id, uint64_t index)
{
  std::string address;
  try {
    std::string selectQuery = "select row_number from vectors where function_id=";
                selectQuery +=boost::lexical_cast<std::string>(function_id);
                selectQuery +=" and index_within_function=";
                selectQuery +=boost::lexical_cast<std::string>(index);
                selectQuery +=" limit 1";
    address = sqlite3x::sqlite3_command(con, selectQuery.c_str()).executestring();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}

  return address;
}  ;

class FindInstructionsVisitor: public std::binary_function<SgNode*, std::vector<SgAsmx86Instruction *>* , void* >
{
  public:
    void* operator()(first_argument_type node, std::vector<SgAsmx86Instruction*>* insns ) const{
      if (isSgAsmx86Instruction(node)) insns->push_back(isSgAsmx86Instruction(node));
      return NULL;
    }
};

class FindAsmFunctionsVisitor: public std::binary_function<SgNode*, std::vector<SgAsmFunctionDeclaration *>* , void* >
{
  public:
    void* operator()(first_argument_type node, std::vector<SgAsmFunctionDeclaration*>* insns ) const{
      if (isSgAsmFunctionDeclaration(node)) insns->push_back(isSgAsmFunctionDeclaration(node));
      return NULL;
    }
};


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
    void* sPtr = intern(unparseX86Expression(e));
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
std::string BinaryCloneGui::normalizeInstructions(std::vector<SgAsmx86Instruction*>::iterator beg, 
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
      normalizedUnparsedInstructions += mne;
      const SgAsmExpressionPtrList& operands = getOperands(insn);
      // Add to total for this variant
      // Add to total for each kind of operand
      size_t operandCount = operands.size();

      for (size_t i = 0; i < operandCount; ++i) {
        SgAsmExpression* operand = operands[i];
        ExpressionCategory cat = getCategory(operand);
        map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
        assert (numIter != valueNumbers[(int)cat].end());
        size_t num = numIter->second;

        normalizedUnparsedInstructions += (cat == ec_reg ? "R" : cat == ec_mem ? "M" : "V") + boost::lexical_cast<string>(num);
      }
      normalizedUnparsedInstructions += ";";
  
    }
   
    return normalizedUnparsedInstructions;
};


void BinaryCloneGui::showClone(int row)
{

  //Disassemble files

  Element& elem = vectorOfClones[row];

 
  std::string beginAddressFileA = getAddressFromVectorsTable(elem.function_A, elem.begin_index_within_function_A);
  std::string beginAddressFileB = getAddressFromVectorsTable(elem.function_B, elem.begin_index_within_function_B);
  std::string endAddressFileA = getAddressFromVectorsTable(elem.function_A, elem.end_index_within_function_A);
  std::string endAddressFileB = getAddressFromVectorsTable(elem.function_B, elem.end_index_within_function_B);

  SgNode* fileA = disassembleFile(elem.file_A);
  SgNode* fileB = disassembleFile(elem.file_B);

  std::string unparsedFileA;
  std::string unparsedFileB;

  std::string normalizedFileA;
  std::string normalizedFileB;
 

  bool first =true;
  while(true)
  {
    SgNode* currentRoot = first ? fileA : fileB;
    
    vector<SgAsmx86Instruction*> insns;
    FindInstructionsVisitor vis;
    AstQueryNamespace::querySubTree(currentRoot, std::bind2nd( vis, &insns ));

    int beg = 0;
    int end = 0;
    for(size_t i=0; i < insns.size(); i++ )
    {
      if(insns[i]->get_address() == boost::lexical_cast<uint64_t>( first ? beginAddressFileA : beginAddressFileB ) ) 
        beg = i;
       if(insns[i]->get_address() == boost::lexical_cast<uint64_t>( first ? endAddressFileA : endAddressFileB ) ) 
       {
         end = i;
         break;
       }
    }

//    first ? normalizedFileA : normalizedFileB = normalizeInstructions(insns.begin()+beg, insns.begin()+end);

    std::string unparsed;
    for(int i =beg; i < end; i++)
    {
      unparsed += unparseInstructionWithAddress(insns[i]);
      unparsed += "\n";
    }
     codeWidget->setPlainText(QString(unparsed.c_str()));

     break;
    codeWidget->setReadOnly(true);
    codeWidget->setPlainText(QString(normalizeInstructions(insns.begin()+beg, insns.begin()+end).c_str()));


   
    if(first == true) first = false;
    else break;
      
  }

  

  DeleteSgTree(fileA);
  DeleteSgTree(fileB);

}
  
void BinaryCloneGui::highlightRow(int row)
{
  activeRow = -1;

  if(row >= 0)
  {         
    QFont f = tableWidget->getFont(0, row);
    f.setBold(true);
    tableWidget->setFont(f, 0, row);
    activeRow = row;

    showClone(row);

    
    //tableWidget->isItemSelected(tableWidget->horizontalHeaderItem(row));

  } //if(row >= 0)

  return;
} //CompassGui::highlightRow(int row)

void BinaryCloneGui::unhighlightRow(int row)
{
  if (row >= 0) 
  {
    QFont f = tableWidget->getFont(0, row);
    f.setBold(false);
    tableWidget->setFont(f, 0, row);
  } //if (row >= 0)

  return;
} //CompassGui::unhighlightRow(int row)




BinaryCloneGui::~BinaryCloneGui()
{
  if( window ) delete window;

} //BinaryCloneGui::~BinaryCloneGui()



