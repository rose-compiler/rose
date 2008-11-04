// tps: the following line causes the frontend to misbehave!!!
// that is totally weird!
//#include <boost/algorithm/string.hpp>
#include "BinQGui.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "icons.h"
#include "folder.xpm"

#include "BinQSupport.h"
#include "slide.h"

#include <qtabwidget.h>
#include "Clone.h"

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;


// ----------------------------------------------------------------------------------------------
// specify analyses here!
void alignFunctions();
void andreasDiff();
void alignFunctionsSmart();

void clicked1() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  QListWidgetItem *item= instance->listWidget->currentItem(); 
  QString te = item->text();
  string t = te.toStdString();
  //const char *t = (const char *)text;
  cerr << " Selected : " << t << endl;

  if (t=="Andreas's Diff") {
    andreasDiff();
  } // andreas diff
  else if (t=="Align Functions") {
    alignFunctions();
  }
  else if (t=="Align Functions Smart") {
    alignFunctionsSmart();
  }

} 





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
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)




// ----------------------------------------------------------------------------------------------


void BinQGUI::highlightFunctionRow(int row, bool fileA) {
  if(row >= 0)    {         
    if (fileA) {
      QFont f = tableWidget->getFont(0, row);
      f.setBold(true);
      tableWidget->setFont(f, 0, row);
      tableWidget->setBgColor(QColor(255,255,0),0,row);
      //showFileA(row);
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
    } else {
      QFont f = tableWidget2->getFont(0, row);
      f.setBold(true);
      tableWidget2->setFont(f, 0, row);
      tableWidget2->setBgColor(QColor(255,255,0),0,row);
      //      showFileB(row);
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

void BinQGUI::unhighlightFunctionRow(int row, bool fileA) {
  if (row >= 0)     {
    if (fileA) {
      QFont f = tableWidget->getFont(0, row);
      f.setBold(false);
      tableWidget->setFont(f, 0, row);
      tableWidget->setBgColor(QColor(255,255,255),0,row);
    } else {
      QFont f = tableWidget2->getFont(0, row);
      f.setBold(false);
      tableWidget2->setFont(f, 0, row);
      tableWidget2->setBgColor(QColor(255,255,255),0,row);
    }
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)




void BinQGUI::highlightInstructionRow(int row, bool fileA) {
  if(row >= 0)    {         
    if (fileA) {
      QFont f = codeTableWidget->getFont(0, row);
      f.setBold(true);
      codeTableWidget->setCurrentCell(row,0);
      Item* item = itemsFileA[row];
      for (int j=1;j<maxrows;j++) {
	codeTableWidget->setFont(f, j, row);
	if (item->function) 
	  codeTableWidget->setBgColor(QColor(120,120,120),j,row);
	else 
	  codeTableWidget->setBgColor(QColor(255,255,0),j,row);
      }
    } else {
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(true);
      codeTableWidget2->setCurrentCell(row,0);
      Item* item = itemsFileB[row];
      for (int j=1;j<maxrows;j++) {
	codeTableWidget2->setFont(f, j, row);
	if (item->function) 
	  codeTableWidget2->setBgColor(QColor(120,120,120),j,row);
	else
	  codeTableWidget2->setBgColor(QColor(255,255,0),j,row);
      }
    }
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightInstructionRow(int row,bool fileA) {
  if (row >= 0)     {
    if (fileA) {
      QFont f = codeTableWidget->getFont(0, row);
      f.setBold(false);
      Item* item = itemsFileA[row];
      for (int j=1;j<maxrows;j++) {      
	codeTableWidget->setFont(f, j, row);
	codeTableWidget->setBgColor(item->bg,j,row);
      }
    } else {
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(false);
      Item* item = itemsFileB[row];
      for (int j=1;j<maxrows;j++) {      
	codeTableWidget2->setFont(f, j, row);
	codeTableWidget2->setBgColor(item->bg,j,row);
      }
    }
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)




// ----------------------------------------------------------------------------------------------

void BinQGUI::updateByteItemList() {
  // update byteItemList
  for (unsigned int i=0;i<itemsFileA.size();++i) {
    Item* a = itemsFileA[i];
    int pos = a->pos;
    int length = a->length;
    for (int k=0; k<length;++k)
      byteItemFileA[pos+k]=a;
  }
  for (unsigned int i=0;i<itemsFileB.size();++i) {
    Item* b = itemsFileB[i];
    int pos = b->pos;
    int length = b->length;
    for (int k=0; k<length;++k)
      byteItemFileB[pos+k]=b;
  }
  slide->colorize();
  showFileA(0);
  showFileB(0);
  //  codeTableWidget->viewport()->update();
  //codeTableWidget2->viewport()->update();

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
	string addrS = RoseBin_support::HexToString(addr);
	string sizeS = RoseBin_support::HexToString(size);
	fileInfo->append( QString("%1 Addr: %2  Size: %3")
			  .arg(h->class_name().c_str())
			  .arg(addrS.c_str())
			  .arg(sizeS.c_str()));	 
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


}


BinQGUI::BinQGUI(std::string fA, std::string fB ) :  window(0), 
						     fileNameA(fA), fileNameB(fB) {
  window = new QRWindow( "mainWindow", QROSE::TopDown );
  binqsupport= new BinQSupport();
  maxrows=5;
  sourceFile=false;
  init();
  createGUI();
}

void BinQGUI::init(){
  cerr << "Disassemble File A ... " << fileNameA << endl;
  std::string sourceFileS;
  fileA = binqsupport->disassembleFile(fileNameA, sourceFileS);
  cerr << "\nDisassemble File B ... " << fileNameB << endl;
  fileB = binqsupport->disassembleFile(fileNameB, sourceFileS);
  ROSE_ASSERT(fileA);
  ROSE_ASSERT(fileB);
  if (sourceFileS=="true")
    sourceFile=true;

  // this part writes the file out to an assembly file -----------------------------------
  SgBinaryFile* binaryFileA = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file1 = binaryFileA != NULL ? binaryFileA->get_binaryFile() : NULL;
  SgAsmInterpretation* interpA = SageInterface::getMainInterpretation(file1);

  unparseAsmStatementToFile("unparsedA.s", interpA->get_global_block());

  if(is_directory( fileNameB  ) == false && sourceFile==false) {
    SgBinaryFile* binaryFileB = isSgBinaryFile(isSgProject(fileB)->get_fileList()[0]);
    SgAsmFile* file2 = binaryFileB != NULL ? binaryFileB->get_binaryFile() : NULL;
    SgAsmInterpretation* interpB = SageInterface::getMainInterpretation(file2);
    unparseAsmStatementToFile("unparsedB.s", interpB->get_global_block());
  }
  // -------------------------------------------------------------------------------------

  itemsFileA.clear();
  itemsFileB.clear();
  
  // ---------------------- Create itemsFileA and itemsFileB , containing all statements
  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcsFileA ));
  if (sourceFile) {
    FindSgFunctionsVisitor funcVisSource;
    AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVisSource, &funcsFileB ));
  } else
    AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVis, &funcsFileB ));

  cerr << " File A has " << funcsFileA.size() << " funcs." << endl;
  cerr << " File B has " << funcsFileB.size() << " funcs." << endl;

  FindAsmStatementsVisitor visStat;
  std::vector<SgNode*> stmts;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat, &stmts ));
  vector<SgNode*>::iterator it= stmts.begin();
  int pos=0;
  int row=0;
  for (;it!=stmts.end();++it) {
    Item* item;
    int length=1;
    // make sure file 1 is a binary file -- source file only for file 2 allowed
    ROSE_ASSERT(isSgAsmStatement(*it));
    if (isSgAsmFunctionDeclaration(*it)) {
      FindAsmStatementsVisitor visStat2;
      std::vector<SgAsmStatement*> stmts2;
      AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat2, &stmts2 ));
      int funcsize= stmts2.size();
      item = new Item(true,isSgAsmFunctionDeclaration(*it),funcsize,2,row,length, pos,"",0);
    } else if (isSgAsmBlock(*it)) {
      continue;
      //item = new Item(false,*it,0,1,row,0);
    } else if (isSgAsmInstruction(*it)) {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      item = new Item(false,isSgAsmInstruction(*it),0,0,row,length,pos,"",0);
    }
    // color code pushes as an example
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
	item = new Item(false,isSgAsmx86Instruction(*it),0,3,row,length,pos,s,0);
      }
    }
    row++;
    itemsFileA.push_back(item);
    pos+=length;
  }


  stmts.clear();
  if (!sourceFile) {
    FindAsmStatementsVisitor visStat2;
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
      int funcsize=0;
      FindAsmStatementsVisitor visStat2;
      std::vector<SgAsmStatement*> stmts2;
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat2, &stmts2 ));
      funcsize= stmts2.size();
      item = new Item(true,isSgAsmFunctionDeclaration(*it),funcsize,2,row,length,pos,"",0);
    }    else if (isSgAsmBlock(*it)) {
      continue;
      //item = new Item(false,*it,0,1,row,0);
    } else if (isSgAsmInstruction(*it)) {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      item = new Item(false,isSgAsmInstruction(*it),0,0,row,length,pos,"",0);
    } else if (isSgFunctionDeclaration(*it)) {
      int funcsize=0;
      FindNodeVisitor visStat2;
      std::vector<SgLocatedNode*> stmts2;
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat2, &stmts2 ));
      funcsize= stmts2.size();
      int color=4;
      SgFunctionDefinition* def = isSgFunctionDefinition(isSgFunctionDeclaration(*it)->get_definition());
      if (def)
	color=2;
      if (isSgFunctionDeclaration(*it)->get_file_info()->isCompilerGenerated())
	color=3;
      item = new Item(true,isSgFunctionDeclaration(*it),funcsize,color,row,length,pos,"",0);
    } else if (isSgLocatedNode(*it)) {
      Sg_File_Info* fi = isSgLocatedNode(*it)->get_file_info();
      int line = -1;
      if (fi) {
	line = fi->get_line();
	length = 1;
      }
      //      cerr << fi << " creating statement : " << isSgLocatedNode(*it)->class_name() << " ... comment " << line << endl;
      item = new Item(false,isSgLocatedNode(*it),0,0,row,length,pos,
		      isSgLocatedNode(*it)->class_name(),line);
    } else {
      cerr << "unknown node" << endl;//*it->class_name() << endl;
      item = new Item(false,NULL,0,0,row,0,pos,
		      " ",0);
    }
    //example -- color pushes red
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
	item = new Item(false,isSgAsmx86Instruction(*it),0,3,row,length,pos,s,0);

      }
    }
    row++;
    itemsFileB.push_back(item);
    pos+=length;
  }

}

  void BinQGUI::createGUI() {
    QDesktopWidget *desktop = QApplication::desktop();


  screenWidth = desktop->width()-50;
  screenHeight = desktop->height()-150;
  if (screenWidth>1424) screenWidth=1424;
  if (screenHeight>1224) screenHeight=1224;


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
      QGroupBox *selectGroup =  topPanels <<  new QGroupBox(("Binary File Analysis Information"));
      {
        QGridLayout *echoLayout =  new QGridLayout;
	slide = new Slide(this);
	echoLayout->addWidget(slide, 0, 0 );
        selectGroup->setLayout(echoLayout);

      }
      selectGroup->setFixedHeight(70);
      QRPanel &analysisPanel = topPanels << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	// add analyses here
	QTabWidget *qtabwidgetL =  analysisPanel << new QTabWidget( );
	listWidget = new QListWidget;
	new QListWidgetItem(("Align Functions"), listWidget);
	new QListWidgetItem(("Align Functions Smart"), listWidget);
	new QListWidgetItem(("Andreas's Diff"), listWidget);

	QROSE::link(listWidget, 
		    SIGNAL(itemSelectionChanged()), 
		    &clicked1, this);

	qtabwidgetL->insertTab(0,listWidget,"Analyses");
	
	QTabWidget *qtabwidget =  analysisPanel << new QTabWidget( );
	analysisPanel.setTileSize(50,50);

	analysisResult = new QTextEdit;//new QREdit(QREdit::Box);
	analysisResult->setReadOnly(true);
	analysisResult->setText("Initializing GUI");

	fileInfo = new QTextEdit;//new QREdit(QREdit::Box);
	fileInfo->setReadOnly(true);
	insertFileInformation();

	qtabwidget->insertTab(0,analysisResult,"Analysis Results");
	qtabwidget->insertTab(1,fileInfo,"File Info");
      }
      //      topPanels.setFixedHeight(300);
    }
      
    QRPanel &bottomPanel = mainPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
    {
      QRPanel &bottomPanelLeft = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	tableWidget = bottomPanelLeft << new QRTable( 1, "function" );
	QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivatedA, this);
	tableWidget2 = bottomPanelLeft << new QRTable( 1, "function" );
	QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivatedB, this);

	//	bottomPanelLeft.setTileSize(20,20);
      }

      QRPanel &bottomPanelRight = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	codeTableWidget = bottomPanelRight << new QRTable( 7, "row","address","instr","operands","comment","pos","byte" );
	QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedA, this);
	if (sourceFile) {
	  codeTableWidget2 = bottomPanelRight << new QRTable( 5, "row","line","text","type","pos" );
	  maxrows=3;
	} else {
	  codeTableWidget2 = bottomPanelRight << new QRTable( 7, "row","address","instr","operands","comment","pos","byte" );
	}
	QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedB, this);

      }
      bottomPanelLeft.setFixedWidth(screenWidth/5 );
    } //mainPanel
    mainPanel.setTileSize(30);
  } //window 



  window->setGeometry(0,0,screenWidth,screenHeight);
  window->setTitle("BinQ");
  analysisResult->append("Initializing done.");
  QString res = QString("A: Total functions  %1.  Total statements: %2. ")
    .arg(funcsFileA.size())
    .arg(itemsFileA.size());
  analysisResult->append(res);  
  QString res2 = QString("B: Total functions  %1.  Total statements: %2. ")
    .arg(funcsFileB.size())
    .arg(itemsFileB.size());
  analysisResult->append(res2);  


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
BinQGUI::run( ) {
  QROSE::unlink(tableWidget, SIGNAL(activated(int, int, int, int)));
  QROSE::unlink(tableWidget2, SIGNAL(activated(int, int, int, int)));
  while(tableWidget->rowCount()) 
    tableWidget->removeRow(0);
  while(tableWidget2->rowCount()) 
    tableWidget2->removeRow(0);

  tableWidget->setTextColor(QColor(0,0,255),0);
  tableWidget2->setTextColor(QColor(0,0,255),0);
  for (size_t row = 0; row < funcsFileA.size(); ++row) {
    tableWidget->addRows(1);
    ROSE_ASSERT(isSgAsmFunctionDeclaration(funcsFileA[row]));
    tableWidget->setText(boost::lexical_cast<std::string>(isSgAsmFunctionDeclaration(funcsFileA[row])->get_name()), 0, row);
    //tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.size), 1, row);
    tableWidget->setVDim(row,18);
  }
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
  tableWidget->setHAlignment(true, false, 0); // left horizontal alignment

  tableWidget->setHDim(0,140);
  tableWidget->setShowGrid(false);
  tableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
  tableWidget2->setHDim(0,140);
  tableWidget2->setShowGrid(false);

  
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivatedA, this);
  QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivatedB, this);
    showFileA(0);
  showFileB(0);
}


void BinQGUI::showFileA(int row) {
  QROSE::unlink(codeTableWidget, SIGNAL(activated(int, int, int, int)));

  while(codeTableWidget->rowCount()) 
    codeTableWidget->removeRow(0);

  ROSE_ASSERT(fileA != NULL);
  ROSE_ASSERT(fileB != NULL);

  ROSE_ASSERT(isSgAsmFunctionDeclaration(funcsFileA[row]));
  QString res = QString("FILE A : Looking at function  %1  row: %2  size ")
    .arg(isSgAsmFunctionDeclaration(funcsFileA[row])->get_name().c_str())
    .arg(row);
  //    .arg(elem.size);
  analysisResult->append(res);  
  //  std::cout << "Looking at function " << elem.function_name_B << "   row : " << row << "   size : " << elem.size << std::endl;
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFileA.size(); i++ )    {
    SgAsmStatement* stmts = isSgAsmStatement(itemsFileA[i]->statement);
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
      }
      addRow=true;
    }
    if (addRow) {
      codeTableWidget->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 2); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 3); // left horizontal alignment
      
      codeTableWidget->setVDim(i,18);
      codeTableWidget->setHDim(0,30);
      codeTableWidget->setHDim(1,80);
      codeTableWidget->setHDim(2,50);
      codeTableWidget->setHDim(3,190);
      codeTableWidget->setHDim(4,110);
      codeTableWidget->setHDim(5,30);
      codeTableWidget->setHDim(6,30);
      
      //posRowA[posC]=rowC;
      //      cerr << "added at pos:" << posC << "  rowC:" << rowC<<endl;
      rowC++;
      posC+=length;
    }
  }

  codeTableWidget->setShowGrid(false);
  codeTableWidget->setCurrentCell(row,0);

  QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedA, this);

}

void BinQGUI::showFileB(int row) {
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
  analysisResult->append(res);  
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
      //      cerr << itemsFileB[i]->statement->class_name() << endl;
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
	  }
	}
      addRow=true;
    }

    if (addRow) {
      codeTableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 2); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 3); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 4); // left horizontal alignment
      
      codeTableWidget2->setVDim(i,18);
      if (sourceFile) {
	codeTableWidget2->setHDim(0,30);
	codeTableWidget2->setHDim(1,50);
	codeTableWidget2->setHDim(2,300);
	codeTableWidget2->setHDim(3,110);
	codeTableWidget2->setHDim(4,40);
      } else {
	codeTableWidget2->setHDim(0,30);
	codeTableWidget2->setHDim(1,80);
	codeTableWidget2->setHDim(2,50);
	codeTableWidget2->setHDim(3,190);
	codeTableWidget2->setHDim(4,110);
	codeTableWidget2->setHDim(5,30);
	codeTableWidget2->setHDim(6,30);
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



