#ifndef COMPASS_GUI_H
#define COMPASS_GUI_H

#include <qrose.h>
#include <QTextBrowser>

class BinaryCloneGui
{
  public:
    BinaryCloneGui( );
    ~BinaryCloneGui();
 
  protected:
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    QTextEdit *codeWidget;

  private:
}; //class BinaryCloneGui

#endif
