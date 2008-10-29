#include <boost/algorithm/string.hpp>
#include "BinQGui.h"
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
#include "BinQSupport.h"
#include "slide.h"

#define EMACS

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;







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
      SgAsmFunctionDeclaration* func = funcsFileA[row];
      std::vector<Item*>::iterator it = itemsFileA.begin();
      int offset=0;
      for (;it!=itemsFileA.end();++it) {
	Item* item = *it;
	SgAsmStatement* stat = item->statement;
	if (func==isSgAsmFunctionDeclaration(stat)) {
	  offset=item->row;
	  break;
	}
      }
      //cerr << " highlight func row : " << row << "  inst row : " << offset << endl;
      codeTableWidget->setCurrentCell(offset,0);
    } else {
      QFont f = tableWidget2->getFont(0, row);
      f.setBold(true);
      tableWidget2->setFont(f, 0, row);
      tableWidget2->setBgColor(QColor(255,255,0),0,row);
      //      showFileB(row);
      SgAsmFunctionDeclaration* func = funcsFileB[row];
      std::vector<Item*>::iterator it = itemsFileB.begin();
      int offset=0;
      for (;it!=itemsFileB.end();++it) {
	Item* item = *it;
	SgAsmStatement* stat = item->statement;
	if (func==isSgAsmFunctionDeclaration(stat)) {
	  offset=item->row;
	  break;
	}
      }
      //      cerr << " highlight func row : " << row << "  inst row : " << offset << endl;
      codeTableWidget2->setCurrentCell(offset,0);
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
      codeTableWidget->setFont(f, 0, row);
      codeTableWidget->setFont(f, 1, row);
      codeTableWidget->setFont(f, 2, row);
      codeTableWidget->setCurrentCell(row,0);
      Item* item = itemsFileA[row];
      if (item->function) {
	codeTableWidget->setBgColor(QColor(120,120,120),0,row);
	codeTableWidget->setBgColor(QColor(120,120,120),1,row);
	codeTableWidget->setBgColor(QColor(120,120,120),2,row);
	codeTableWidget->setBgColor(QColor(120,120,120),3,row);
      } else {
	codeTableWidget->setBgColor(QColor(255,255,0),0,row);
	codeTableWidget->setBgColor(QColor(255,255,0),1,row);
	codeTableWidget->setBgColor(QColor(255,255,0),2,row);
	codeTableWidget->setBgColor(QColor(255,255,0),3,row);
      }
    } else {
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(true);
      codeTableWidget2->setFont(f, 0, row);
      codeTableWidget2->setFont(f, 1, row);
      codeTableWidget2->setFont(f, 2, row);
      codeTableWidget2->setCurrentCell(row,0);
      Item* item = itemsFileB[row];
      if (item->function) {
	codeTableWidget2->setBgColor(QColor(120,120,120),0,row);
	codeTableWidget2->setBgColor(QColor(120,120,120),1,row);
	codeTableWidget2->setBgColor(QColor(120,120,120),2,row);
	codeTableWidget2->setBgColor(QColor(120,120,120),3,row);
      } else {
	codeTableWidget2->setBgColor(QColor(255,255,0),0,row);
	codeTableWidget2->setBgColor(QColor(255,255,0),1,row);
	codeTableWidget2->setBgColor(QColor(255,255,0),2,row);
	codeTableWidget2->setBgColor(QColor(255,255,0),3,row);
      }
    }
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightInstructionRow(int row,bool fileA) {
  if (row >= 0)     {
    if (fileA) {
      QFont f = codeTableWidget->getFont(0, row);
      f.setBold(false);
      codeTableWidget->setFont(f, 0, row);
      codeTableWidget->setFont(f, 1, row);
      codeTableWidget->setFont(f, 2, row);
      Item* item = itemsFileA[row];
      if (!item->function) {
	codeTableWidget->setBgColor(QColor(255,255,255),0,row);
	codeTableWidget->setBgColor(QColor(255,255,255),1,row);
	codeTableWidget->setBgColor(QColor(255,255,255),2,row);
	codeTableWidget->setBgColor(QColor(255,255,255),3,row);
      } else {
	codeTableWidget->setBgColor(QColor(0,0,0),0,row);
	codeTableWidget->setBgColor(QColor(0,0,0),1,row);
	codeTableWidget->setBgColor(QColor(0,0,0),2,row);
	codeTableWidget->setBgColor(QColor(0,0,0),3,row);
      }
    } else {
      QFont f = codeTableWidget2->getFont(0, row);
      f.setBold(false);
      codeTableWidget2->setFont(f, 0, row);
      codeTableWidget2->setFont(f, 1, row);
      codeTableWidget2->setFont(f, 2, row);
      Item* item = itemsFileB[row];
      if (!item->function) {
	codeTableWidget2->setBgColor(QColor(255,255,255),0,row);
	codeTableWidget2->setBgColor(QColor(255,255,255),1,row);
	codeTableWidget2->setBgColor(QColor(255,255,255),2,row);
	codeTableWidget2->setBgColor(QColor(255,255,255),3,row);
      } else {
	codeTableWidget2->setBgColor(QColor(0,0,0),0,row);
	codeTableWidget2->setBgColor(QColor(0,0,0),1,row);
	codeTableWidget2->setBgColor(QColor(0,0,0),2,row);
	codeTableWidget2->setBgColor(QColor(0,0,0),3,row);
      }
    }
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)




// ----------------------------------------------------------------------------------------------




BinQGUI::BinQGUI(std::string fA, std::string fB ) :  window(0), fileNameA(fA), fileNameB(fB) {
  window = new QRWindow( "mainWindow", QROSE::TopDown );
  binqsupport= new BinQSupport();

  fileA = binqsupport->disassembleFile(fileNameA);
  fileB = binqsupport->disassembleFile(fileNameB);

  FindInstructionsVisitor vis;
  scoped_array<scoped_array<size_t> > C;
  vector_start_at_one<SgNode*> insnsA;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( vis, &insnsA ));
  vector_start_at_one<SgNode*> insnsB;
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( vis, &insnsB ));

  LCSLength(C,insnsA,insnsB);
  std::vector<pair<int,int> > addInstr,minusInst;
  printDiff(C,insnsA, insnsB,insnsA.size(),insnsB.size(),addInstr,minusInst);

  
  cerr << " found adds on left side : " << addInstr.size() << endl;
  cerr << " found subbs on left side : " << minusInst.size() << endl;



  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcsFileA ));
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( funcVis, &funcsFileB ));

  FindStatementsVisitor visStat;
  std::vector<SgAsmStatement*> stmts;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat, &stmts ));
  vector<SgAsmStatement*>::iterator it= stmts.begin();
  int pos=0;
  int row=0;
  for (;it!=stmts.end();++it) {
    Item* item;
    int length=1;
    if (isSgAsmFunctionDeclaration(*it)) {
      FindStatementsVisitor visStat2;
      std::vector<SgAsmStatement*> stmts2;
      AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat2, &stmts2 ));
      int funcsize= stmts2.size();
      item = new Item(true,*it,funcsize,2,row);
    } else if (isSgAsmBlock(*it)) {
            continue;
      //item = new Item(false,*it,0,1,row);
    } else
      item = new Item(false,*it,0,0,row);
    if (isSgAsmx86Instruction(*it)) {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      if (isSgAsmx86Instruction(*it)->get_kind() == x86_push)
	item = new Item(false,*it,0,3,row);
    }
    row++;
    itemsFileA.push_back(item);
    byteItemFileA[pos]=item;
    //cerr << "FILE_A: Adding item at pos:"<<pos<<"  length: " <<length<< "   " << item->statement->class_name()<<endl;
    for (int i=0;i<length;i++)
      byteItemFileA[i]=item;
    pos+=length;
  }

  stmts.clear();
  AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat, &stmts ));
  it= stmts.begin();
  pos=0;
  row=0;
  for (;it!=stmts.end();++it) {
    Item* item;
    int length=1;
    if (isSgAsmFunctionDeclaration(*it)){
      FindStatementsVisitor visStat2;
      std::vector<SgAsmStatement*> stmts2;
      AstQueryNamespace::querySubTree(fileB, std::bind2nd( visStat2, &stmts2 ));
      int funcsize= stmts2.size();
      item = new Item(true,*it,funcsize,2,row);
    }    else if (isSgAsmBlock(*it)) {
      continue;
      //item = new Item(false,*it,0,1,row);
      } else
      item = new Item(false,*it,0,0,row);
    if (isSgAsmx86Instruction(*it)) {
      length = isSgAsmInstruction(*it)->get_raw_bytes().size();
      if (isSgAsmx86Instruction(*it)->get_kind() == x86_push)
	item = new Item(false,*it,0,3,row);
    }
    row++;
    itemsFileB.push_back(item);
    byteItemFileB[pos]=item;
    //cerr << "FILE_B: Adding item at pos:"<<pos<<"  length: " <<length<< "   " << item->statement->class_name()<<endl;
    for (int i=0;i<length;i++)
      byteItemFileB[i]=item;
    pos+=length;
  }
  QDesktopWidget *desktop = QApplication::desktop();


  screenWidth = desktop->width()-50;
  screenHeight = desktop->height()-150;
  if (screenWidth>1424) screenWidth=1424;
  if (screenHeight>1124) screenHeight=1124;


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
	QGroupBox *selectGroup2 =  analysisPanel <<  new QGroupBox(("Analyses available"));
	{
	  QGridLayout *echoLayout2 =  new QGridLayout;
	  
	  listWidget = new QListWidget;
	  new QListWidgetItem(("Oak"), listWidget);
	  new QListWidgetItem(("OakTree"), listWidget);
	  echoLayout2->addWidget(listWidget, 0, 0 );
	  selectGroup2->setLayout(echoLayout2);
	}
	QGroupBox *selectGroup3 =  analysisPanel <<  new QGroupBox(("Analyses results"));
	{
	  QGridLayout *echoLayout2 =  new QGridLayout;
	  analysisResult = new QTextEdit;//new QREdit(QREdit::Box);
	  analysisResult->setReadOnly(true);
	  echoLayout2->addWidget(analysisResult, 0, 0 );
	  selectGroup3->setLayout(echoLayout2);
	  analysisResult->setText("Initializing GUI");
	}
	analysisPanel.setTileSize(50,50);

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
	codeTableWidget = bottomPanelRight << new QRTable( 4, "address","instr","operands","comment" );
	QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedA, this);
	codeTableWidget2 = bottomPanelRight << new QRTable( 4, "address","instr","operands","comment" );
	QROSE::link(codeTableWidget2, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivatedB, this);

	//	bottomPanelRight.setTileSize(0,80);
      }
      bottomPanelLeft.setFixedWidth(screenWidth/4 );
      //bottomPanelRight.setFixedWidth(screenWidth/2 );

    } //mainPanel
    mainPanel.setTileSize(30);
  } //window 



  window->setGeometry(0,0,screenWidth,screenHeight);
  window->setTitle("BinQ");
  analysisResult->append("Initializing done.");
  QString res = QString("Total functions  %1.  Total statements: %2. ")
    .arg(funcsFileA.size())
    .arg(stmts.size());
  analysisResult->append(res);  

} //BinQGUI::BinQGUI()


void BinQGUI::open() {
  char buf[4096] = "\0";
  std::string database = QFileDialog::getOpenFileName( 0, "Open As", getcwd(buf, 4096), "ASCII (*.sql)").toStdString();
  if( database.empty() ) return;
  if(exists(database) == true)    {
    //codeWidget->setReadOnly(true);
    //codeWidget->setPlainText(QString("foobar\nBar\nFoobari3"));
  }
} //CompassGui::open()

void BinQGUI::reset() {
} //CompassGui::open()




void
BinQGUI::run( ) {
  QROSE::unlink(tableWidget, SIGNAL(activated(int, int, int, int)));
  QROSE::unlink(tableWidget2, SIGNAL(activated(int, int, int, int)));
  while(tableWidget->rowCount()) 
    tableWidget->removeRow(0);
  while(tableWidget2->rowCount()) 
    tableWidget2->removeRow(0);

  for (size_t row = 0; row < funcsFileA.size(); ++row) {
    tableWidget->addRows(1);
    tableWidget->setText(boost::lexical_cast<std::string>(funcsFileA[row]->get_name()), 0, row);
    //tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.size), 1, row);
    tableWidget->setVDim(row,18);
  }
  for (size_t row = 0; row < funcsFileB.size(); ++row) {
    tableWidget2->addRows(1);
    tableWidget2->setText(boost::lexical_cast<std::string>(funcsFileB[row]->get_name()), 0, row);
    tableWidget2->setVDim(row,18);
  }
  tableWidget->setHAlignment(true, false, 0); // left horizontal alignment
  //tableWidget->setHAlignment(true, false, 1); // left horizontal alignment
  tableWidget->setTextColor(QColor(0,0,255),0);
  tableWidget->setHDim(0,140);
  //tableWidget->setHDim(1,40);
  tableWidget->setShowGrid(false);

  tableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
  tableWidget2->setTextColor(QColor(0,0,255),0);
  tableWidget2->setHDim(0,140);
  //tableWidget2->setHDim(1,40);
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

  QString res = QString("FILE A : Looking at function  %1  row: %2  size ")
    .arg(funcsFileA[row]->get_name().c_str())
    .arg(row);
  //    .arg(elem.size);
  analysisResult->append(res);  
  //  std::cout << "Looking at function " << elem.function_name_B << "   row : " << row << "   size : " << elem.size << std::endl;
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFileA.size(); i++ )    {
    SgAsmStatement* stmts = itemsFileA[i]->statement;
    int length=1;    
    bool addRow=false;
    posRowA[posC]=-1;
    if (isSgAsmx86Instruction(stmts)) {
      codeTableWidget->addRows(1);
      length = isSgAsmInstruction(stmts)->get_raw_bytes().size();
      codeTableWidget->setTextColor(QColor(255,0,0),0,i);
      codeTableWidget->setTextColor(QColor(0,0,255),1,i);
      codeTableWidget->setTextColor(QColor(0,155,0),2,i);
      codeTableWidget->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) ), 0, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_mnemonic() ), 1, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts)->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseX86Expression(*it, false) );
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(opsName), 2, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_comment() ), 3, i);
      addRow=true;
    } else if (isSgAsmBlock(stmts)  && !(isSgAsmInterpretation(isSgAsmBlock(stmts)->get_parent()))) {
    //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      codeTableWidget->addRows(1);
      codeTableWidget->setTextColor(QColor(128,128,128),0,i);
      codeTableWidget->setTextColor(QColor(255,255,0),1,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts))->get_address()) ), 0, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("***"), 1, i);
      addRow=true;
    } else if (isSgAsmFunctionDeclaration(stmts),i) {
      codeTableWidget->addRows(1);
      //      codeTableWidget->addRows(1);
      codeTableWidget->setBgColor(QColor(0,0,0),0,i);
      codeTableWidget->setBgColor(QColor(0,0,0),1,i);
      codeTableWidget->setBgColor(QColor(0,0,0),2,i);
      codeTableWidget->setBgColor(QColor(0,0,0),3,i);
      codeTableWidget->setTextColor(QColor(255,255,255),0,i);
      codeTableWidget->setTextColor(QColor(255,255,255),1,i);
      codeTableWidget->setTextColor(QColor(255,255,255),2,i);
      codeTableWidget->setTextColor(QColor(255,255,255),3,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts))->get_address()) ), 0, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("FUNC"), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts))->get_name() ), 2, i);
      addRow=true;
    } else {
      codeTableWidget->addRows(1);
      cerr << " FOUND UNEXPECTED NODE " << stmts->class_name() << endl;
      addRow=true;
    }
    if (addRow) {
      codeTableWidget->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 2); // left horizontal alignment
      codeTableWidget->setHAlignment(true, false, 3); // left horizontal alignment
      
      codeTableWidget->setVDim(i,18);
      codeTableWidget->setHDim(0,80);
      codeTableWidget->setHDim(1,50);
      codeTableWidget->setHDim(2,170);
      codeTableWidget->setHDim(3,130);
      
      posRowA[posC]=rowC;
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

  QString res = QString("FILE B : Looking at function  %1  row: %2  size ")
    .arg(funcsFileB[row]->get_name().c_str())
    .arg(row);
  //    .arg(elem.size);
  analysisResult->append(res);  
  //  std::cout << "Looking at function " << elem.function_name_B << "   row : " << row << "   size : " << elem.size << std::endl;
  int rowC=0;
  int posC=0;
  for(size_t i=0; i < itemsFileB.size(); i++ )    {
    SgAsmStatement* stmts = itemsFileB[i]->statement;
    int length=1;    
    bool addRow=false;
    posRowB[posC]=-1;
    if (isSgAsmx86Instruction(stmts)) {
      codeTableWidget2->addRows(1);
      length = isSgAsmInstruction(stmts)->get_raw_bytes().size();
      codeTableWidget2->setTextColor(QColor(255,0,0),0,i);
      codeTableWidget2->setTextColor(QColor(0,0,255),1,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),2,i);
      codeTableWidget2->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts))->get_address()) ), 0, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_mnemonic() ), 1, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts)->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseX86Expression(*it, false) );
      }
      codeTableWidget2->setText(boost::lexical_cast<std::string>(opsName), 2, i);	
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts))->get_comment() ), 3, i);
      addRow=true;
    } else if (isSgAsmBlock(stmts)  && !(isSgAsmInterpretation(isSgAsmBlock(stmts)->get_parent()))) {
    //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      codeTableWidget2->addRows(1);
      codeTableWidget2->setTextColor(QColor(128,128,128),0,i);
      codeTableWidget2->setTextColor(QColor(255,255,0),1,i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts))->get_address()) ), 0, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>("***"), 1, i);
      addRow=true;
    } else if (isSgAsmFunctionDeclaration(stmts),i) {
      codeTableWidget2->addRows(1);
      //      codeTableWidget->addRows(1);
      codeTableWidget2->setBgColor(QColor(0,0,0),0,i);
      codeTableWidget2->setBgColor(QColor(0,0,0),1,i);
      codeTableWidget2->setBgColor(QColor(0,0,0),2,i);
      codeTableWidget2->setBgColor(QColor(0,0,0),3,i);
      codeTableWidget2->setTextColor(QColor(255,255,255),0,i);
      codeTableWidget2->setTextColor(QColor(255,255,255),1,i);
      codeTableWidget2->setTextColor(QColor(255,255,255),2,i);
      codeTableWidget2->setTextColor(QColor(255,255,255),3,i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts))->get_address()) ), 0, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>("FUNC"), 1, i);
      codeTableWidget2->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts))->get_name() ), 2, i);
      addRow=true;
    } else {
      codeTableWidget2->addRows(1);
      cerr << " FOUND UNEXPECTED NODE " << stmts->class_name() << endl;
      addRow=true;
    }
    if (addRow) {
      codeTableWidget2->setHAlignment(true, false, 0); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 1); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 2); // left horizontal alignment
      codeTableWidget2->setHAlignment(true, false, 3); // left horizontal alignment
      
      codeTableWidget2->setVDim(i,18);
      codeTableWidget2->setHDim(0,80);
      codeTableWidget2->setHDim(1,50);
      codeTableWidget2->setHDim(2,170);
      codeTableWidget2->setHDim(3,130);
      
      posRowB[posC]=rowC;
      //      cerr << "added at pos:" << posC << "  rowC:" << rowC<<endl;
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



