#ifndef CANNONFIELD_H
#define CANNONFIELD_H

#include "rose.h"
#include <QWidget>

#include "Item.h"
#include "BinQGui.h"
//class BinQGUI;

class Slide : public QWidget
{
  //     Q_OBJECT

 public:
  Slide(int step, BinQGUI* gui, QWidget *parent = 0);
  void colorize(BinQGUI* gui);

 protected:
  void paintEvent(QPaintEvent *event);
  void mouseMoveEvent( QMouseEvent *mevt );

  
 private:
  int posX;
  int posY;
  int step;
  BinQGUI* gui;
  QString lastString;
  int lastRow;
};

#endif
