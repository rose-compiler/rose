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
  BinQGUI(std::string, std::string,     
	  std::vector<std::string> dllA,     
	  std::vector<std::string> dllB, 
	  bool test);
    ~BinQGUI();

    // public functions ---- GUI
    void run( ) ;
    void open();
    void reset();
    // handling of interaction
    void highlightFunctionRow(int,bool fileA);
    void unhighlightFunctionRow(int,bool fileA);
    void highlightInstructionRow(int,bool fileA);
    void unhighlightInstructionRow(int,bool fileA);
    // GUI
    void showFile(int row,qrs::QRTable *currentWidget, 
		  std::vector<SgNode*>& funcsFile, std::vector<Item*>& itemsFile);

    // internal data that stores information of each table entry
    std::vector<Item*> itemsFileA;
    std::vector<Item*> itemsFileB;
    std::map<int,Item*> byteItemFileA;
    std::map<int,Item*> byteItemFileB;
    std::map<int,Item*> rowItemFileA;
    std::map<int,Item*> rowItemFileB;
    // GUI elements that need to be accesses within this object but also from 
    // analyses implemented for BinQ
    QTextEdit *console;
    QTextEdit *analysisResult;
    QTextEdit *fileInfo;
    QTextEdit *sectionInfo;
    QListWidget *listWidget;
    QTextEdit *analysisInfo;
    // the two main files
    SgNode* fileA;
    SgNode* fileB;
    // the dll files
    std::vector<SgNode*> dllFilesA;
    std::vector<SgNode*> dllFilesB;

    // elements in the GUI used for scrolling and painting
    Slide *slide;
    int maxrows;
    QScrollBar* bar;
    QScrollBar* bar2;
    QScrollBar* bar3;
    int screenWidth;

    // main GUI elements
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRTable *codeTableWidget;
    QTabWidget *analysisTab;
    qrs::QRTable *tableWidget2;
    qrs::QRTable *codeTableWidget2;
    //qrs::QRTable *currentWidget;
    std::vector<BinAnalyses*> analyses;
    BinAnalyses* currentAnalysis;

    // filenames for both files
    std::string fileNameA,fileNameB;
    
    // functions that implement GUI element functionality
    void updateByteItemList();
    void insertSectionInformation(SgAsmElfSection* sec,std::string filename);
    void insertFileInformation();
  protected:

  private:
    // used for testing
    bool test;
    void testAnalyses();
    // holds function information that is displayed in function tables
    std::vector<SgNode*> funcsFileA;
    std::vector<SgNode*> funcsFileB;

    // support class
    BinQSupport* binqsupport;

    // support functions
    void showFileTab();
    void init();
    void createGUI();
    bool sourceFile;
    int screenHeight;
    std::vector<std::string> dllA;
    std::vector<std::string> dllB;
   
}; //class BinQGUI

#endif
