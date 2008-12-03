#include "BinQGui.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "icons.h"
#include "folder.xpm"

#include "BinQSupport.h"
#include "slide.h"
#include "MyBars.h"

#include <qtabwidget.h>
#include <QtGui>
#include <QScrollBar>

#include "LCS.h"
#include "Clone.h"
#include "FunctionDiff.h"
#include "AlignFunctions.h"
#include "BinCallGraph.h"
#include "BinControlFlowAnalysis.h"
#include "BinDataFlowAnalysis.h"
#include "BufferOverflow.h"
#include "InterruptAnalysis.h"
#include "BinDynamicInfo.h"

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;


// ----------------------------------------------------------------------------------------------
// specify analyses here!
//void alignFunctions();
//void andreasDiff();
//void alignFunctionsSmart();

void clicked1() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  QListWidgetItem *item= instance->listWidget->currentItem(); 
  QString te = item->text();
  string t = te.toStdString();
  //const char *t = (const char *)text;
  cerr << " Selected : " << t << endl;
  for (unsigned int i=0;i<instance->analyses.size();++i) {
    BinAnalyses* analysis = instance->analyses[i];
    if (t==analysis->name()) {
      instance->analysisInfo->setText(instance->analyses[i]->getDescription().c_str());
      bool twoFiles = instance->analyses[i]->twoFiles();
      if (twoFiles && instance->fileB!=NULL || twoFiles==false)
	instance->currentAnalysis=instance->analyses[i];
    } 
  }
} 

void clicked2(QListWidgetItem* item) {}



// ----------------------------------------------------------------------------------------------
void toolbarClick(int action) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  switch(action){
  case 0:
    instance->run();
    break;
  case 1:
    instance->reset();
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



static void tableWidgetCellActivatedA(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightFunctionRow(oldRow, true);
  instance->highlightFunctionRow(row,true);
    return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void codeTableWidgetCellActivatedA(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightInstructionRow(oldRow, true);
  instance->highlightInstructionRow(row, true);
  Item* item = instance->rowItemFileA[row];
  if (item) {
    QString res = QString("A: Item selected  %1 : %2.")
      .arg(item->row)
      .arg(isSgNode(item->statement)->class_name().c_str());
    instance->console->append(res);  
    SgAsmElfSection* sec = isSgAsmElfSection(item->statement);
    if (sec) {
      instance->analysisTab->setCurrentIndex(3);
      instance->insertSectionInformation(sec,instance->fileNameA);      
    }
  }
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void tableWidgetCellActivatedB(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightFunctionRow(oldRow, false);
  instance->highlightFunctionRow(row,false);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void codeTableWidgetCellActivatedB(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightInstructionRow(oldRow, false);
  instance->highlightInstructionRow(row, false);
  Item* item = instance->rowItemFileB[row];
  if (item) {
    QString res = QString("B: Item selected  %1 : %2.")
      .arg(item->row)
      .arg(isSgNode(item->statement)->class_name().c_str());
    instance->console->append(res);  
    SgAsmElfSection* sec = isSgAsmElfSection(item->statement);
    if (sec) {
      instance->analysisTab->setCurrentIndex(3);
      instance->insertSectionInformation(sec,instance->fileNameB);      
    }
  }
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)




// ----------------------------------------------------------------------------------------------


void BinQGUI::highlightFunctionRow(int row, bool fileAYes) {
  if(row >= 0)    {         
    if (fileAYes) {
      QFont f = tableWidget->getFont(0, row);
      f.setBold(true);
      tableWidget->setFont(f, 0, row);
      tableWidget->setBgColor(QColor(255,255,0),0,row);
      
      if (isSgAsmFunctionDeclaration(funcsFileA[row])) {
	SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(funcsFileA[row]);
	std::vector<Item*>::iterator it = itemsFileA.begin();
	int offset=0;
	for (;it!=itemsFileA.end();++it) {
	  Item* item = *it;
	  if (isSgAsmStatement(item->statement)) {
	    SgAsmStatement* stat = isSgAsmStatement(item->statement);
	    if (func==isSgAsmFunctionDeclaration(stat)) {
	      offset=item->row;
	      break;
	    }
	  } 
	}
      
	//cerr << " highlight func row : " << row << "  inst row : " << offset << endl;
	codeTableWidget->setCurrentCell(offset,0);
      }
    } else if (fileB) {
      QFont f = tableWidget2->getFont(0, row);
      f.setBold(true);
      tableWidget2->setFont(f, 0, row);
      tableWidget2->setBgColor(QColor(255,255,0),0,row);
      
      if (isSgAsmFunctionDeclaration(funcsFileB[row])) {
	SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(funcsFileB[row]);
	std::vector<Item*>::iterator it = itemsFileB.begin();
	int offset=0;
	for (;it!=itemsFileB.end();++it) {
	  Item* item = *it;
	  SgAsmStatement* stat = isSgAsmStatement(item->statement);
	  if (func==isSgAsmFunctionDeclaration(stat)) {
	    offset=item->row;
	    break;
	  }
	}
	//      cerr << " highlight func row : " << row << "  inst row : " << offset << endl;
	codeTableWidget2->setCurrentCell(offset,0);
      } else if (isSgFunctionDeclaration(funcsFileB[row])) {
	SgFunctionDeclaration* func = isSgFunctionDeclaration(funcsFileB[row]);
	std::vector<Item*>::iterator it = itemsFileB.begin();
	int offset=0;
	for (;it!=itemsFileB.end();++it) {
	  Item* item = *it;
	  SgLocatedNode* stat = isSgLocatedNode(item->statement);
	  if (func==isSgFunctionDeclaration(stat)) {
	    offset=item->row;
	    break;
	  }
	}
	codeTableWidget2->setCurrentCell(offset,0);
      }
    }
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightFunctionRow(int row, bool fileAYes) {
  if (row >= 0)     {
    if (fileAYes) {
      QFont f = tableWidget->getFont(0, row);
      f.setBold(false);
      tableWidget->setFont(f, 0, row);
      tableWidget->setBgColor(QColor(255,255,255),0,row);
    } else if (fileB) {
      QFont f = tableWidget2->getFont(0, row);
      f.setBold(false);
      tableWidget2->setFont(f, 0, row);
      tableWidget2->setBgColor(QColor(255,255,255),0,row);
    }
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)




void BinQGUI::highlightInstructionRow(int row, bool fileAYes) {
  if(row >= 0)    {         
    if (fileAYes) {
      QFont f = codeTableWidget->getFont(0, row);
      f.setBold(true);
      codeTableWidget->setCurrentCell(row,0);
      Item* item = itemsFileA[row];
      for (int j=1;j<maxrows;j++) {
	codeTableWidget->setFont(f, j, row);
	if (isSgAsmFunctionDeclaration(item->statement) ||
	    isSgFunctionDeclaration(item->statement)) 
	  codeTableWidget->setBgColor(QColor(120,120,120),j,row);
	else 
	  codeTableWidget->setBgColor(QColor(255,255,0),j,row);
      }
    } else if (fileB) {
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(true);
      codeTableWidget2->setCurrentCell(row,0);
      Item* item = itemsFileB[row];
      if (item) {
	for (int j=1;j<maxrows;j++) {
	  codeTableWidget2->setFont(f, j, row);
	  if (isSgAsmFunctionDeclaration(item->statement) ||
	      isSgFunctionDeclaration(item->statement)) 
	    codeTableWidget2->setBgColor(QColor(120,120,120),j,row);
	  else
	    codeTableWidget2->setBgColor(QColor(255,255,0),j,row);
	}
      }
    }
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightInstructionRow(int row,bool fileAYes) {
  if (row >= 0)     {
    if (fileAYes) {
      QFont f = codeTableWidget->getFont(0, row);
      f.setBold(false);
      Item* item = itemsFileA[row];
      for (int j=1;j<maxrows;j++) {      
	codeTableWidget->setFont(f, j, row);
	codeTableWidget->setBgColor(item->bg,j,row);
      }
    } else if (fileB){
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(false);
      Item* item = itemsFileB[row];
      if (item) {
	for (int j=1;j<maxrows;j++) {      
	  codeTableWidget2->setFont(f, j, row);
	  codeTableWidget2->setBgColor(item->bg,j,row);
	}
      }
    }
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)




// ----------------------------------------------------------------------------------------------

void BinQGUI::updateByteItemList() {
  // update byteItemList
  cerr << "updating itemsFileA : " << itemsFileA.size() << endl; 
  for (unsigned int i=0;i<itemsFileA.size();++i) {
    Item* a = itemsFileA[i];
    if (a) {
      int pos = a->pos;
      int length = a->length;
      //      cerr << i << ": updating pos : " << pos << " length : " << length << endl;       
      for (int k=0; k<length;++k)
	byteItemFileA[pos+k]=a;
      rowItemFileA[i]=a;
    }
  }
  if (fileB) {
    cerr << "updating itemsFileB : " << itemsFileB.size() << endl; 
    for (unsigned int i=0;i<itemsFileB.size();++i) {
      Item* b = itemsFileB[i];
      if (b) {
	int pos = b->pos;
	int length = b->length;
	for (int k=0; k<length;++k)
	  byteItemFileB[pos+k]=b;
	rowItemFileB[i]=b;
      }
    }
  }
  slide->colorize();
  showFileA(0);
  if (fileB)
    showFileB(0);
  
  showFileTab();
}



void
BinQGUI::insertSectionInformation(SgAsmElfSection* sec, std::string filename) {
  ROSE_ASSERT(sec);
  sectionInfo->setText(filename.c_str());
  SgAsmElfFileHeader* elf = isSgAsmElfFileHeader(sec);
  if (elf) {
    sectionInfo->append(elf->format_name());
    string num = RoseBin_support::ToString(elf->get_e_machine());
    sectionInfo->append( QString("Machine: %1")
			 .arg(num.c_str()) );
    SgAsmElfSegmentTable *segments = elf->get_segment_table();
    string seg = RoseBin_support::ToString(segments);
    sectionInfo->append( QString("ElfSegmentTable: %1")
			 .arg(seg.c_str()) );
    SgAsmElfSectionTable *sections = elf->get_section_table();
    string sec = RoseBin_support::ToString(sections);
    sectionInfo->append( QString("ElfSectionTable: %1")
			 .arg(sec.c_str()) );
  } else {
    SgAsmElfSectionTableEntry* entry= sec->get_section_entry();
    if (entry) {
      rose_addr_t addr = entry->get_sh_addr();
      rose_addr_t size = entry->get_sh_size();
      rose_addr_t offset = entry->get_sh_offset();
      string addrS = RoseBin_support::HexToString(addr);
      string sizeS = RoseBin_support::HexToString(size);
      string offsetS = RoseBin_support::HexToString(offset);
      sectionInfo->append( QString("%1           type:     %2 Addr: %3  Size: %4   Offset: %5")
			   .arg(QString(sec->get_name()->get_string().c_str()))
			   .arg(sec->class_name().c_str())
			   .arg(addrS.c_str())
			   .arg(sizeS.c_str())
			   .arg(offsetS.c_str()));	 
    }
  }
  sectionInfo->append(QString("Append dump information here..."));
}

void
BinQGUI::insertFileInformation() {

  ROSE_ASSERT(isSgProject(fileA));

  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;

  SgAsmInterpretationPtrList& interps = file->get_interpretations();
  SgAsmInterpretationPtrList::iterator it = interps.begin();
  for (;it!=interps.end();++it) {
    SgAsmInterpretation* mainInt = *it;
    SgAsmGenericHeader* header = mainInt->get_header();
    fileInfo->append(header->class_name().c_str());
    SgAsmElfFileHeader* elf = isSgAsmElfFileHeader(header);
    if (elf) {
      fileInfo->append(file->get_name().c_str());
      fileInfo->append(elf->format_name());
      string num = RoseBin_support::ToString(elf->get_e_machine());
      fileInfo->append( QString("Machine: %1")
			.arg(num.c_str()) );
      SgAsmElfSegmentTable *segments = elf->get_segment_table();
      string seg = RoseBin_support::ToString(segments);
      fileInfo->append( QString("ElfSegmentTable: %1")
			.arg(seg.c_str()) );
      SgAsmElfSectionTable *sections = elf->get_section_table();
      string sec = RoseBin_support::ToString(sections);
      fileInfo->append( QString("ElfSectionTable: %1")
			.arg(sec.c_str()) );
    }
  }
  SgAsmGenericFile *genericF = file->get_genericFile() ;
  string size = RoseBin_support::ToString(genericF->get_orig_size());
  fileInfo->append( QString("Orig Size of AsmGenericFile: %1")
		    .arg(size.c_str()) );

  SgAsmGenericHeaderPtrList headers = genericF->get_headers()->get_headers();
  fileInfo->append(QString("HEADERS ------------- %1")
		   .arg(RoseBin_support::ToString(headers.size()).c_str()));	 
  for (unsigned int i=0;i<headers.size();++i) {
    SgAsmGenericHeader* h = headers[i];
    fileInfo->append(h->class_name().c_str());	 
    fileInfo->append(h->format_name());	 
  }

  SgAsmGenericSectionPtrList sectionsList = genericF->get_sections();
  fileInfo->append(QString("SECTIONS ------------- %1")
		   .arg(RoseBin_support::ToString(sectionsList.size()).c_str()));	 
  for (unsigned int i=0;i<sectionsList.size();++i) {
    SgAsmGenericSection* h = sectionsList[i];
    SgAsmElfSection* elfSec = isSgAsmElfSection(h);
    if (elfSec) {
      SgAsmElfSectionTableEntry* entry= elfSec->get_section_entry();
      if (entry) {
	rose_addr_t addr = entry->get_sh_addr();
	rose_addr_t size = entry->get_sh_size();
	rose_addr_t offset = entry->get_sh_offset();
	string addrS = RoseBin_support::HexToString(addr);
	string sizeS = RoseBin_support::HexToString(size);
	string offsetS = RoseBin_support::HexToString(offset);
	fileInfo->append( QString("%1           type:     %2 Addr: %3  Size: %4   Offset: %5")
			  .arg(QString(h->get_name()->get_string().c_str()))
			  .arg(h->class_name().c_str())
			  .arg(addrS.c_str())
			  .arg(sizeS.c_str())
			  .arg(offsetS.c_str()));	 
      }
    } else {
      fileInfo->append( QString("%1")
			.arg(h->class_name().c_str()));
    }
  }
  SgFileContentList fileList = genericF->get_data();
  fileInfo->append(QString("DATA ---------------- %1 ")
		   .arg(fileList.size()));	 
#if 0
  string data="";
  for (unsigned int i=0;i<fileList.size();++i) {
    data.append(RoseBin_support::ToString(fileList[i]));	
  }
  fileInfo->append( data.c_str());	 
#endif
  // doesnt work
  SgFileContentList contentList = genericF->content();
  fileInfo->append(QString("CONTENT ------------- %1 ")
		   .arg(contentList.size()));	 
#if 0
  string content="";
  for (unsigned int i=0;i<contentList.size();++i) {
    data += RoseBin_support::ToString(contentList[i]);	
  }
  fileInfo->append( content.c_str());	 
#endif

    fileInfo->moveCursor(QTextCursor::Start);
}


BinQGUI::BinQGUI(std::string fA, std::string fB, std::vector<std::string> dllAA, 
		 std::vector<std::string> dllBB, bool t) :  
  window(0), fileNameA(fA), fileNameB(fB) {
  test=t;
  if (test==false) {
    cerr << "Running in normal mode." << endl;
    window = new QRWindow( "mainWindow", QROSE::TopDown );
    binqsupport= new BinQSupport();
  } else {
    cerr << "Running in test mode." << endl;
  }
  maxrows=5;
  dllA=dllAA;
  dllB=dllBB;
  currentAnalysis=NULL;
  sourceFile=false;

  init();
  if (test==false)
    createGUI();
  cerr << "Initialization done." <<endl;
  if (test)
    testAnalyses();
}

void BinQGUI::init(){
  cerr << "Checking for analyses ... " << endl;
  analyses.clear();
  analyses.push_back(new DiffAlgo());
  analyses.push_back(new FunctionDiffAlgo());
  analyses.push_back(new AlignFunction());
  analyses.push_back(new BinCallGraph());
  analyses.push_back(new BinControlFlowAnalysis());
  analyses.push_back(new BinDataFlowAnalysis());
  analyses.push_back(new BufferOverflow());
  analyses.push_back(new InterruptAnalysis());
  analyses.push_back(new DynamicInfo());


  cerr << "Disassemble File A ... " << fileNameA << endl;
  std::string sourceFileS;
  fileA = binqsupport->disassembleFile(fileNameA, sourceFileS);
  ROSE_ASSERT(fileA);
  fileB=NULL;
  if (fileNameB!="") {
    cerr << "\nDisassemble File B ... " << fileNameB << endl;
    fileB = binqsupport->disassembleFile(fileNameB, sourceFileS);
    ROSE_ASSERT(fileB);
    if (sourceFileS=="true")
      sourceFile=true;
  }

  if (dllA.size()>0) {
    std::vector<std::string>::const_iterator nameIt = dllA.begin();
    for (;nameIt!=dllA.end();++nameIt) {
      string name = *nameIt;
      SgNode* node = binqsupport->disassembleFile(name, sourceFileS);
      dllFilesA.push_back(node);
    }
  }

  if (dllB.size()>0) {
    std::vector<std::string>::const_iterator nameIt = dllB.begin();
    for (;nameIt!=dllB.end();++nameIt) {
      string name = *nameIt;
      SgNode* node = binqsupport->disassembleFile(name, sourceFileS);
      dllFilesB.push_back(node);
    }
  }

  ROSE_ASSERT(isSgProject(fileA));
  backend(isSgProject(fileA));

  string filename="binary_tree.dot";
  AST_BIN_Traversal* trav = new AST_BIN_Traversal();
  trav->run(fileA, filename);

  // ------------------------------------------------------------
  // can be used with  -rose:read_executable_file_format_only
  //  generateDOTforMultipleFile ( *isSgProject(fileA) );
  // ------------------------------------------------------------
  
  // this part writes the file out to an assembly file -----------------------------------
  SgBinaryFile* binaryFileA = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file1 = binaryFileA != NULL ? binaryFileA->get_binaryFile() : NULL;
  SgAsmInterpretation* interpA = SageInterface::getMainInterpretation(file1);

  unparseAsmStatementToFile("unparsedA.s", interpA->get_global_block());

  if (fileNameB!="") 
    if(is_directory( fileNameB  ) == false && sourceFile==false) {
      SgBinaryFile* binaryFileB = isSgBinaryFile(isSgProject(fileB)->get_fileList()[0]);
      SgAsmFile* file2 = binaryFileB != NULL ? binaryFileB->get_binaryFile() : NULL;
      SgAsmInterpretation* interpB = SageInterface::getMainInterpretation(file2);
      unparseAsmStatementToFile("unparsedB.s", interpB->get_global_block());
    }
  // -------------------------------------------------------------------------------------

  itemsFileA.clear();
  if (fileB)
    itemsFileB.clear();
  
  // ---------------------- Create itemsFileA and itemsFileB , containing all statements
  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcsFileA ));
  if (fileB) {
    if (sourceFile) {
      FindSgFunctionsVisitor funcVisSource;
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVisSource, &funcsFileB ));
    } else
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVis, &funcsFileB ));
  }

  cerr << " File A has " << funcsFileA.size() << " funcs." << endl;
  if (fileB)
    cerr << " File B has " << funcsFileB.size() << " funcs." << endl;

  FindAsmStatementsHeaderVisitor visStat;
  std::vector<SgNode*> stmts;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat, &stmts ));
  vector<SgNode*>::iterator it= stmts.begin();
  int pos=0;
  int row=0;
  for (;it!=stmts.end();++it) {
    Item* item=NULL;
    int length=1;
    // make sure file 1 is a binary file -- source file only for file 2 allowed
    ROSE_ASSERT(isSgAsmNode(*it));
    if (isSgAsmFunctionDeclaration(*it)) {
      SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(*it);
      item = new Item(func->get_address(),func,2, row,length, 0, pos,"",0);
    } else if (isSgAsmBlock(*it)) {
      
    } else if (isSgAsmInstruction(*it)) {
      SgAsmInstruction* inst = isSgAsmInstruction(*it);
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      item = new Item(inst->get_address(),inst,0,row,length,length, pos,"",0);
    } else if (isSgAsmElfSection(*it)) {
      SgAsmElfSection* sec = isSgAsmElfSection(*it);
      std::string nam = "size: " + RoseBin_support::ToString(sec->get_size());
      item = new Item(sec->get_mapped_rva(),sec,2,row,length,sec->get_size(),pos,nam,0);
    } else if (isSgAsmElfSymbol(*it)) {
      SgAsmElfSymbol* sym = isSgAsmElfSymbol(*it);
      std::string nam = "size: " + RoseBin_support::ToString(sym->get_st_size());
      int color=6;
      item = new Item(sym->get_st_name(),sym,color,row,length,sym->get_st_size(),pos,nam,0);
    }  else if (isSgAsmElfSectionTableEntry(*it)) {
      SgAsmElfSectionTableEntry* sec = isSgAsmElfSectionTableEntry(*it);
      std::string nam = "size: " + RoseBin_support::ToString(sec->get_sh_size());
      int color=0;
      item = new Item(sec->get_sh_addr(),sec,color,row,length,sec->get_sh_size(),pos,nam,0);
    }  else if (isSgAsmElfSegmentTableEntry(*it)) {
      SgAsmElfSegmentTableEntry* sec = isSgAsmElfSegmentTableEntry(*it);
      std::string nam = "size: " + RoseBin_support::ToString(sec->get_offset());
      int color=0;
      item = new Item(sec->get_vaddr(),sec,color,row,length,length,pos,nam,0);
    } else {
      cerr << " >>> found pos : " << pos << "  item " << 
	isSgAsmNode(*it)->class_name() << " length : " << length << endl;

    }
    // color code call as an example
    if (isSgAsmx86Instruction(*it)) {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      if (isSgAsmx86Instruction(*it)->get_kind() == x86_call) { 
	SgAsmx86Instruction* inst = isSgAsmx86Instruction(*it);
	SgAsmOperandList * ops = inst->get_operandList();
	SgAsmExpressionPtrList& opsList = ops->get_operands();
	std::string addrDest="";
	SgAsmExpressionPtrList::iterator itOP = opsList.begin();
	for (;itOP!=opsList.end();++itOP) {
	  addrDest += unparseX86Expression(*itOP, false) ;
	}
	string s="<...>";
	for (unsigned int j=0; j<funcsFileA.size();++j) {
	  SgAsmFunctionDeclaration* f = isSgAsmFunctionDeclaration(funcsFileA[j]);
	  string addrF= RoseBin_support::HexToString(f->get_address());
	  addrF="0x"+addrF.substr(1,addrF.size());
	  if (addrF==addrDest) {
	    s="<"+f->get_name()+">";
	    break;
	  }
	}
	delete item;
	item = new Item(inst->get_address(),inst,3,row,length,length,pos,s,0);
      }
    }
    if (item) {
      row++;
      //      cerr << " creating pos : " << pos << "  item " << 
      //	isSgAsmNode(*it)->class_name() << " length : " << length << endl;
      itemsFileA.push_back(item);
      pos+=length;
    }
  }

  if (fileB) {
    stmts.clear();
    if (!sourceFile) {
      FindAsmStatementsHeaderVisitor visStat2;
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat2, &stmts ));
    } else {
      FindNodeVisitor visStat2;
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat2, &stmts ));
    }
    it= stmts.begin();
    pos=0;
    row=0;
    for (;it!=stmts.end();++it) {
      Item* item = NULL;
      int length=1;
      if (isSgAsmFunctionDeclaration(*it)){
	SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(*it);
	item = new Item(func->get_address(),func,2,row,length,0,pos,"",0);
      }    else if (isSgAsmBlock(*it)) {
	continue;
	//item = new Item(false,*it,0,1,row,0);
      } else if (isSgAsmInstruction(*it)) {
	SgAsmInstruction* inst = isSgAsmInstruction(*it);
	length = isSgAsmInstruction(*it)->get_raw_bytes().size();
	item = new Item(inst->get_address(),inst,0,row,length,length,pos,"",0);
      } else if (isSgFunctionDeclaration(*it)) {
	int color=4;
	SgFunctionDeclaration* func = isSgFunctionDeclaration(*it);
	SgFunctionDefinition* def = isSgFunctionDefinition(isSgFunctionDeclaration(*it)->get_definition());
	if (def)
	  color=2;
	if (isSgFunctionDeclaration(*it)->get_file_info()->isCompilerGenerated())
	  color=3;
	item = new Item(0,func,color,row,length,length,pos,"",0);
      } else if (isSgAsmElfSymbol(*it)) {
	SgAsmElfSymbol* sym = isSgAsmElfSymbol(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sym->get_st_size());
	int color=6;
	item = new Item(sym->get_st_name(),sym,color,row,length,sym->get_st_size(),pos,nam,0);
      }  else if (isSgAsmElfSection(*it)) {
	SgAsmElfSection* sec = isSgAsmElfSection(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sec->get_size());
	item = new Item(sec->get_mapped_rva(),sec,2,row,length,sec->get_size(),pos,nam,0);
      } else if (isSgAsmElfSectionTableEntry(*it)) {
	SgAsmElfSectionTableEntry* sec = isSgAsmElfSectionTableEntry(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sec->get_sh_size());
	int color=0;
	item = new Item(sec->get_sh_addr(),sec,color,row,length,sec->get_sh_size(),pos,nam,0);
      } else if (isSgAsmElfSegmentTableEntry(*it)) {
	SgAsmElfSegmentTableEntry* sec = isSgAsmElfSegmentTableEntry(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sec->get_offset());
	int color=0;
	item = new Item(sec->get_vaddr(),sec,color,row,length,sec->get_offset(),pos,nam,0);
      } else if (isSgLocatedNode(*it)) {
	Sg_File_Info* fi = isSgLocatedNode(*it)->get_file_info();
	int line = -1;
	if (fi) {
	  line = fi->get_line();
	  length = 1;
	}
	//      cerr << fi << " creating statement : " << isSgLocatedNode(*it)->class_name() << " ... comment " << line << endl;
	item = new Item(line,isSgLocatedNode(*it),0,row,length,length,pos,
			isSgLocatedNode(*it)->class_name(),line);
      } else {
	//      cerr << "unknown node " << endl;//*it->class_name() << endl;
	//      item = new Item(false,NULL,0,0,row,0,pos, " ",0);
      }
      //example -- color calls red
      if (isSgAsmx86Instruction(*it)) {
	length = isSgAsmInstruction(*it)->get_raw_bytes().size();
	if (isSgAsmx86Instruction(*it)->get_kind() == x86_call) {
	  SgAsmx86Instruction* inst = isSgAsmx86Instruction(*it);
	  SgAsmOperandList * ops = inst->get_operandList();
	  SgAsmExpressionPtrList& opsList = ops->get_operands();
	  std::string addrDest="";
	  SgAsmExpressionPtrList::iterator itOP = opsList.begin();
	  for (;itOP!=opsList.end();++itOP) {
	    addrDest += unparseX86Expression(*itOP, false) ;
	  }
	  string s="<...>";
	  for (unsigned int j=0; j<funcsFileB.size();++j) {
	    SgAsmFunctionDeclaration* f = isSgAsmFunctionDeclaration(funcsFileB[j]);
	    string addrF= RoseBin_support::HexToString(f->get_address());
	    addrF="0x"+addrF.substr(1,addrF.size());
	    if (addrF==addrDest) {
	      s="<"+f->get_name()+">";
	      break;
	    }
	  }
	  delete item;
	  item = new Item(inst->get_address(),inst,3,row,length,length,pos,s,0);

	}
      }
      if (item) {
	row++;
	itemsFileB.push_back(item);
	pos+=length;
      }
    }
  }

}

void BinQGUI::createGUI() {
  QDesktopWidget *desktop = QApplication::desktop();
  screenWidth = desktop->width()-10;
  screenHeight = desktop->height()-100;
  if (screenWidth>1450) screenWidth=1450;
  if (screenHeight>1250) screenHeight=1250;


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

  QRPanel &mainPanel = *window << *( new QRPanel(QROSE::TopDown, QROSE::UseSplitter) );
  {
    QRPanel &topPanels = mainPanel << *new QRPanel(QROSE::TopDown, QROSE::UseSplitter);
    {
      QGroupBox *topPanelLeft =  topPanels <<  new QGroupBox(("Binary File Analysis Information"));
      {
        QGridLayout *echoLayout =  new QGridLayout;
	bar = new QScrollBar(Qt::Horizontal);
	slide = new Slide(this, bar);
	ROSE_ASSERT(bar);
	//	cerr << " setting range to max : " << slide->maxX << endl;
	bar->setFocusPolicy(Qt::StrongFocus);
	//	bar->setRange(0,1);
	bar->setSingleStep(10);

	//	QScrollArea* area = new QScrollArea;
	//area->setBackgroundRole(QPalette::Dark);
	//area->setWidget(slide);
	echoLayout->addWidget(slide, 0, 0 );
	echoLayout->addWidget(bar);

        topPanelLeft->setLayout(echoLayout);

	// cant get this to work
	//QObject::connect(slide, SIGNAL(valueChanged(int)), bar, SLOT(setValue(int)));
        Slide::connect(bar, SIGNAL(valueChanged(int)), slide, SLOT(setValue(int)));


      }
      topPanelLeft->setFixedHeight(90);
      QRPanel &analysisPanel = topPanels << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	QRPanel &analysisPanelLeft = analysisPanel << *new QRPanel(QROSE::TopDown, QROSE::UseSplitter);
	{
	  // add analyses here
	  QTabWidget *qtabwidgetL =  analysisPanelLeft << new QTabWidget( );
	  listWidget = new QListWidget;
	  for (unsigned int i=0; i < analyses.size(); ++i){
	    new QListWidgetItem((analyses[i]->name().c_str()), listWidget);
	  }

	  QROSE::link(listWidget, 
		      SIGNAL(itemSelectionChanged()), 
		      &clicked1, this);
	  
	  qtabwidgetL->insertTab(0,listWidget,"Analyses");

	  analysisInfo = analysisPanelLeft << new QTextEdit;//new QREdit(QREdit::Box);
	  analysisInfo->setReadOnly(true);
	  
	}
	analysisPanelLeft.setFixedWidth(screenWidth/5 );
	analysisPanelLeft.setTileSize(80);
	//	analysisPanelLeft.setFixedHeight(170);

	QRPanel &analysisPanelRight = analysisPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
	{

	  // --------
	  analysisTab =  analysisPanelRight << new QTabWidget();
	  //	analysisPanelRight.setTileSize(50,50);

	  console = new QTextEdit;//new QREdit(QREdit::Box);
	  console->setReadOnly(true);
	  console->setText("Initializing GUI");

	  analysisResult = new QTextEdit;//new QREdit(QREdit::Box);
	  analysisResult->setReadOnly(true);

	  sectionInfo = new QTextEdit;//new QREdit(QREdit::Box);
	  sectionInfo->setReadOnly(true);

	  fileInfo = new QTextEdit;//new QREdit(QREdit::Box);
	  fileInfo->setReadOnly(true);
	  insertFileInformation();

	  analysisTab->insertTab(0,console,"Console");
	  analysisTab->insertTab(1,analysisResult,"Analysis Results");
	  analysisTab->insertTab(2,fileInfo,"File Info");
	  analysisTab->insertTab(3,sectionInfo,"Section Info");

	  QGroupBox *rightGraphics =  analysisPanelRight <<  new QGroupBox(("File Visualization"));
	  {
	    QGridLayout *echoLayout =  new QGridLayout;
	    bar2 = new QScrollBar(Qt::Horizontal);
	    bar3 = new QScrollBar(Qt::Vertical);
	    ROSE_ASSERT(bar2);
	    bar2->setFocusPolicy(Qt::StrongFocus);
	    bar2->setSingleStep(1);
	    ROSE_ASSERT(bar3);
	    bar3->setFocusPolicy(Qt::StrongFocus);
	    bar3->setSingleStep(1);
	    MyBars* mybars = new MyBars(this,bar2);
	    echoLayout->addWidget(mybars, 0, 0 );
	    echoLayout->addWidget(bar3,0,1);
	    echoLayout->addWidget(bar2);
	    rightGraphics->setLayout(echoLayout);
	    MyBars::connect(bar2, SIGNAL(valueChanged(int)), mybars, SLOT(setValue(int)));
	    MyBars::connect(bar3, SIGNAL(valueChanged(int)), mybars, SLOT(setValueY(int)));
	  }


	}
      }
      //      topPanels.setFixedHeight(300);
    }
      
    QRPanel &bottomPanel = mainPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
    {
      QRPanel &bottomPanelLeft = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	tableWidget = bottomPanelLeft << new QRTable( 1, "function" );
	QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivatedA, this);
	if (fileB) {
	  tableWidget2 = bottomPanelLeft << new QRTable( 1, "function" );
	  QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivatedB, this);
	} 
	//	bottomPanelLeft.setTileSize(20,20);
      }

      QRPanel &bottomPanelRight = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	codeTableWidget = bottomPanelRight << new QRTable( 8, "row","address","instr","operands","comment","pos","size","byte" );
	QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedA, this);
	if (fileB) {
	  if (sourceFile) {
	    codeTableWidget2 = bottomPanelRight << new QRTable( 5, "row","line","text","type","pos" );
	    maxrows=3;
	  } else {
	    codeTableWidget2 = bottomPanelRight << new QRTable( 8, "row","address","instr","operands","comment","pos","size","byte" );
	  }
	  QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedB, this);
	} else {
	  QTextEdit* graph =  bottomPanel << new QTextEdit;
	  graph->setReadOnly(true);
	  graph->append("The call or control flow graph should be shown here...");
	}
      }
      bottomPanelLeft.setFixedWidth(screenWidth/5 );
    } //mainPanel
    mainPanel.setTileSize(30);
  } //window 



  window->setGeometry(0,0,screenWidth,screenHeight);
  window->setTitle("BinQ");
  console->append("Initializing done.");
  QString res = QString("A: Total functions  %1.  Total statements: %2. ")
    .arg(funcsFileA.size())
    .arg(itemsFileA.size());
  console->append(res);  
  if (fileB) {
    QString res2 = QString("B: Total functions  %1.  Total statements: %2. ")
      .arg(funcsFileB.size())
      .arg(itemsFileB.size());
    console->append(res2);  
  }

  updateByteItemList();
  // ------------------------------------

} //BinQGUI::BinQGUI()


void BinQGUI::open() {
#if 0
  char buf[4096] = "\0";
  std::string database = QFileDialog::getOpenFileName( 0, "Open As", getcwd(buf, 4096), "ASCII (*.sql)").toStdString();
  if( database.empty() ) return;
  if(exists(database) == true)    {
    //codeWidget->setReadOnly(true);
    //codeWidget->setPlainText(QString("foobar\nBar\nFoobari3"));
  }
#endif
} //CompassGui::open()

void BinQGUI::reset() {
  init();
  updateByteItemList();
} //CompassGui::open()

void
BinQGUI::testAnalyses() {
  for (unsigned int i=0;i<analyses.size();++i) {
    cerr << " testing analysis : " << analyses[i]->getDescription() << endl;
    bool twoFiles = analyses[i]->twoFiles();
    if (twoFiles && fileB!=NULL || twoFiles==false) {
      currentAnalysis=analyses[i];
      if (currentAnalysis) 
	currentAnalysis->test(fileA,fileB);
    }
  } 
 
}


void
BinQGUI::run( ) {
  //  cerr << " calling run!! " << endl;
  if (analysisResult)
    analysisResult->clear();
  if (currentAnalysis) {
    currentAnalysis->run();
    analysisResult->moveCursor(QTextCursor::Start);
  }
}

void 
BinQGUI::showFileTab() {
  cerr << "creating FileTab " << endl; 

  QROSE::unlink(tableWidget, SIGNAL(activated(int, int, int, int)));
  while(tableWidget->rowCount()) 
    tableWidget->removeRow(0);
  tableWidget->setTextColor(QColor(0,0,255),0);
  if (fileB) {
    QROSE::unlink(tableWidget2, SIGNAL(activated(int, int, int, int)));
    while(tableWidget2->rowCount()) 
      tableWidget2->removeRow(0);
    tableWidget2->setTextColor(QColor(0,0,255),0);
  }


  for (size_t row = 0; row < funcsFileA.size(); ++row) {
    tableWidget->addRows(1);
    ROSE_ASSERT(isSgAsmFunctionDeclaration(funcsFileA[row]));
    tableWidget->setText(boost::lexical_cast<std::string>(isSgAsmFunctionDeclaration(funcsFileA[row])->get_name()), 0, row);
    //tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.size), 1, row);
    tableWidget->setVDim(row,18);
  }
  if (fileB) {
    for (size_t row = 0; row < funcsFileB.size(); ++row) {
      tableWidget2->addRows(1);
      if (isSgAsmFunctionDeclaration(funcsFileB[row]))
	tableWidget2->setText(boost::lexical_cast<std::string>(isSgAsmFunctionDeclaration(funcsFileB[row])->get_name()), 0, row);
      if (isSgFunctionDeclaration(funcsFileB[row])) {
	SgFunctionDeclaration* func = isSgFunctionDeclaration(funcsFileB[row]);
	if (func->get_file_info()->isCompilerGenerated())
	  tableWidget2->setTextColor(QColor(255,0,0),0,row);	
	else if (isSgFunctionDefinition(func->get_definition()))
	  tableWidget2->setTextColor(QColor(0,0,0),0,row);
	else
	  tableWidget2->setTextColor(QColor(128,128,128),0,row);
	//      cerr << row << " comp : " << func->get_file_info()->isCompilerGenerated() <<
	//	"   def : " << isSgFunctionDefinition(func->get_definition()) << endl;
	tableWidget2->setText(boost::lexical_cast<std::string>(isSgFunctionDeclaration(funcsFileB[row])->get_name().str()), 0, row);
      }
      tableWidget2->setVDim(row,18);
    }
  }
  tableWidget->setHAlignment(true, false, 0); // left horizontal alignment

  tableWidget->setHDim(0,140);
  tableWidget->setShowGrid(false);
  if (fileB) {
    tableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
    tableWidget2->setHDim(0,140);
    tableWidget2->setShowGrid(false);
  }
  
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivatedA, this);
  if (fileB)
    QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivatedB, this);
  
}


void BinQGUI::showFileA(int row) {
  cerr << "displaying A " << endl; 

  QROSE::unlink(codeTableWidget, SIGNAL(activated(int, int, int, int)));

  while(codeTableWidget->rowCount()) 
    codeTableWidget->removeRow(0);

  ROSE_ASSERT(fileA != NULL);


  ROSE_ASSERT(isSgAsmFunctionDeclaration(funcsFileA[row]));
  QString res = QString("FILE A : Looking at function  %1  row: %2  size ")
    .arg(isSgAsmFunctionDeclaration(funcsFileA[row])->get_name().c_str())
    .arg(row);
  //    .arg(elem.size);
  console->append(res);  
  
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFileA.size(); i++ )    {
    SgAsmNode* stmts = isSgAsmNode(itemsFileA[i]->statement);
    //    ROSE_ASSERT(stmts);
    int length=1;    
    bool addRow=false;
    //posRowA[posC]=-1;
    if (isSgAsmx86Instruction(stmts)) {
      codeTableWidget->addRows(1);
      length = isSgAsmInstruction(stmts)->get_raw_bytes().size();
      //      if (itemsFileA[i]->plus)
      //	itemsFileA[i]->bg=QColor(135,206,255);

      itemsFileA[i]->bg=QColor(255,255,255);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(0,0,0);
      QColor front = itemsFileA[i]->fg;

      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
      }
      codeTableWidget->setTextColor(front,0,i);
      codeTableWidget->setTextColor(QColor(255,0,0),1,i);
      codeTableWidget->setTextColor(QColor(0,0,255),2,i);
      codeTableWidget->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget->setTextColor(QColor(0,155,0),4,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_mnemonic() ), 2, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts)->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseX86Expression(*it, false) )+", ";
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(opsName), 3, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->comment ), 4, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->realByteSize), 7, i);	
      addRow=true;
    } else if (isSgAsmBlock(stmts)  && !(isSgAsmInterpretation(isSgAsmBlock(stmts)->get_parent()))) {
      //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      codeTableWidget->addRows(1);
      codeTableWidget->setTextColor(QColor(128,128,128),0,i);
      codeTableWidget->setTextColor(QColor(128,128,128),1,i);
      codeTableWidget->setTextColor(QColor(255,255,0),2,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts))->get_address()) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("***"), 2, i);
      addRow=true;
    }  else if (isSgAsmFunctionDeclaration(stmts)) {
      codeTableWidget->addRows(1);
      itemsFileA[i]->bg=QColor(0,0,0);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(255,255,255);
      QColor front = itemsFileA[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
	codeTableWidget->setTextColor(front,j,i);
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts))->get_address()) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("FUNC"), 2, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts))->get_name() ), 3, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->realByteSize), 7, i);	
      addRow=true;
    } else if (isSgAsmElfSection(stmts)) {
      codeTableWidget->addRows(1);
      itemsFileA[i]->bg=QColor(0,100,0);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(255,255,255);
      QColor front = itemsFileA[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
	codeTableWidget->setTextColor(front,j,i);
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileA[i]->addr) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSection(itemsFileA[i]->statement)->get_name()->get_string()), 2, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->comment ), 4, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->realByteSize), 7, i);	
      addRow=true;


    } else if (isSgAsmElfSectionTableEntry(stmts)) {
      codeTableWidget->addRows(1);
      itemsFileA[i]->bg=QColor(0,0,0);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(255,255,255);
      QColor front = itemsFileA[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
	codeTableWidget->setTextColor(front,j,i);
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileA[i]->addr) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSectionTableEntry(itemsFileA[i]->statement)->class_name()), 2, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->comment ), 4, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->realByteSize), 7, i);	
      addRow=true;


    } else if (isSgAsmElfSegmentTableEntry(stmts)) {
      codeTableWidget->addRows(1);
      itemsFileA[i]->bg=QColor(0,0,0);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(255,255,255);
      QColor front = itemsFileA[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
	codeTableWidget->setTextColor(front,j,i);
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileA[i]->addr) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSegmentTableEntry(itemsFileA[i]->statement)->class_name()), 2, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->comment ), 4, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->realByteSize), 7, i);	
      addRow=true;


    } else if (isSgAsmElfSymbol(stmts)) {
      codeTableWidget->addRows(1);
      itemsFileA[i]->bg=QColor(50,50,0);
      QColor back = itemsFileA[i]->bg;
      itemsFileA[i]->fg=QColor(255,255,255);
      QColor front = itemsFileA[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
	codeTableWidget->setTextColor(front,j,i);
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileA[i]->addr) ), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSymbol(itemsFileA[i]->statement)->get_name()->get_string()), 2, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->comment ), 4, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->realByteSize), 7, i);	
      addRow=true;


    } else {
      codeTableWidget->addRows(1);
      itemsFileA[i]->bg=QColor(128,128,128);
      QColor back = itemsFileA[i]->bg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget->setBgColor(back,j,i);
      }
      if (itemsFileA[i]->row) {
	codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->row), 0, i);	
	codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->pos), 5, i);	
	codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->length), 6, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>(itemsFileA[i]->realByteSize), 7, i);	
      }
      addRow=true;
    }


    // for testing
    if (rowC>2000)
      cerr << "Stopping here... currently table formatting allows only max 2000 entries..." <<endl;
      else
    if (addRow) {
      if ((rowC%500)==0)
	cout << " adding row ... " << rowC << " / " << itemsFileA.size() << endl;
      //this is slow!!
      //      codeTableWidget->setHAlignment(true, false, 3); // left horizontal alignment
#if 0      
      codeTableWidget->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 2); // left horizontal alignment

      codeTableWidget->setHAlignment(true, false, 4); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 5); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 6); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 7); // left horizontal alignment

#endif
      codeTableWidget->setVDim(i,18);
      codeTableWidget->setHDim(0,40);
      codeTableWidget->setHDim(1,65);
      codeTableWidget->setHDim(2,55);
      codeTableWidget->setHDim(3,180);
      codeTableWidget->setHDim(4,90);
      codeTableWidget->setHDim(5,40);
      codeTableWidget->setHDim(6,40);
      codeTableWidget->setHDim(7,40);

      
      rowC++;
      posC+=length;
      //cerr << "row added" << endl;
    }

  }

  codeTableWidget->setShowGrid(false);
  codeTableWidget->setCurrentCell(row,0);

  QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedA, this);

}

void BinQGUI::showFileB(int row) {
  cerr << "displaying B ... " << endl; 

  QROSE::unlink(codeTableWidget2, SIGNAL(activated(int, int, int, int)));

  while(codeTableWidget2->rowCount()) 
    codeTableWidget2->removeRow(0);

  ROSE_ASSERT(fileA != NULL);
  ROSE_ASSERT(fileB != NULL);
  std::string funcname ="";
  if (isSgAsmFunctionDeclaration(funcsFileB[row])) {
    funcname=isSgAsmFunctionDeclaration(funcsFileB[row])->get_name();
  }
  QString res = QString("FILE B : Looking at function  %1  row: %2  size ")
    .arg(funcname.c_str())
    .arg(row);
  //    .arg(elem.size);
  console->append(res);  
  //  std::cout << "Looking at function " << elem.function_name_B << "   row : " << row << "   size : " << elem.size << std::endl;
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFileB.size(); i++ )    {
    SgNode* stmts = itemsFileB[i]->statement;
    int length=1;    
    bool addRow=false;
    //posRowB[posC]=-1;
    if (isSgAsmx86Instruction(stmts)) {
      codeTableWidget2->addRows(1);
      length = isSgAsmInstruction(stmts)->get_raw_bytes().size();

      itemsFileB[i]->bg=QColor(255,255,255);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(0,0,0);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
      }
      codeTableWidget2->setTextColor(front,0,i);
      codeTableWidget2->setTextColor(QColor(255,0,0),1,i);
      codeTableWidget2->setTextColor(QColor(0,0,255),2,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),4,i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_mnemonic() ), 2, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts)->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseX86Expression(*it, false) )+", ";
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(opsName), 3, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->comment ), 4, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->realByteSize), 7, i);	
      addRow=true;
    } else if (isSgAsmBlock(stmts)  && !(isSgAsmInterpretation(isSgAsmBlock(stmts)->get_parent()))) {
      //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      codeTableWidget2->addRows(1);
      codeTableWidget2->setTextColor(QColor(128,128,128),0,i);
      codeTableWidget2->setTextColor(QColor(128,128,128),1,i);
      codeTableWidget2->setTextColor(QColor(255,255,0),2,i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts))->get_address()) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>("***"), 2, i);
      addRow=true;
    } else if (isSgAsmFunctionDeclaration(stmts)) {
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(0,0,0);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(255,255,255);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
	codeTableWidget2->setTextColor(front,j,i);
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts))->get_address()) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>("FUNC"), 2, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts))->get_name() ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->realByteSize), 7, i);	
      addRow=true;
    } 
    else if (isSgAsmElfSection(stmts)) {
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(0,100,0);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(255,255,255);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
	codeTableWidget2->setTextColor(front,j,i);
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileB[i]->addr) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(isSgAsmElfSection(itemsFileB[i]->statement)->get_name()->get_string()), 2, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->comment ), 4, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->realByteSize), 7, i);	
      addRow=true;


    }
    else if (isSgAsmElfSectionTableEntry(stmts)) {
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(0,0,0);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(255,255,255);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
	codeTableWidget2->setTextColor(front,j,i);
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileB[i]->addr) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(isSgAsmElfSectionTableEntry(itemsFileB[i]->statement)->class_name()), 2, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->comment ), 4, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->realByteSize), 7, i);	
      addRow=true;


    }
    else if (isSgAsmElfSegmentTableEntry(stmts)) {
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(0,0,0);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(255,255,255);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
	codeTableWidget2->setTextColor(front,j,i);
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileB[i]->addr) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(isSgAsmElfSegmentTableEntry(itemsFileB[i]->statement)->class_name()), 2, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->comment ), 4, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->realByteSize), 7, i);	
      addRow=true;


    }
    else if (isSgAsmElfSymbol(stmts)) {
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(50,50,0);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(255,255,255);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
	codeTableWidget2->setTextColor(front,j,i);
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFileB[i]->addr) ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(isSgAsmElfSymbol(itemsFileB[i]->statement)->get_name()->get_string()), 2, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->comment ), 4, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->realByteSize), 7, i);	
      addRow=true;


    }

    else if (isSgFunctionDeclaration(stmts)) {
      SgFunctionDeclaration* func = isSgFunctionDeclaration(stmts);
      //cerr << func->class_name() << "  maxrows: " << maxrows << endl;
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(0,0,0);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(255,255,255);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
	codeTableWidget2->setTextColor(front,j,i);
      }
      //      if (isSgFunctionDefinition(func->get_definition()))
      //	cerr << row << " " << func->unparseToString() << endl;

      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>("FUNC"), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(func->get_name().str() ), 2, i);
      std::string comment = func->get_file_info()->isCompilerGenerated() ? "compiler gen": " ";
      codeTableWidget2->setText(boost::lexical_cast<std::string>(comment ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 4, i);	
      addRow=true;

    } else if (isSgLocatedNode(stmts)) {
      SgLocatedNode* st = isSgLocatedNode(stmts);
      //cerr << st->class_name() << "  maxrows: " << maxrows << endl;
      codeTableWidget2->addRows(1);

      itemsFileB[i]->bg=QColor(255,255,255);
      QColor back = itemsFileB[i]->bg;
      itemsFileB[i]->fg=QColor(0,0,0);
      QColor front = itemsFileB[i]->fg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
      }
      codeTableWidget2->setTextColor(front,0,i);
      codeTableWidget2->setTextColor(QColor(255,0,0),1,i);
      codeTableWidget2->setTextColor(QColor(0,0,255),2,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),4,i);

      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->lineNr ), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(st->unparseToString()), 2, i);
      //int size = st->get_traversalSuccessorContainer().size();
      //cerr << size << " : " << st->unparseToString() << endl;
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->comment ), 3, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 4, i);	

      addRow=true;
    } 
    else {
      codeTableWidget2->addRows(1);
      itemsFileB[i]->bg=QColor(128,128,128);
      QColor back = itemsFileB[i]->bg;
      for (int j=1;j<maxrows;++j) {
	codeTableWidget2->setBgColor(back,j,i);
      }

      if (itemsFileB[i]->row) {
	codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->row), 0, i);	
	if (!sourceFile) {
	  codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->pos), 5, i);	
	  codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->length), 6, i);	
	  codeTableWidget2->setText(boost::lexical_cast<std::string>(itemsFileB[i]->realByteSize), 7, i);	
	}
      }
      addRow=true;
    }

    if (addRow) {
      //codeTableWidget2->setHAlignment(true, false, 3); // left horizontal alignment

#if 0
      codeTableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 2); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 4); // left horizontal alignment
#endif
      
      codeTableWidget2->setVDim(i,18);
      if (sourceFile) {
	codeTableWidget2->setHDim(0,30);
	codeTableWidget2->setHDim(1,50);
	codeTableWidget2->setHDim(2,300);
	codeTableWidget2->setHDim(3,110);
	codeTableWidget2->setHDim(4,40);
      } else {
#if 0
	codeTableWidget2->setHAlignment(true, false, 5); // left horizontal alignment
	codeTableWidget2->setHAlignment(true, false, 6); // left horizontal alignment
	codeTableWidget2->setHAlignment(true, false, 7); // left horizontal alignment
#endif
	codeTableWidget2->setHDim(0,40);
	codeTableWidget2->setHDim(1,65);
	codeTableWidget2->setHDim(2,55);
	codeTableWidget2->setHDim(3,180);
	codeTableWidget2->setHDim(4,90);
	codeTableWidget2->setHDim(5,40);
	codeTableWidget2->setHDim(6,40);
	codeTableWidget2->setHDim(7,40);
      }
      rowC++;
      posC+=length;
    }
  }
  codeTableWidget2->setShowGrid(false);
  codeTableWidget2->setCurrentCell(row,0);

  QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedB, this);
}


BinQGUI::~BinQGUI(){
  if( window ) delete window;
} //BinQGUI::~BinQGUI()



