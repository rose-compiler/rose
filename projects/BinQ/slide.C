#include "rose.h"
#include <QPainter>
#include <QMouseEvent>
#include "slide.h"
#include <QScrollBar>

using namespace Qt;
using namespace std;

Slide::Slide(BinQGUI* g,
             QWidget *parent)
  : QWidget(parent) // cant get this to work
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
  maxX=0;
  offset=0;
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
    if (item) {
    pos = item->pos;
    int length=item->length;
    int color = item->resolvedColor;
    if (color==0)   painter.setBrush(Qt::black);
    if (color==1)   painter.setBrush(Qt::blue);
    if (color==2)   painter.setBrush(Qt::green);
    if (color==3)   painter.setBrush(Qt::red);
    if (color==4)   painter.setBrush(Qt::gray);
    if (color==5)   painter.setBrush(Qt::white);
    if (color==6)   painter.setBrush(Qt::darkRed);
    painter.drawRect(QRect(pos-offset, 0, length, 15));
    }
  }
  maxX=pos;

  ite = gui->itemsFileB;
  it=gui->itemsFileB.begin();
  pos=0;
  for (;it!=gui->itemsFileB.end();++it) {
    Item* item = *it;
    if (item) {
     pos = item->pos;
    int length=item->length;
    int color = item->resolvedColor;
    if (color==0)   painter.setBrush(Qt::black);
    if (color==1)   painter.setBrush(Qt::blue);
    if (color==2)   painter.setBrush(Qt::green);
    if (color==3)   painter.setBrush(Qt::red);
    if (color==4)   painter.setBrush(Qt::gray);
    if (color==5)   painter.setBrush(Qt::white);
    if (color==6)   painter.setBrush(Qt::darkRed);
    painter.drawRect(QRect(pos-offset, 15, length, 15));
    }
  }
  if (pos>maxX) maxX=pos;
  //  cerr << " new maxX : " << maxX << " screenwidth: " << gui->screenWidth << endl;
  gui->bar->setRange(0,maxX-(gui->screenWidth)/1.2);
  
  painter.setPen(Qt::white);
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(QRect(posX, 0, (1), 30));

}

void Slide::setValue(int value) {
  //cerr << "posX: " << posX<< " maxX:" << maxX<<"  -  value slider :  " << value << endl;
  offset=value;
  update();
  updateMouse();
}

void Slide::mouseMoveEvent( QMouseEvent *mevt )
{
  posX=mevt->pos().x();
  posY=mevt->pos().y();
  updateMouse();
}

void Slide::updateMouse() {
  int selected = posX+offset;
  Item* item = gui->byteItemFileA[selected];
  Item* item2 = gui->byteItemFileB[selected];
  ROSE_ASSERT(gui);

  if (item) {
    SgAsmNode* stmt=isSgAsmNode(item->statement);
    if (stmt) {
      if (isSgAsmFunctionDeclaration(stmt)) {
	QString res = QString("FILE_A: selected Function  %1    pos:%2")
	  .arg(isSgAsmFunctionDeclaration(stmt)->get_name().c_str())
	  .arg(selected);
	if (lastStringA!=res) {
	  lastStringA = res;
	  gui->console->append(res);
	}
      } else if (isSgAsmBlock(stmt)) {
	QString res = QString("FILE_A: selected Block");
	if (lastStringA!=res) {
	  lastStringA = res;
	  gui->console->append(res);
	}
      } else if (isSgAsmElfSection(stmt) ||
                 isSgAsmElfSectionTableEntry(stmt) ||
		 isSgAsmElfSegmentTableEntry(stmt)) {
        QString res = QString("");
	if (isSgAsmElfSection(stmt))
	  res = QString("FILE_A: selected Section %1")
	    .arg(isSgAsmElfSection(stmt)->get_name()->get_string().c_str());
	if (lastStringA!=res) {
	  lastStringA = res;
	  gui->console->append(res);
	  int row = item->row;
	  //cerr << "Selected row: " << row << "   lastRowA:" << lastRowA << endl;
	  if (row>=0) {
	    if (lastRowA!=row) {
	      gui->unhighlightInstructionRow(lastRowA, gui->codeTableWidget);
	      gui->highlightInstructionRow(row,  gui->codeTableWidget);
	      lastRowA=row;
	    }
	  }
	}
      } else if (isSgAsmElfSymbol(stmt)) {
	QString res = QString("FILE_A: selected Symbol %1")
	.arg(isSgAsmElfSymbol(stmt)->get_name()->get_string().c_str());
	if (lastStringA!=res) {
	  lastStringA = res;
	  gui->console->append(res);
	  int row = item->row;
	  //cerr << "Selected row: " << row << "   lastRowA:" << lastRowA << endl;
	  if (row>=0) {
	    if (lastRowA!=row) {
	      gui->unhighlightInstructionRow(lastRowA, gui->codeTableWidget);
	      gui->highlightInstructionRow(row,  gui->codeTableWidget);
	      lastRowA=row;
	    }
	  }
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
	  gui->console->append(res);
	  int row = item->row;
	  //cerr << "Selected row: " << row << "   lastRowA:" << lastRowA << endl;
	  if (row>=0) {
	    if (lastRowA!=row) {
	      gui->unhighlightInstructionRow(lastRowA, gui->codeTableWidget);
	      gui->highlightInstructionRow(row,  gui->codeTableWidget);
	      lastRowA=row;
	    }
	  }
	}
      } // if
    } //if stmt
  }

  if (item2) {
    SgNode* stmt=item2->statement;
    if (stmt) {
      if (isSgAsmFunctionDeclaration(stmt)) {
	QString res = QString("FILE_B: selected Function  %1    pos:%2")
	  .arg(isSgAsmFunctionDeclaration(stmt)->get_name().c_str())
	  .arg(selected);
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->console->append(res);
	}
      } else if (isSgAsmBlock(stmt)) {
	QString res = QString("FILE_B: selected Block");
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->console->append(res);
	}
      } else if (isSgFunctionDeclaration(stmt)) {
	QString res = QString("FILE_B: Func selected");
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->console->append(res);
	}
      } else if (isSgStatement(stmt)) {
	//	cerr << " selected statement!! " << endl;
	QString res = QString("FILE_B: selected Byte  %1: %2  size %3  pos: %4")
	  .arg(QString(" ") )
	  .arg(	isSgStatement(stmt)->class_name().c_str())
	  .arg(QString(" "))
	  .arg(selected);
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->console->append(res);
	  int row = item2->row;
	  //cerr << "Selected row: " << row << "   lastRowB:" << lastRowB << endl;
	  if (row>=0) {
	    if (lastRowB!=row) {
	      gui->unhighlightInstructionRow(lastRowB, gui->codeTableWidget2);
	      gui->highlightInstructionRow(row,  gui->codeTableWidget2);
	      lastRowB=row;
	    }
	  }
	}
      }else if (isSgAsmElfSection(stmt) ||
                isSgAsmElfSectionTableEntry(stmt) ||
                isSgAsmElfSegmentTableEntry(stmt)) {
        QString res = QString("");
	if (isSgAsmElfSection(stmt))
	  res = QString("FILE_B: selected Section %1")
	.arg(isSgAsmElfSection(stmt)->get_name()->get_string().c_str());
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->console->append(res);
	  int row = item2->row;
	  //cerr << "Selected row: " << row << "   lastRowA:" << lastRowA << endl;
	  if (row>=0) {
	    if (lastRowB!=row) {
	      gui->unhighlightInstructionRow(lastRowB, gui->codeTableWidget2);
	      gui->highlightInstructionRow(row,  gui->codeTableWidget2);
	      lastRowB=row;
	    }
	  }
	}
      }else if (isSgAsmElfSymbol(stmt)) {
	QString res = QString("FILE_B: selected Symbol %1")
	.arg(isSgAsmElfSymbol(stmt)->get_name()->get_string().c_str());
	if (lastStringB!=res) {
	  lastStringB = res;
	  gui->console->append(res);
	  int row = item2->row;
	  //cerr << "Selected row: " << row << "   lastRowA:" << lastRowA << endl;
	  if (row>=0) {
	    if (lastRowB!=row) {
	      gui->unhighlightInstructionRow(lastRowB, gui->codeTableWidget2);
	      gui->highlightInstructionRow(row,  gui->codeTableWidget2);
	      lastRowB=row;
	    }
	  }
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
	  gui->console->append(res);
	  int row = item2->row;
	  //cerr << "Selected row: " << row << "   lastRowB:" << lastRowB << endl;
	  if (row>=0) {
	    if (lastRowB!=row) {
	      gui->unhighlightInstructionRow(lastRowB, gui->codeTableWidget2);
	      gui->highlightInstructionRow(row,  gui->codeTableWidget2);
	      lastRowB=row;
	    }
	  }
	}
      } // if
    } //if stmt
  }

  update();
}
