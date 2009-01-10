#ifndef COMPASS_GUI_H
#define COMPASS_GUI_H

#include <qrose.h>

#include <QTextBrowser>

#include "compassViolation.h"
#include "compassInterface.h"

#include <map>

class CompassGui
{
  public:
    CompassGui( CompassInterface & ci );
    ~CompassGui();
    void updateTableWidget();
    void run();
    void refresh();
    void reset();
    void select(int);
    void sort(int);
    void save();

    void highlightRow(int);
    void unhighlightRow(int);

    void showDescription(int);
    void showViolations(int);

    void launchEditor(unsigned int);
    void writeCodeWidget(unsigned int);

  protected:
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRSelect *violationsWidget;
    qrs::QREdit *descriptionWidget;
    QTextEdit *codeWidget;

  private:
    CompassInterface & compassInterface;
    int sortType;
    int activeRow;
    std::map<std::string, std::string*> sourceCodesMap;
}; //class CompassGui

#endif
