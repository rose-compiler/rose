#ifndef MYBARS_H
#define MYBARS_H

#include <rose.h>
#include <QWidget>

#include "Item.h"
#include "BinQGui.h"
//class BinQGUI;
#include <qobject.h>

class MyBars : public QWidget
{

 public:
  MyBars(BinQGUI* gui, QWidget *parent = 0);
  ~MyBars(){};
  void colorize();
  int maxX;
  int offset;
  int offsetY;
  void updateMouse();
 
 signals:
  void valueChanged(int value);

  public slots:
    void setValue(int value) ;
    void setValueY(int value) ;
  
 protected:
  void paintEvent(QPaintEvent *event);
  void mouseMoveEvent( QMouseEvent *mevt );

 private:
    Q_OBJECT // cant get this to work
  
 private:

  int posX;
  int posY;
  BinQGUI* gui;
};

#endif
