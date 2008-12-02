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
#include "BinAnalyses.h"

class Slide;
class BinQSupport;


class BinQGUI //: public QWidget
{
  public:
  BinQGUI(std::string, std::string);
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
    std::map<int,Item*> rowItemFileA;
    std::map<int,Item*> rowItemFileB;
    QTextEdit *console;
    QTextEdit *analysisResult;
    QTextEdit *fileInfo;
    QTextEdit *sectionInfo;
    QListWidget *listWidget;
    QTextEdit *analysisInfo;
    SgNode* fileA;
    SgNode* fileB;
    Slide *slide;
    int maxrows;
    QScrollBar* bar;
    QScrollBar* bar2;
    QScrollBar* bar3;

    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRTable *codeTableWidget;
    QTabWidget *analysisTab;

    qrs::QRTable *tableWidget2;
    qrs::QRTable *codeTableWidget2;

    std::string fileNameA,fileNameB;
    
    void updateByteItemList();
    void insertSectionInformation(SgAsmElfSection* sec,std::string filename);
    void insertFileInformation();
    std::vector<BinAnalyses*> analyses;
    BinAnalyses* currentAnalysis;


    int screenWidth;
  protected:
    QComboBox *comboBox;
    qrs::QRProgress *wholeFunction;
    

    QTextBrowser *codeBrowser;
    QLineEdit *smallerThanRestriction;
    QLineEdit *largerThanRestriction;

  private:


    std::vector<SgNode*> funcsFileA;
    std::vector<SgNode*> funcsFileB;
    BinQSupport* binqsupport;
    void showFileTab();
    

    int screenHeight;
    void init();
    void createGUI();
    bool sourceFile;
   
}; //class BinQGUI

#endif
