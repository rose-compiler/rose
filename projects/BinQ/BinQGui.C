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

static void tableWidgetCellActivated(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightFunctionRow(oldRow);
  instance->highlightFunctionRow(row);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void codeTableWidgetCellActivated(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightInstructionRow(oldRow);
  instance->highlightInstructionRow(row);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void viewBoxActivated(int selection) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->selectView(selection);
  return;
}







// ----------------------------------------------------------------------------------------------

void BinQGUI::highlightFunctionRow(int row) {
  activeFunctionRow = -1;
  if(row >= 0)    {         
    QFont f = tableWidget->getFont(0, row);
    f.setBold(true);
    tableWidget->setFont(f, 0, row);
    activeFunctionRow = row;
    showFileA(row);
    //tableWidget->isItemSelected(tableWidget->horizontalHeaderItem(row));
  } //if(row >= 0)
} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightFunctionRow(int row) {
  if (row >= 0)     {
    QFont f = tableWidget->getFont(0, row);
    f.setBold(false);
    tableWidget->setFont(f, 0, row);
  } //if (row >= 0)
} //CompassGui::unhighlighFunctionRow(int row)


void BinQGUI::highlightInstructionRow(int row) {
  activeInstructionRow = -1;
  if(row >= 0)    {         
    QFont f = codeTableWidget->getFont(0, row);
    f.setBold(true);
    codeTableWidget->setFont(f, 0, row);
    activeInstructionRow = row;
    //tableWidget->isItemSelected(tableWidget->horizontalHeaderItem(row));
  } //if(row >= 0)

} //CompassGui::highlighFunctionRow(int row)

void BinQGUI::unhighlightInstructionRow(int row) {
  if (row >= 0)     {
    QFont f = codeTableWidget->getFont(0, row);
    f.setBold(false);
    codeTableWidget->setFont(f, 0, row);
  } //if (row >= 0)

} //CompassGui::unhighlighFunctionRow(int row)




// ----------------------------------------------------------------------------------------------
void
BinQGUI::selectView(int selection) {
  switch (selection) {
  case 0:
    //    codeWidget->setHtml(QString(normalizedView.first.c_str()));
    codeWidget2->setHtml(QString(normalizedView.second.c_str()));
    break;
  case 1:
    //codeWidget->setHtml(QString(allInsnsUnparsedView.first.c_str()));
    codeWidget2->setHtml(QString(allInsnsUnparsedView.second.c_str()));
    break;
  }
};




BinQGUI::BinQGUI(std::string fA, std::string fB ) :  window(0), fileNameA(fA), fileNameB(fB) {
  window = new QRWindow( "mainWindow", QROSE::TopDown );
  binqsupport= new BinQSupport();

  fileA = binqsupport->disassembleFile(fileNameA);
  fileB = binqsupport->disassembleFile(fileNameB);
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
    QRPanel &parameters = mainPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
    {
      QGroupBox *selectGroup =  parameters <<  new QGroupBox(("Data Selection Parameters"));
      {
        QGridLayout *echoLayout =  new QGridLayout;
        QLabel *echoLabel = new QLabel(">=:");
	echoLayout->addWidget(echoLabel, 0, 0);
        largerThanRestriction = new QLineEdit;
        echoLayout->addWidget(largerThanRestriction, 0, 1);
        
        QLabel *functionLabel = new QLabel("<=:");
        echoLayout->addWidget(functionLabel, 1, 0);
        smallerThanRestriction = new QLineEdit;
        echoLayout->addWidget(smallerThanRestriction, 1, 1 );


	wholeFunction = new QComboBox;
	wholeFunction->addItem(("No"));
	wholeFunction->addItem(("Yes"));

	QLabel *wholeFunctionLabel = new QLabel("Only whole functions:");
	echoLayout->addWidget(wholeFunctionLabel, 2, 0 );

	echoLayout->addWidget(wholeFunction, 2, 1 );

        selectGroup->setLayout(echoLayout);
      }
      
      comboBox = new QComboBox;
      comboBox->addItem(("Normalized"));
      comboBox->addItem(("Whole Function"));
      
      QGroupBox *echoGroup =  parameters <<  new QGroupBox(("Selection Clone-View"));
      QGridLayout *echoLayout =  new QGridLayout;
      QLabel *echoLabel = new QLabel("Views:");
      echoLayout->addWidget(echoLabel, 0, 0);
      echoLayout->addWidget(comboBox, 0, 1);


      echoGroup->setLayout(echoLayout);

      QROSE::link(comboBox, SIGNAL(activated(int)), &viewBoxActivated, this);
    }
      
    QRPanel &bottomPanel = mainPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
    {
      QRPanel &bottomPanelLeft = bottomPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
      {
	tableWidget = bottomPanelLeft << new QRTable( 2, "function","#instr" );
	QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableWidgetCellActivated, this);
	codeTableWidget = bottomPanelLeft << new QRTable( 4, "address","instr","operands","comment" );


	QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), &codeTableWidgetCellActivated, this);

	//	codeWidget = bottomPanelLeft << new QTextEdit;//new QREdit(QREdit::Box);
	//codeWidget->setReadOnly(true);
      }
      bottomPanelLeft.setTileSize(25,90);
      codeWidget2 = bottomPanel << new QTextEdit;//new QREdit(QREdit::Box);
      codeWidget2->setReadOnly(true);
      bottomPanel.setTileSize(50,50);
    } //mainPanel
    mainPanel.setTileSize(10,60);
  } //window 

  window->setGeometry(0,0,1280,1200);
  window->setTitle("BinaryCloneMainGui");
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
  while(tableWidget->rowCount()) tableWidget->removeRow(0);


  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcs ));
  //FindInstructionsVisitor vis;
  //AstQueryNamespace::querySubTree(fileA, std::bind2nd( vis, &insns ));
  FindStatementsVisitor visStat;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( visStat, &stmts ));

  vectorOfClones.allocate(funcs.size());
  for (size_t row = 0; row < funcs.size(); ++row) {
    Element cur_elem;
    cur_elem.function_name_A = funcs[row]->get_name();
    cur_elem.function_name_B = funcs[row]->get_name();

    FindStatementsVisitor visStat2;
    std::vector<SgAsmStatement*> stmts2;
    AstQueryNamespace::querySubTree(funcs[row], std::bind2nd( visStat2, &stmts2 ));
    cur_elem.size = stmts2.size();
    vectorOfClones[row] = cur_elem;
    
    tableWidget->addRows(1);
    tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.function_name_A), 0, row);
    tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.size), 1, row);
    tableWidget->setVDim(row,18);
  }
  tableWidget->setHAlignment(true, false, 0); // left horizontal alignment
  tableWidget->setHAlignment(true, false, 1); // left horizontal alignment
  tableWidget->setTextColor(QColor(0,0,255),0);
  tableWidget->setHDim(0,100);
  tableWidget->setHDim(1,40);
  tableWidget->setShowGrid(false);
  //    tableWidget->setColumnStretchable(0,false);
  
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), 
	      &tableWidgetCellActivated, this);
  showFileA(0);
}


  


void BinQGUI::showFileA(int row) {
  QROSE::unlink(codeTableWidget, SIGNAL(activated(int, int, int, int)));
  while(codeTableWidget->rowCount()) codeTableWidget->removeRow(0);

  //Disassemble files
  Element& elem = vectorOfClones[row];
  std::string unparsedFileA;
  std::string unparsedFileB;

  std::string normalizedFileA;
  std::string normalizedFileB;

  ROSE_ASSERT(fileA != NULL);
  ROSE_ASSERT(fileB != NULL);


  int addr=0;
  for (size_t i = 0; i < funcs.size(); ++i) {
    if( funcs[i]->get_name() == (elem.function_name_A ))      {
      addr=funcs[i]->get_address();
      //cerr << " FOUND FUNCTION " << elem.function_name_A << " at address : " << RoseBin_support::HexToString(addr) << endl;
    }
  }
  size_t pos=0;
  for (size_t i = 1; i < stmts.size(); ++i) {
    if (stmts[i]->get_address()==addr) {
      pos=i;
      //      cerr << " FOUND STMT at address : " << RoseBin_support::HexToString(addr) << "  at pos : " << pos << endl;
      break;
    }
  }

  std::cout << "Looking at function " << elem.function_name_B << "   pos : " << pos << "   size : " << elem.size << std::endl;
  std::string allInsnsUnparsed;
  for(size_t i=0; i < stmts.size(); i++ )    {
    bool addRow=false;
    if (isSgAsmx86Instruction(stmts[i])) {
      codeTableWidget->addRows(1);
      codeTableWidget->setTextColor(QColor(255,0,0),0,i);
      codeTableWidget->setTextColor(QColor(0,0,255),1,i);
      codeTableWidget->setTextColor(QColor(0,155,0),2,i);
      codeTableWidget->setTextColor(QColor(0,155,0),3,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmx86Instruction(stmts[i]))->get_address()) ), 0, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts[i]))->get_mnemonic() ), 1, i);
      SgAsmOperandList * ops = isSgAsmx86Instruction(stmts[i])->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::iterator it = opsList.begin();
      string opsName="";
      for (;it!=opsList.end();++it) {
	opsName += boost::lexical_cast<std::string>(unparseX86Expression(*it, false) );
      }
      codeTableWidget->setText(boost::lexical_cast<std::string>(opsName), 2, i);	
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmx86Instruction(stmts[i]))->get_comment() ), 3, i);
      addRow=true;
    } else if (isSgAsmBlock(stmts[i])  && !(isSgAsmInterpretation(isSgAsmBlock(stmts[i])->get_parent()))) {
    //cerr << " isSgAsmBlock(stmts[i])->get_parent() " << isSgAsmBlock(stmts[i])->get_parent()->class_name() << endl;
      codeTableWidget->addRows(1);
      codeTableWidget->setTextColor(QColor(128,128,128),0,i);
      codeTableWidget->setTextColor(QColor(255,255,0),1,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmBlock(stmts[i]))->get_address()) ), 0, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("***"), 1, i);
      addRow=true;
    } else if (isSgAsmFunctionDeclaration(stmts[i]),i) {
      codeTableWidget->addRows(1);
      //      codeTableWidget->addRows(1);
      codeTableWidget->setTextColor(QColor(0,0,0),0,i);
      codeTableWidget->setTextColor(QColor(0,0,0),1,i);
      codeTableWidget->setTextColor(QColor(0,0,0),2,i);
      codeTableWidget->setText(boost::lexical_cast<std::string>(RoseBin_support::HexToString((isSgAsmFunctionDeclaration(stmts[i]))->get_address()) ), 0, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>("FUNC"), 1, i);
      codeTableWidget->setText(boost::lexical_cast<std::string>((isSgAsmFunctionDeclaration(stmts[i]))->get_name() ), 2, i);
      addRow=true;
    } else {
      codeTableWidget->addRows(1);
      cerr << " FOUND UNEXPECTED NODE " << stmts[i]->class_name() << endl;
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

    }
  }

  selectView(0);
     
  for(size_t i=0; i < stmts.size(); i++ ) 
    unhighlightInstructionRow(i);  
  for(size_t i=pos; i < (pos+elem.size); i++ ) 
    highlightInstructionRow(i);  


  codeTableWidget->setShowGrid(false);
  codeTableWidget->setCurrentCell(pos+elem.size,0);
  QROSE::link(codeTableWidget, SIGNAL(activated(int, int, int, int)), 
	      &codeTableWidgetCellActivated, this);

}


BinQGUI::~BinQGUI(){
  if( window ) delete window;
} //BinQGUI::~BinQGUI()



