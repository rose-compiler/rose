#include "rose.h"
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


using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;


// ----------------------------------------------------------------------------------------------
// This function is called when an analysis is selected and run
void clicked1() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  QListWidgetItem *item= instance->listWidget->currentItem(); 
  QString te = item->text();
  string t = te.toStdString();
  //cerr << " Selected : " << t << endl;
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

// ----------------------------------------------------------------------------------------------
// this function handles the toolbar
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


// this function handles the function-table for fileA
static void tableWidgetCellActivated(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  QWidget* wid = QApplication::focusWidget();
  qrs::QRTable* widget = dynamic_cast<qrs::QRTable*>(wid);
  if (widget==NULL)
    return;
  //  cerr << " FUNCTION WIDGET CLICKED " << widget << endl;
  instance->currentTableWidget=widget;
  instance->unhighlightFunctionRow(oldRow, widget);
  instance->highlightFunctionRow(row, widget);
  return;
} 



// this function handles the instruction-table for fileA
static void codeTableWidgetCellActivated(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  QWidget* wid = QApplication::focusWidget();
  qrs::QRTable* widget = dynamic_cast<qrs::QRTable*>(wid);
  if (widget==NULL)
    return;
  //cerr << " INSTRUCTION WIDGET CLICKED " << widget << endl;
  if (instance->currentTableWidget==widget) {
    //    cerr << "Referred from Table widget ... ignore " <<endl;
    return;
  }
  instance->unhighlightInstructionRow(oldRow, widget);
  instance->highlightInstructionRow(row,  widget);
  instance->currentSelectedFile=instance->getFileForWidget[widget];

  Item* item = NULL;
  string filename = "unknown";
  if (instance->currentSelectedFile==instance->fileA) {
    item = instance->rowItemFileA[row];
    filename = instance->fileNameA;
  }
  if (instance->currentSelectedFile==instance->fileB) {
    item = instance->rowItemFileB[row];
    filename = instance->fileNameB;
  }
  if (item) {
    QString res = QString("%1: Item selected  %2 : %3.")
      .arg(QString(filename.c_str()))
      .arg(item->row)
      .arg(isSgNode(item->statement)->class_name().c_str());
    instance->console->append(res);  
    SgAsmElfSection* sec = isSgAsmElfSection(item->statement);
    if (sec) {
      instance->analysisTab->setCurrentIndex(3);
      instance->insertSectionInformation(sec,filename);      
    }
  }
  return;
} 


// ----------------------------------------------------------------------------------------------
// this function controls what happens when an item in the function table is selected (A or B)
void BinQGUI::highlightFunctionRow(int row, qrs::QRTable* widget) {
  if(row >= 0)    {         
    QFont f = widget->getFont(0, row);
    f.setBold(true);
    widget->setFont(f, 0, row);
    widget->setBgColor(QColor(255,255,0),0,row);
    bool fileA = true;
    if (widget==tableWidget2)
      fileA=false;
    if (fileA) {
      //cerr << "Function Table A selected : " <<  endl;
      if (isSgAsmFunctionDeclaration(funcsFileA[row])) {
	SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(funcsFileA[row]);
	std::vector<Item*>::iterator it = itemsFileA.begin();
	int offset=0;
	for (;it!=itemsFileA.end();++it) {
	  Item* item = *it;
	  SgAsmStatement* stat = isSgAsmStatement(item->statement);
	  if (func==isSgAsmFunctionDeclaration(stat)) {
	    offset=item->row;
	    break;
	  }
	}
	//cerr << " highlight func row : " << row << "  inst row : " << offset << endl;
	ROSE_ASSERT(codeTableWidget);
	codeTableWidget->setCurrentCell(offset,0);
      } 
    }
    else {
      //cerr << "Function Table B selected" << endl;
      if (fileB) {
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
    }
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

// this function controls what happens when an item in the function table is de-selected (A or B)
void BinQGUI::unhighlightFunctionRow(int row, qrs::QRTable* widget) {
  if (row >= 0)     {
    QFont f = widget->getFont(0, row);
    f.setBold(false);
    widget->setFont(f, 0, row);
    widget->setBgColor(QColor(255,255,255),0,row);
  } //if (row >= 0)
} 




// this function controls what happens when an item in the instruction table is selected (A or B)
void BinQGUI::highlightInstructionRow(int row, qrs::QRTable* widget) {
  if(row >= 0)    {         
    QFont f = widget->getFont(0, row);
    f.setBold(true);
    widget->setCurrentCell(row,0);
    Item* item = NULL;
    if (currentSelectedFile==fileA) {
      item = rowItemFileA[row];
    }
    if (currentSelectedFile==fileB) {
      item = rowItemFileB[row];
    }
    if (item) {
      for (int j=1;j<maxrows;j++) {
	widget->setFont(f, j, row);
	if (isSgAsmFunctionDeclaration(item->statement) ||
	    isSgFunctionDeclaration(item->statement)) 
	  widget->setBgColor(QColor(120,120,120),j,row);
	else 
	  widget->setBgColor(QColor(255,255,0),j,row);
      } 
    } else {
     // This is a DLL
      for (int j=1;j<maxrows;j++) {
	widget->setFont(f, j, row);
	widget->setBgColor(QColor(255,255,0),j,row);
      } 
    }
  } //if(row >= 0)
} 

// this function controls what happens when an item in the instruction table is de-selected (A or B)
void BinQGUI::unhighlightInstructionRow(int row,qrs::QRTable* widget) {
  if (row >= 0)     {
    QFont f = widget->getFont(0, row);
    f.setBold(false);
    Item* item = NULL;
    if (currentSelectedFile==fileA) {
      item = rowItemFileA[row];
    }
    if (currentSelectedFile==fileB) {
      item = rowItemFileB[row];
    }
    if (item) {
      for (int j=1;j<maxrows;j++) {      
	widget->setFont(f, j, row);
	widget->setBgColor(item->bg,j,row);
      }
    } else {
      // This is a DLL
      for (int j=1;j<maxrows;j++) {      
	widget->setFont(f, j, row);
	widget->setBgColor(QColor(0,0,0),j,row);
      }
    }
  } //if (row >= 0)
} 


// ----------------------------------------------------------------------------------------------
// this function manages the code to keep the slide bar update with the instruction table
void BinQGUI::updateByteItemList() {
  // update byteItemList
  //cerr << "updating itemsFileA : " << RoseBin_support::ToString(itemsFileA.size()) << endl; 
  for (unsigned int i=0;i<itemsFileA.size();++i) {
    Item* a = itemsFileA[i];
    if (a) {
      int pos = a->pos;
      int length = a->length;
      //  cerr << i << ": updating pos : " << pos << " length : " << length << endl;       
      for (int k=0; k<length;++k)
	byteItemFileA[pos+k]=a;
      rowItemFileA[i]=a;
    }
  }
  if (fileB) {
    //cerr << "updating itemsFileB : " << RoseBin_support::ToString(itemsFileB.size()) << endl; 
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
  //cerr << "displaying A  ... " << endl; 
  showFile(0, codeTableWidget, funcsFileA, itemsFileA);
  getFileForWidget[codeTableWidget]=fileA;
  QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivated, this);
  if (fileB) {
    //cerr << "displaying B ... " << endl; 
    showFile(0, codeTableWidget2, funcsFileB, itemsFileB);
    getFileForWidget[codeTableWidget2]=fileB;
    QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivated, this);
  }
  showFileTab();


  //cerr << ">> Handling DLLs ... " << endl; 
  //handle DLLs
#if 1
  std::vector<SgNode*>::const_iterator dllIt = dllFilesA.begin();
  std::vector<qrs::QRTable*>::const_iterator widgetIt = codeTableWidgetADLLlist.begin();
  ROSE_ASSERT(dllFilesA.size()==codeTableWidgetADLLlist.size());
  for (;dllIt!=dllFilesA.end();++dllIt,++widgetIt) {
    SgNode* file = *dllIt;
    qrs::QRTable* codeTableWidgetDLL = *widgetIt ;
    ROSE_ASSERT(file);
    ROSE_ASSERT(codeTableWidgetDLL);
    std::vector<SgNode*> funcsFile;
    std::vector<Item*> itemsFile;
    createFunction(file, funcsFile, true);
    createItem(file,itemsFile, funcsFile, true);
    //cerr << " Adding DLL of FileA---  funcsFileSize: " << RoseBin_support::ToString(funcsFile.size()) << "  itemsFileSize: " << RoseBin_support::ToString(itemsFile.size()) << endl;
    showFile(0, codeTableWidgetDLL, funcsFile, itemsFile);
    getFileForWidget[codeTableWidgetDLL]=file;
    QROSE::link(codeTableWidgetDLL, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivated, this);
  }
  dllIt = dllFilesB.begin();
  widgetIt = codeTableWidgetBDLLlist.begin();
  ROSE_ASSERT(dllFilesB.size()==codeTableWidgetBDLLlist.size());
  for (;dllIt!=dllFilesB.end();++dllIt,++widgetIt) {
    SgNode* file = *dllIt;
    qrs::QRTable* codeTableWidgetDLL = *widgetIt ;
    ROSE_ASSERT(file);
    ROSE_ASSERT(codeTableWidgetDLL);
    std::vector<SgNode*> funcsFile;
    std::vector<Item*> itemsFile;
    createFunction(file, funcsFile, true);
    createItem(file,itemsFile, funcsFile, true);
    //cerr << " Adding DLL of FileB---  funcsFileSize: " << RoseBin_support::ToString(funcsFile.size()) << "  itemsFileSize: " << RoseBin_support::ToString(itemsFile.size()) << endl;
    showFile(0, codeTableWidgetDLL, funcsFile, itemsFile);
    getFileForWidget[codeTableWidgetDLL]=file;
    QROSE::link(codeTableWidgetDLL, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivated, this);
  }

#endif
}


// this function inserts a tab and reports information about a selected section
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

// this function inserts a tab with information about the file(A).
// this should be replaced in the future against the file.dump function
void
BinQGUI::insertFileInformation() {

  ROSE_ASSERT(isSgProject(fileA));

  SgBinaryComposite* binary = isSgBinaryComposite(isSgProject(fileA)->get_fileList()[0]);
  SgAsmGenericFile* file = binary != NULL ? binary->get_binaryFile() : NULL;

  SgAsmInterpretationPtrList& interps = binary->get_interpretations()->get_interpretations();
  SgAsmInterpretationPtrList::iterator it = interps.begin();
  for (;it!=interps.end();++it) {
    SgAsmInterpretation* mainInt = *it;
    ROSE_ASSERT(mainInt->get_headers()->get_headers().size()==1);
    SgAsmGenericHeader* header = mainInt->get_headers()->get_headers()[0];
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
  SgAsmGenericFile *genericF = file;
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

// Constructor
//BinQGUI::BinQGUI(std::string fA, std::string fB, std::vector<std::string> dllAA, 
//		 std::vector<std::string> dllBB, bool t) :  
// window(0) {
BinQGUI::BinQGUI(){
}

void 
BinQGUI::DeleteSgTree( SgNode* root) {
#if 0
  VariantVector vv1 = V_SgNode;
  std::cout << "Number of nodes before: " << 
    NodeQuery::queryMemoryPool(vv1).size() << std::endl;

  DeleteAST deleteTree;
  deleteTree.traverse(root,postorder);

  vector<SgNode*> vec = NodeQuery::queryMemoryPool(vv1);
  std::cout << "Number of nodes after deleting in AST: " << 
    vec.size() << std::endl;

  vector<SgNode*>::const_iterator it = vec.begin();
  for (;it!=vec.end();++it) {
    SgNode* node = *it;
    // tps :: the following nodes are not deleted with the 
    // AST traversal. We can only delete some of them -
    // but not all within the memory pool traversal
    //cerr << "  Not deleted : " << node->class_name() ;
    if (!isSgAsmTypeByte(node) &&
	!isSgAsmTypeWord(node) &&
	!isSgAsmTypeDoubleWord(node) &&
	!isSgAsmTypeQuadWord(node) &&

	!isSgAsmType128bitFloat(node) && 
	!isSgAsmType80bitFloat(node) && 
	!isSgAsmTypeDoubleFloat(node) && 
	!isSgAsmTypeDoubleQuadWord(node) && 
	!isSgAsmTypeSingleFloat(node) && 
	!isSgAsmTypeVector(node) && 

	!isSgAsmGenericFile(node) &&
	!isSgAsmGenericHeader(node) &&
	!isSgAsmGenericSection(node) &&
	!isSgAsmExecutableFileFormat(node) &&
	!isSgSupport(node) &&
	!isSgAsmInterpretation(node) &&
	!isSgProject(node) &&
	!isSgNode(node)
	) {
      //cerr << " .. deleting. " ;
      delete node;
    }
    //cerr << endl;
  }

    std::cout << "Number of nodes after deeleting in Memory pool: " << 
  NodeQuery::queryMemoryPool(vv1).size() << std::endl;
#endif  
}


// The GUI is created here
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

	  console = new QTextEdit;//new QREdit(QREdit::Box);
	  console->setReadOnly(true);
	  console->setText("Initializing GUI");

	  analysisResult = new QTextEdit;//new QREdit(QREdit::Box);
	  analysisResult->setReadOnly(true);

	  sectionInfo = new QTextEdit;//new QREdit(QREdit::Box);
	  sectionInfo->setReadOnly(true);

	  fileInfo = new QTextEdit;//new QREdit(QREdit::Box);
	  fileInfo->setReadOnly(true);
	  
          
          if(isSgProject(fileA) )  insertFileInformation();

	  analysisTab->insertTab(0,console,"Console");
	  analysisTab->insertTab(1,analysisResult,"Analysis Results");
	  analysisTab->insertTab(2,fileInfo,"File Info");
	  analysisTab->insertTab(3,sectionInfo,"Section Info");

	  QGroupBox *rightGraphics =  analysisPanelRight <<  new QGroupBox(("FileA Visualization"));
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
	QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivated, this);
	if (fileB) {
	  tableWidget2 = bottomPanelLeft << new QRTable( 1, "function" );
	  QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivated, this);
	} 
	//	bottomPanelLeft.setTileSize(20,20);
      }

      QRPanel &bottomPanelRight = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	codeTabA =  bottomPanelRight << new QTabWidget();
	codeTabB =  bottomPanelRight << new QTabWidget();
	codeTableWidget = new QRTable( 8, "row","address","instr","operands","comment","pos","size","byte" );
	QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivated, this);
	
	codeTabA->insertTab(0,codeTableWidget,QString(fileNameA.c_str()));

	if (fileB) {
	  if (sourceFile) {
	    codeTableWidget2 = new QRTable( 5, "row","line","text","type","pos" );
	    maxrows=3;
	  } else {
	    codeTableWidget2 =  new QRTable( 8, "row","address","instr","operands","comment","pos","size","byte" );
	  }
	  codeTabB->insertTab(0,codeTableWidget2,QString(fileNameB.c_str()));
	  QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivated, this);
	} 

	int count=1;
	//if (fileB)
	//  count=1;
	std::vector<SgNode*>::const_iterator dllIt = dllFilesA.begin();
	std::vector<std::string>::const_iterator nameIt = dllA.begin();
	codeTableWidgetADLLlist.clear();
	codeTableWidgetBDLLlist.clear();
	for (;nameIt!=dllA.end();++nameIt,++dllIt) {
	  string name = *nameIt;
	  codeTableWidgetDLL =  new QRTable( 8, "row","address","instr","operands","comment","pos","size","byte" );
	  codeTableWidgetADLLlist.push_back(codeTableWidgetDLL);
	  codeTabB->insertTab(count++,codeTableWidgetDLL,QString("LIB-A:%1").arg(name.c_str()));
	}
	QTextEdit* graphA = new QTextEdit;
	graphA->setReadOnly(true);
	graphA->append("The call or control flow graph should be shown here...");
	codeTabB->insertTab(count,graphA,"Graph FileA");

	if (fileB) {
	  count=1;
	  dllIt = dllFilesB.begin();
	  nameIt = dllB.begin();
	  for (;nameIt!=dllB.end();++nameIt,++dllIt) {
	    string name = *nameIt;
	    codeTableWidgetDLL =  new QRTable( 8, "row","address","instr","operands","comment","pos","size","byte" );
	    codeTableWidgetBDLLlist.push_back(codeTableWidgetDLL);
	    codeTabA->insertTab(count++,codeTableWidgetDLL,QString("LIB-B:%1").arg(name.c_str()));
	  }
	  
	  QTextEdit* graphB = new QTextEdit;
	  graphB->setReadOnly(true);
	  graphB->append("The call or control flow graph should be shown here...");
	  codeTabA->insertTab(count,graphB,"Graph FileB");
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


  // ------------------------------------

} //BinQGUI::BinQGUI()

// if the user hits open in the GUI
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

// if the user hits reset in the GUI
void BinQGUI::reset() {
  init();
  updateByteItemList();
  getFileForWidget.clear();
} //CompassGui::open()

// This is for testing purposes only
void
BinQGUI::testAnalyses(std::vector<BinAnalyses*>& analysesVec) {
  testAnalysisResults.clear();
  int problems=0;
  double startTotal = RoseBin_support::getTime();
  for (unsigned int i=0;i<analysesVec.size();++i) {
    //cerr << " testing analysis : " << analysesVec[i]->getDescription() << endl;
    bool twoFiles = analysesVec[i]->twoFiles();
    if (twoFiles && fileB!=NULL || twoFiles==false) {
      currentAnalysis=analysesVec[i];
      if (currentAnalysis) {
	double start = RoseBin_support::getTime();
	currentAnalysis->test(fileA,fileB);
	double end = RoseBin_support::getTime();
	double time = (double) (end - start);
	map<SgNode*,string> resu = currentAnalysis->getResult();
	problems+=resu.size();
	//cerr << "Running analysis : " << currentAnalysis->name() <<
	//  "   time : " << time << "   Problems : " << RoseBin_support::ToString(resu.size()) << endl;
	testAnalysisResults[currentAnalysis->name()]=resu.size();
      }
    }
  } 
 
  double endTotal = RoseBin_support::getTime();
  double timeTotal = (double) (endTotal - startTotal);
  //cerr << "Total time : " << timeTotal << "  problems : " << problems << endl;
  testAnalysisTime=timeTotal;
}

std::map<std::string, int>
BinQGUI::getTestAnalysisResults() {
  return testAnalysisResults;
}

double
BinQGUI::getTestAnalysisTime() {
  return testAnalysisTime;
}

// this is the implementation of the run function in the GUI
void
BinQGUI::run( ) {
  //  cerr << " calling run!! " << endl;
  if (analysisResult)
    analysisResult->clear();
  if (currentAnalysis) {
    bool twoFiles = currentAnalysis->twoFiles();
    if (twoFiles)
      currentAnalysis->run(fileA,fileB);
    else if (currentSelectedFile!=NULL) {
      if (isSgProject(currentSelectedFile)) {
	SgBinaryComposite* binary = isSgBinaryComposite(isSgProject(currentSelectedFile)->get_fileList()[0]);
	SgAsmGenericFile* file = binary != NULL ? binary->get_binaryFile() : NULL;
	if (file) {
	  // cerr << " Current selected file : " << file->get_name() << endl;
	}
      }
      currentAnalysis->run(currentSelectedFile,NULL);
    }
    analysisResult->moveCursor(QTextCursor::Start);
  }
  updateByteItemList();
}

// Creates the Function table (left hand side) for file A and B
void 
BinQGUI::showFileTab() {
  //cerr << "creating FileTab " << endl; 

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
  
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivated, this);
  if (fileB)
    QROSE::link(tableWidget2, SIGNAL(activated(int, int, int, int)),  &tableWidgetCellActivated, this);
  
}


// Creates the table for Instructions for FileB
void BinQGUI::showFile(int row, qrs::QRTable* currentWidget,
                       std::vector<SgNode*>& funcsFile,
		       std::vector<Item*>& itemsFile) {
  QROSE::unlink(currentWidget, SIGNAL(activated(int, int, int, int)));

  while(currentWidget->rowCount()) 
    currentWidget->removeRow(0);

  std::string funcname ="";
  // make sure that only valid files are passed on
  if (funcsFile.size()==0) {
    cerr << "INVALID BINARY"<<endl;
    exit(0);
  }
  if (isSgAsmFunctionDeclaration(funcsFile[row])) {
    funcname=isSgAsmFunctionDeclaration(funcsFile[row])->get_name();
  }
  QString res = QString(" Looking at function  %1  row: %2  size ")
    .arg(funcname.c_str())
    .arg(row);
  //    .arg(elem.size);
  console->append(res);  
  //  std::cout << "Looking at function " << elem.function_name_B << "   row : " << row << "   size : " << elem.size << std::endl;
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFile.size(); i++ )    {
    SgNode* stmts = itemsFile[i]->statement;
    int length=1;    
    bool addRow=false;
    if (isSgAsmx86Instruction(stmts)) {
      currentWidget->addRows(1);
      length = isSgAsmInstruction(stmts)->get_raw_bytes().size();
      //itemsFile[i]->bg=QColor(255,255,255);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(0,0,0);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
      }
      currentWidget->setTextColor(front,0,i);
      currentWidget->setTextColor(QColor(255,0,0),1,i);
      currentWidget->setTextColor(QColor(0,0,255),2,i);
      currentWidget->setTextColor(QColor(0,155,0),3,i);
      currentWidget->setTextColor(QColor(0,155,0),4,i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_mnemonic() ), 2, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts)->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseExpression(*it) )+", ";
      }
      currentWidget->setText(boost::lexical_cast<std::string>(opsName), 3, i);	
      currentWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_comment() ), 4, i);
      //currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->comment), 4, i);
      //cerr << " Setting comment : " << (isSgAsmx86Instruction(stmts))->get_comment() << endl;
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 5, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->length), 6, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->realByteSize), 7, i);	
      //cerr << i<< ": Found instruction : " << RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) << "  mnemonic : " << (isSgAsmx86Instruction(stmts))->get_mnemonic()  <<
      //	" row : " << itemsFile[i]->row <<  endl;
      addRow=true;
    } else if (isSgAsmBlock(stmts)  && !(isSgAsmInterpretation(isSgAsmBlock(stmts)->get_parent()))) {
      //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      currentWidget->addRows(1);
      currentWidget->setTextColor(QColor(128,128,128),0,i);
      currentWidget->setTextColor(QColor(128,128,128),1,i);
      currentWidget->setTextColor(QColor(255,255,0),2,i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts))->get_address()) ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>("***"), 2, i);
      addRow=true;
    } else if (isSgAsmFunctionDeclaration(stmts)) {
      currentWidget->addRows(1);
      itemsFile[i]->bg=QColor(0,0,0);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(255,255,255);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
	currentWidget->setTextColor(front,j,i);
      }
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts))->get_address()) ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>("FUNC"), 2, i);
      currentWidget->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts))->get_name() ), 3, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 5, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->length), 6, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->realByteSize), 7, i);	
      addRow=true;
    } 
    else if (isSgAsmElfSection(stmts)) {
      currentWidget->addRows(1);
      itemsFile[i]->bg=QColor(0,100,0);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(255,255,255);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
	currentWidget->setTextColor(front,j,i);
      }
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFile[i]->addr) ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSection(itemsFile[i]->statement)->get_name()->get_string()), 3, i);
      currentWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 4, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->comment ), 2, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 5, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->length), 6, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->realByteSize), 7, i);	
      addRow=true;
    }

    else if (isSgAsmElfSectionTableEntry(stmts)) {
      currentWidget->addRows(1);
      itemsFile[i]->bg=QColor(0,0,0);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(255,255,255);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
	currentWidget->setTextColor(front,j,i);
      }
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFile[i]->addr) ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSectionTableEntry(itemsFile[i]->statement)->class_name()), 3, i);
      currentWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 4, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->comment ), 2, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 5, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->length), 6, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->realByteSize), 7, i);	
      addRow=true;
    }

    else if (isSgAsmElfSegmentTableEntry(stmts)) {
      currentWidget->addRows(1);
      itemsFile[i]->bg=QColor(0,0,0);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(255,255,255);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
	currentWidget->setTextColor(front,j,i);
      }
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFile[i]->addr) ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSegmentTableEntry(itemsFile[i]->statement)->class_name()), 3, i);
      currentWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 4, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->comment ), 2, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 5, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->length), 6, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->realByteSize), 7, i);	
      addRow=true;
    }

    else if (isSgAsmElfSymbol(stmts)) {
      currentWidget->addRows(1);
      itemsFile[i]->bg=QColor(50,50,0);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(255,255,255);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
	currentWidget->setTextColor(front,j,i);
      }
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString(itemsFile[i]->addr) ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>(isSgAsmElfSymbol(itemsFile[i]->statement)->get_name()->get_string()), 3, i);
      currentWidget->setText(boost::lexical_cast<std::string>((isSgAsmNode(stmts))->class_name() ), 4, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->comment ), 2, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 5, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->length), 6, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->realByteSize), 7, i);	
      addRow=true;
    }

    else if (isSgFunctionDeclaration(stmts)) {
      SgFunctionDeclaration* func = isSgFunctionDeclaration(stmts);
      //cerr << func->class_name() << "  maxrows: " << maxrows << endl;
      currentWidget->addRows(1);
      itemsFile[i]->bg=QColor(0,0,0);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(255,255,255);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
	currentWidget->setTextColor(front,j,i);
      }
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>("FUNC"), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>(func->get_name().str() ), 2, i);
      std::string comment = func->get_file_info()->isCompilerGenerated() ? "compiler gen": " ";
      currentWidget->setText(boost::lexical_cast<std::string>(comment ), 3, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 4, i);	
      addRow=true;
    } 

    else if (isSgLocatedNode(stmts)) {
      SgLocatedNode* st = isSgLocatedNode(stmts);
      //cerr << st->class_name() << "  maxrows: " << maxrows << endl;
      currentWidget->addRows(1);

      itemsFile[i]->bg=QColor(255,255,255);
      QColor back = itemsFile[i]->bg;
      itemsFile[i]->fg=QColor(0,0,0);
      QColor front = itemsFile[i]->fg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
      }
      currentWidget->setTextColor(front,0,i);
      currentWidget->setTextColor(QColor(255,0,0),1,i);
      currentWidget->setTextColor(QColor(0,0,255),2,i);
      currentWidget->setTextColor(QColor(0,155,0),3,i);
      currentWidget->setTextColor(QColor(0,155,0),4,i);

      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->lineNr ), 1, i);
      currentWidget->setText(boost::lexical_cast<std::string>(st->unparseToString()), 2, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->comment ), 3, i);
      currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 4, i);	
      addRow=true;
    } 

    else {
      currentWidget->addRows(1);
      itemsFile[i]->bg=QColor(128,128,128);
      QColor back = itemsFile[i]->bg;
      for (int j=1;j<maxrows;++j) {
	currentWidget->setBgColor(back,j,i);
      }
      //cerr << "Unknown Type : " << stmts->class_name() << endl;
      if (itemsFile[i]->row) {
	currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->row), 0, i);	
	if (!sourceFile) {
	  currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->pos), 5, i);	
	  currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->length), 6, i);	
	  currentWidget->setText(boost::lexical_cast<std::string>(itemsFile[i]->realByteSize), 7, i);	
	}
      }
      addRow=true;
    }

      if (addRow) {
	//if ((rowC%500)==0)
	//cerr << "Adding Row : " << RoseBin_support::ToString(rowC) << "/"<< RoseBin_support::ToString(itemsFile.size()) << endl;
	// tps : this is still very slow and commented out for now
#if 0
		currentWidget->setHAlignment(true, false, 3); // left horizontal alignment

	currentWidget->setHAlignment(true, false, 0); // left horizontal alignment
	currentWidget->setHAlignment(true, false, 1); // left horizontal alignment
	currentWidget->setHAlignment(true, false, 2); // left horizontal alignment
	currentWidget->setHAlignment(true, false, 4); // left horizontal alignment
#endif
      
	currentWidget->setVDim(i,18);
	if (sourceFile) {
	  currentWidget->setHDim(0,30);
	  currentWidget->setHDim(1,50);
	  currentWidget->setHDim(2,300);
	  currentWidget->setHDim(3,110);
	  currentWidget->setHDim(4,40);
	} else {
#if 0
	  currentWidget->setHAlignment(true, false, 5); // left horizontal alignment
	  currentWidget->setHAlignment(true, false, 6); // left horizontal alignment
	  currentWidget->setHAlignment(true, false, 7); // left horizontal alignment
#endif
	  currentWidget->setHDim(0,40);
	  currentWidget->setHDim(1,65);
	  currentWidget->setHDim(2,55);
	  currentWidget->setHDim(3,240);
	  currentWidget->setHDim(4,90);
	  currentWidget->setHDim(5,40);
	  currentWidget->setHDim(6,40);
	  currentWidget->setHDim(7,40);
	}
	rowC++;
	posC+=length;
      }
  }
  //cerr << "Widget done." <<endl;
  currentWidget->setShowGrid(false);
  currentWidget->setCurrentCell(row,0);


}





