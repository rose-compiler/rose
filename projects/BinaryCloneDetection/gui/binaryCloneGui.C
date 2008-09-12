#include <boost/algorithm/string.hpp>
#include "binaryCloneGui.h"

#include <sstream>
#include <QFileDialog>

#include <unistd.h>

#include "icons.h"

#include "disks.xpm"

#define EMACS

using namespace qrs;

void toolbarClick(int action)
{

}


static void tableCellActivated(int col, int row, int oldCol, int oldRow) 
{
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
    toolbar->addButton("quit"); toolbar->setPicture(2, iconQuit);

    QROSE::link(toolbar, SIGNAL(clicked(int)), &toolbarClick, this);

  } //window 



  tableWidget = *window << new QRTable( 3, "Checker", "Timing", "Violations" );
  QROSE::link(tableWidget, SIGNAL(activated(int, int, int, int)), &tableCellActivated, this);

   codeWidget = *window << new QTextEdit;//new QREdit(QREdit::Box);
   codeWidget->setReadOnly(true);
 codeWidget->setHtml(QString("foobar\nBar\nFoobar"));

  window->setGeometry(0,0,1280,800);
  window->setTitle("BinaryCloneMainGui (QROSE)");

} //BinaryCloneGui::BinaryCloneGui()

BinaryCloneGui::~BinaryCloneGui()
{
  if( window ) delete window;

} //BinaryCloneGui::~BinaryCloneGui()



