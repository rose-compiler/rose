#ifndef COMPASS_GUI_H
#define COMPASS_GUI_H
#include "rose.h"

#include <qrose.h>
#include <QTextBrowser>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRProgress.h>

#include <QList>
#include <QListWidget>
#include <QListWidgetItem>

#include <QDesktopWidget>
#include <QApplication>

#include "Item.h"

class Slide;
class BinQSupport;


class BinQGUI //: public QWidget
{
  public:
    BinQGUI(std::string, std::string );
    ~BinQGUI();
    void run( ) ;
    void open();
    void reset();
    void highlightFunctionRow(int,bool fileA);
    void unhighlightFunctionRow(int,bool fileA);
    void highlightInstructionRow(int,bool fileA);
    void unhighlightInstructionRow(int,bool fileA);

    void showFileA(int row);
    void showFileB(int row);

    std::vector<Item*> itemsFileA;
    std::vector<Item*> itemsFileB;
    std::map<int,Item*> byteItemFileA;
    std::map<int,Item*> byteItemFileB;
    QTextEdit *analysisResult;
    QTextEdit *fileInfo;
    QListWidget *listWidget;
    SgNode* fileA;
    SgNode* fileB;
    Slide *slide;
    int maxrows;

    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRTable *codeTableWidget;

    qrs::QRTable *tableWidget2;
    qrs::QRTable *codeTableWidget2;

    bool findPosWhenFunctionsAreNotSync(int& position, int& offset, int& currentPos);
    void updateByteItemList();

  protected:
    QComboBox *comboBox;
    qrs::QRProgress *wholeFunction;
    

    QTextBrowser *codeBrowser;
    QLineEdit *smallerThanRestriction;
    QLineEdit *largerThanRestriction;

  private:
    void insertFileInformation();
    std::vector<SgAsmFunctionDeclaration*> funcsFileA;
    std::vector<SgAsmFunctionDeclaration*> funcsFileB;
    BinQSupport* binqsupport;
    
    std::string fileNameA,fileNameB;

    int screenWidth;
    int screenHeight;

   
}; //class BinQGUI

#endif
