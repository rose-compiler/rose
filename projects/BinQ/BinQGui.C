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

static void tableCellActivated(int col, int row, int oldCol, int oldRow) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->unhighlightRow(oldRow);
  instance->highlightRow(row);
  return;
} //tableCellActivated(int col, int row, int oldCol, int oldRow)

static void viewBoxActivated(int selection) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->selectView(selection);
  return;
}

static void selectLockOrUnlockBars(int selection) {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();
  instance->lockBars(selection);
  return;
}
// ----------------------------------------------------------------------------------------------





// ----------------------------------------------------------------------------------------------
void
BinQGUI::selectView(int selection) {
  switch (selection) {
  case 0:
    codeWidget->setHtml(QString(normalizedView.first.c_str()));
    codeWidget2->setHtml(QString(normalizedView.second.c_str()));
    break;
  case 1:
    codeWidget->setHtml(QString(allInsnsUnparsedView.first.c_str()));
    codeWidget2->setHtml(QString(allInsnsUnparsedView.second.c_str()));
    break;
  }
};


void
BinQGUI::lockBars(int selection) {
  switch(selection)
    {
    case 0:
      QROSE::disconnect_ex((const QObject*)(codeWidget->verticalScrollBar()), SIGNAL(valueChanged(int)),
			   (const QObject*)(codeWidget2->verticalScrollBar()), SLOT(setValue(int)));
      QROSE::disconnect_ex((const QObject*)(codeWidget2->verticalScrollBar()), SIGNAL(valueChanged(int)),
			   (const QObject*)(codeWidget->verticalScrollBar()), SLOT(setValue(int)));
      break;
    case 1:
      QROSE::connect_ex((const QObject*)(codeWidget->verticalScrollBar()), SIGNAL(valueChanged(int)),
			(const QObject*)(codeWidget2->verticalScrollBar()), SLOT(setValue(int)));
      QROSE::connect_ex((const QObject*)(codeWidget2->verticalScrollBar()), SIGNAL(valueChanged(int)),
			(const QObject*)(codeWidget->verticalScrollBar()), SLOT(setValue(int)));

      break;
    }
}


BinQGUI::BinQGUI(std::string fA, std::string fB ) :
  window(0), fileNameA(fA), fileNameB(fB) {
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

  QRPanel &tiledPanel = *window << *( new QRPanel(QROSE::TopDown, QROSE::UseSplitter) );
  {
    QRPanel &lowerInnerTiledPanel = tiledPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
    {
      tableWidget = lowerInnerTiledPanel << new QRTable( 2, "function","# instructions" );
      QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableCellActivated, this);
      QRPanel &parameters = lowerInnerTiledPanel << *( new QRPanel(QROSE::TopDown, QROSE::UseSplitter) );
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


      QLabel *lockBarsLabel = new QLabel("Lock ScrollBars:");
      checkBoxLockBars = new QComboBox;
      checkBoxLockBars->addItem(("No"));
      checkBoxLockBars->addItem(("Yes"));

      echoLayout->addWidget(lockBarsLabel,1,0);
      echoLayout->addWidget(checkBoxLockBars,1,1);
      echoGroup->setLayout(echoLayout);

      QROSE::link(checkBoxLockBars, SIGNAL(activated(int)), &selectLockOrUnlockBars, this);
      QROSE::link(comboBox, SIGNAL(activated(int)), &viewBoxActivated, this);

    }
      
    QRPanel &upperInnerTiledPanel = tiledPanel << *new QRPanel(QROSE::LeftRight, QROSE::UseSplitter);
    {
      codeWidget = upperInnerTiledPanel << new QTextEdit;//new QREdit(QREdit::Box);
      codeWidget->setReadOnly(true);
      codeWidget2 = upperInnerTiledPanel << new QTextEdit;//new QREdit(QREdit::Box);
      codeWidget2->setReadOnly(true);
    } //tiledPanel
    tiledPanel.setTileSize(40,60);
  } //window 

  window->setGeometry(0,0,1280,800);
  window->setTitle("BinaryCloneMainGui");
} //BinQGUI::BinQGUI()


void BinQGUI::open() {
  char buf[4096] = "\0";
  std::string database = QFileDialog::getOpenFileName( 0, "Open As", getcwd(buf, 4096), "ASCII (*.sql)").toStdString();
  if( database.empty() ) return;
  if(exists(database) == true)    {
    codeWidget->setReadOnly(true);
    codeWidget->setPlainText(QString("foobar\nBar\nFoobari3"));
  }
} //CompassGui::open()





void
BinQGUI::run( ) {
  //Fill in table
  // unlink activation callback, which otherwise would trigger each
  // time we add a row in the table.
  QROSE::unlink(tableWidget, SIGNAL(activated(int, int, int, int)));

  while(tableWidget->rowCount()) tableWidget->removeRow(0);
  vector<SgAsmFunctionDeclaration*> funcs;
  FindAsmFunctionsVisitor funcVis;
  AstQueryNamespace::querySubTree(fileA, std::bind2nd( funcVis, &funcs ));

  vectorOfClones.allocate(funcs.size());
  for (size_t row = 0; row < funcs.size(); ++row) {
    Element cur_elem;
    cur_elem.function_name_A = funcs[row]->get_name();
    cur_elem.function_name_B = funcs[row]->get_name();

    vector<SgAsmx86Instruction*> insns;
    FindInstructionsVisitor vis;

    AstQueryNamespace::querySubTree(funcs[row], std::bind2nd( vis, &insns ));
    cur_elem.size = insns.size();
    vectorOfClones[row] = cur_elem;
    tableWidget->addRows(1);
    tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.function_name_A), 0, row);
    tableWidget->setText(boost::lexical_cast<std::string>(cur_elem.size), 1, row);
    tableWidget->setHAlignment(true, false, 0); // left horizontal alignment
  }
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), 
	      &tableCellActivated, this);
}


  
void BinQGUI::highlightRow(int row) {
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

void BinQGUI::unhighlightRow(int row) {
  if (row >= 0) 
    {
      QFont f = tableWidget->getFont(0, row);
      f.setBold(false);
      tableWidget->setFont(f, 0, row);
    } //if (row >= 0)

  return;
} //CompassGui::unhighlightRow(int row)



void BinQGUI::showClone(int row) {
  //Disassemble files
  Element& elem = vectorOfClones[row];
  std::string unparsedFileA;
  std::string unparsedFileB;

  std::string normalizedFileA;
  std::string normalizedFileB;

  ROSE_ASSERT(fileA != NULL);
  ROSE_ASSERT(fileB != NULL);

  bool first =true;
  while(true)
    {
      SgNode* currentRoot = first ? fileA : fileB;
   
      vector<SgAsmx86Instruction*> insns;
      FindInstructionsVisitor vis;

      vector<SgAsmFunctionDeclaration*> funcs;
      FindAsmFunctionsVisitor funcVis;
      AstQueryNamespace::querySubTree(currentRoot, std::bind2nd( funcVis, &funcs ));

      for (size_t i = 0; i < funcs.size(); ++i) {
	if( funcs[i]->get_name() == (first ? elem.function_name_A : elem.function_name_B) )
	  {
	    AstQueryNamespace::querySubTree(funcs[i], std::bind2nd( vis, &insns ));
	    break;
	  }
      }

      std::cout << "Looking at function " << elem.function_name_B << std::endl;
      std::string allInsnsUnparsed;
      for(size_t i=0; i < insns.size(); i++ )    {
	allInsnsUnparsed += "<code>" + binqsupport->unparseX86InstructionToHTMLWithAddress(isSgAsmx86Instruction(insns[i])) + "</code>";
	allInsnsUnparsed += "<br> \n";
      }

      std::cout <<  "INSNS : "  << insns.size()  << std::endl ;
      std::string normalized = insns.size()>0 ? binqsupport->normalizeInstructionsToHTML(insns.begin(), insns.end()): ""; 

      (first? normalizedView.first : normalizedView.second ) = normalized;
      (first? allInsnsUnparsedView.first : allInsnsUnparsedView.second ) = allInsnsUnparsed;

      selectView(0);
     
      if(first == true) first = false;
      else break;
    }
}


BinQGUI::~BinQGUI(){
  if( window ) delete window;
} //BinQGUI::~BinQGUI()



