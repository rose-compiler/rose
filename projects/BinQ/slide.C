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
  lastStringA=QString("  ");
  lastRowA=-1;
  lastStringB=QString("  ");
  lastRowB=-1;
  gui = g;
  setPalette(QPalette(QColor(250, 250, 250)));
  setAutoFillBackground(true);
  posX=0;
  posY=0;
}

void
Slide::colorize() {
  ROSE_ASSERT(gui);
  update();
}

void Slide::paintEvent(QPaintEvent * /* event */)
{
  QPainter painter(this);
  painter.setPen(Qt::NoPen);
  ROSE_ASSERT(gui);
  std::vector<Item*> ite = gui->itemsFileA;
  std::vector<Item*>::const_iterator it=gui->itemsFileA.begin();
  int pos=0;
  for (;it!=gui->itemsFileA.end();++it) {
    Item* item = *it;
    int pos = item->pos;
    int length=item->length;
    int color = item->resolved;
    if (color==0)   painter.setBrush(Qt::black);
    if (color==1)   painter.setBrush(Qt::blue);
    if (color==2)   painter.setBrush(Qt::green);
    if (color==3)   painter.setBrush(Qt::red);
    if (color==4)   painter.setBrush(Qt::gray);
    painter.drawRect(QRect(pos, 0, length, 15));
  }

  ite = gui->itemsFileB;
  it=gui->itemsFileB.begin();
  pos=0;
  for (;it!=gui->itemsFileB.end();++it) {
    Item* item = *it;
    int pos = item->pos;
    int length=item->length;
    int color = item->resolved;
    if (color==0)   painter.setBrush(Qt::black);
    if (color==1)   painter.setBrush(Qt::blue);
    if (color==2)   painter.setBrush(Qt::green);
    if (color==3)   painter.setBrush(Qt::red);
    if (color==4)   painter.setBrush(Qt::gray);
    painter.drawRect(QRect(pos, 15, length, 15));
  }

  painter.setPen(Qt::white);
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(QRect(posX, 0, (1), 30));

}


void Slide::mouseMoveEvent( QMouseEvent *mevt )
{
  posX=mevt->pos().x();
  posY=mevt->pos().y();
  int selected = posX;
  Item* item = gui->byteItemFileA[selected];
  Item* item2 = gui->byteItemFileB[selected];


  if (item) {
    SgAsmStatement* stmt=isSgAsmStatement(item->statement);
    if (stmt) {
      if (isSgAsmFunctionDeclaration(stmt)) {
	QString res = QString("FILE_A: selected Function  %1    pos:%2")
	  .arg(isSgAsmFunctionDeclaration(stmt)->get_name().c_str())
	  .arg(selected);
	if (lastStringA!=res) {
	  lastStringA = res;
	  gui->analysisResult->append(res);
	}
      } else if (isSgAsmBlock(stmt)) {
	QString res = QString("FILE_A: selected Block");
	if (lastStringA!=res) {
	  lastStringA = res;
	  gui->analysisResult->append(res);
	}
      } else if (isSgAsmInstruction(stmt)) {
	//cerr << " selected Byte: " << isSgAsmInstruction(stmt)->get_mnemonic() << endl;
	QString res = QString("FILE_A: selected Byte  %1: %2  size %3  pos: %4")
	  .arg(RoseBin_support::HexToString((isSgAsmx86Instruction(stmt))->get_address()).c_str() )
	  .arg(	unparseInstruction(isSgAsmInstruction(stmt)).c_str())
	  .arg(isSgAsmInstruction(stmt)->get_raw_bytes().size())
	  .arg(selected);
	if (lastStringA!=res) {
	  lastStringA = res;
	  gui->analysisResult->append(res);
	  int row = item->row;
	  //cerr << "Selected row: " << row << "   lastRowA:" << lastRowA << endl;
	  if (row>=0) {
	    if (lastRowA!=row) {
	      gui->unhighlightInstructionRow(lastRowA, true);
	      gui->highlightInstructionRow(row, true);
	      lastRowA=row;
	    }
	  }
	}
      } // if
    } //if stmt
  }

  if (item2) {
    SgAsmStatement* stmt=isSgAsmStatement(item2->statement);
    if (stmt) {
      if (isSgAsmFunctionDeclaration(stmt)) {
	QString res = QString("FILE_B: selected Function  %1    pos:%2")
	  .arg(isSgAsmFunctionDeclaration(stmt)->get_name().c_str())
	  .arg(selected);
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->analysisResult->append(res);
	}
      } else if (isSgAsmBlock(stmt)) {
	QString res = QString("FILE_B: selected Block");
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->analysisResult->append(res);
	}
      } else if (isSgAsmInstruction(stmt)) {
	//cerr << " selected Byte: " << isSgAsmInstruction(stmt)->get_mnemonic() << endl;
	QString res = QString("FILE_B: selected Byte  %1: %2  size %3  pos: %4")
	  .arg(RoseBin_support::HexToString((isSgAsmx86Instruction(stmt))->get_address()).c_str() )
	  .arg(	unparseInstruction(isSgAsmInstruction(stmt)).c_str())
	  .arg(isSgAsmInstruction(stmt)->get_raw_bytes().size())
	  .arg(selected);
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->analysisResult->append(res);
	  int row = item2->row;
	  //cerr << "Selected row: " << row << "   lastRowB:" << lastRowB << endl;
	  if (row>=0) {
	    if (lastRowB!=row) {
	      gui->unhighlightInstructionRow(lastRowB, false);
	      gui->highlightInstructionRow(row, false);
	      lastRowB=row;
	    }
	  }
	}
      } // if
    } //if stmt
  }

  update();
}
