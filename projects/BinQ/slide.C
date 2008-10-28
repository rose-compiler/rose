#include "rose.h"
#include <QPainter>
#include <QMouseEvent>
#include "slide.h"

using namespace Qt;
using namespace std;

Slide::Slide(BinQGUI* g,
	     QWidget *parent)
  //  : QWidget(parent)
{
  lastString=QString("  ");
  lastRow=-1;
  //step =s;
  gui = g;
  setPalette(QPalette(QColor(250, 250, 200)));
  setAutoFillBackground(true);
  posX=0;
  posY=0;
}

void
Slide::colorize(BinQGUI* g) {
  gui=g;
  update();
}

void Slide::paintEvent(QPaintEvent * /* event */)
{
  QPainter painter(this);
  painter.setPen(Qt::NoPen);


  std::vector<Item*> ite = gui->itemsFileA;
  std::vector<Item*>::const_iterator it=gui->itemsFileA.begin();
  int pos=0;
  ite = gui->itemsFileA;
  it=gui->itemsFileA.begin();
  pos=0;
  for (;it!=gui->itemsFileA.end();++it) {
    Item* item = *it;
    int length=1;
    SgAsmStatement* stmt = item->statement;
    if (isSgAsmInstruction(stmt))
      length = isSgAsmInstruction(stmt)->get_raw_bytes().size();
    int color = item->resolved;
    if (color==0)   painter.setBrush(Qt::black);
    if (color==1)   painter.setBrush(Qt::blue);
    if (color==2)   painter.setBrush(Qt::green);
    if (color==3)   painter.setBrush(Qt::red);
    painter.drawRect(QRect(pos, 0, length, 15));
    pos+=length;
  }

  ite = gui->itemsFileB;
  it=gui->itemsFileB.begin();
  pos=0;
  for (;it!=gui->itemsFileB.end();++it) {
    Item* item = *it;
    int length=1;
    SgAsmStatement* stmt = item->statement;
    if (isSgAsmInstruction(stmt))
      length = isSgAsmInstruction(stmt)->get_raw_bytes().size();
    int color = item->resolved;
    if (color==0)   painter.setBrush(Qt::black);
    if (color==1)   painter.setBrush(Qt::blue);
    if (color==2)   painter.setBrush(Qt::green);
    if (color==3)   painter.setBrush(Qt::red);
    painter.drawRect(QRect(pos, 15, length, 15));
    pos+=length;
  }

  painter.setPen(Qt::white);
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(QRect(posX, 0, (1), 30));

}


void Slide::mouseMoveEvent( QMouseEvent *mevt )
{
  posX=mevt->pos().x();
  posY=mevt->pos().y();
  //  cerr << " posX = " << posX << " posY="<<posY<<endl;
  int selected = 0;
  //posX/step;
  /*
  SgAsmStatement* stmt=NULL;
  if (selected>0 && selected<gui->itemsFileA.size())
    stmt=gui->itemsFileA[selected]->statement;
  if (stmt) {
    //    cerr << " selected Item: " << stmt->class_name() << endl;
  }
  */
  selected=posX;
  Item* item = gui->byteItemFileA[selected];
  if (item) {
    SgAsmStatement* stmt=isSgAsmStatement(item->statement);
    if (stmt) {
      if (isSgAsmFunctionDeclaration(stmt)) {
	QString res = QString("FILE_A: selected Function  %1    pos:%2")
	  .arg(isSgAsmFunctionDeclaration(stmt)->get_name().c_str())
	  .arg(selected);
	//	cerr << " selected Byte Function: " << isSgAsmFunctionDeclaration(stmt)->get_name() << endl;
	if (lastString!=res) {
	  lastString = res;
	  gui->analysisResult->append(res);
	}
      } else if (isSgAsmBlock(stmt)) {
	QString res = QString("FILE_A: selected Block");
	if (lastString!=res) {
	  lastString = res;
	  gui->analysisResult->append(res);
	}
      } else if (isSgAsmInstruction(stmt)) {
	//cerr << " selected Byte: " << isSgAsmInstruction(stmt)->get_mnemonic() << endl;
	QString res = QString("FILE_A: selected Byte  %1: %2  size %3  pos: %4")
	  .arg(RoseBin_support::HexToString((isSgAsmx86Instruction(stmt))->get_address()).c_str() )
	  .arg(	unparseInstruction(isSgAsmInstruction(stmt)).c_str())
	  .arg(isSgAsmInstruction(stmt)->get_raw_bytes().size())
	  .arg(selected);
	if (lastString!=res) {
	  lastString = res;
	  gui->analysisResult->append(res);
	  // need to convert pos to row
	  // 
	  int row = gui->posRowA[selected];
	  //cerr << "Selected row: " << row << "   lastRow:" << lastRow << endl;
	  if (row>=0) {
	    if (lastRow!=row) {
	      gui->unhighlightInstructionRow(lastRow, true);
	      gui->highlightInstructionRow(row, true);
	      lastRow=row;
	    }
	  }
	}
      }
    }
  }

  update();
}
