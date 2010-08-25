#include "rose.h"
#include <QPainter>
#include <QMouseEvent>
#include "MyBars.h"
#include <QScrollBar>

using namespace Qt;
using namespace std;

MyBars::MyBars(BinQGUI* g,
               QWidget *parent)
  : QWidget(parent) // cant get this to work
{
  gui = g;
  setPalette(QPalette(QColor(250, 250, 250)));
  setAutoFillBackground(true);
  posX=0;
  posY=0;
  maxX=0;
  offset=0;
  offsetY=0;
}

void
MyBars::colorize() {
  ROSE_ASSERT(gui);
  update();
}



void MyBars::paintEvent(QPaintEvent * /* event */)
{
  QPainter painter(this);
  painter.setPen(Qt::NoPen);
  ROSE_ASSERT(gui);
  std::vector<Item*> ite = gui->itemsFileA;
  std::vector<Item*>::const_iterator it=gui->itemsFileA.begin();
  int pos=0;
  maxX=0;
  bool even=true;
  int yoffset=0;

  for (;it!=gui->itemsFileA.end();++it) {
    Item* item = *it;
    if (item) {
      pos = item->pos;
      SgAsmElfSection* sec = isSgAsmElfSection(item->statement);
      if (sec) {
	QString name = QString("%1")
	  .arg(sec->get_name()->get_string().c_str());
	int size =  item->realByteSize;
	//	int REALPOS = sec->get_offset();
	size=size/10;
	if (even)
	  painter.setBrush(Qt::yellow);
	else
	  painter.setBrush(Qt::gray);
	even=!even;
	painter.drawRect(QRect(-offset, yoffset-offsetY, size, 15));
	painter.setPen(QColor(0, 0, 0));
	painter.drawText(0, 12+yoffset-offsetY, name);
	yoffset+=16;
	if ( size > maxX) 
	  maxX= size+20;
      }
    }
  }

  //  cerr << " new maxX : " << maxX << " screenwidth: " << gui->screenWidth << endl;
  gui->bar2->setRange(0,maxX);//-(gui->screenWidth)/1.2);
  gui->bar3->setRange(0,yoffset-200);//-(gui->screenWidth)/1.2);
  

}

void MyBars::setValue(int value) {
  //  cerr << "posX: " << posX<< " maxX:" << maxX<<"  -  value slider :  " << value << endl;
  offset=value;
  update();
  updateMouse();
}

void MyBars::setValueY(int value) {
  //  cerr << "posX: " << posX<< " maxX:" << maxX<<"  -  value slider :  " << value << endl;
  offsetY=value;
  update();
  updateMouse();
}

void MyBars::mouseMoveEvent( QMouseEvent *mevt )
{
  posX=mevt->pos().x();
  posY=mevt->pos().y();
  updateMouse();
}

void MyBars::updateMouse() {
}
