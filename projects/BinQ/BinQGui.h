#ifndef BinQT_GUI_H
#define BinQT_GUI_H
//#include "rose.h"

#include "BinQAbstract.h"


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

//#include "Item.h"
//#include "BinAnalyses.h"

class Slide;
class BinQSupport;

class DeleteAST : public SgSimpleProcessing {
 public:
  //! Required traversal function
  void visit (SgNode* node) {
    if (node) {
      // tps , 2March 2009
      // This node can't be deleted for some reason?!
      if (!isSgAsmExecutableFileFormat(node)) {
      	delete node;
      }
    }
  }
};



class BinQGUI : public BinQAbstract
{
  public:
  //  BinQGUI(std::string, std::string,     
  //	  std::vector<std::string> dllA,     
  //	  std::vector<std::string> dllB, 
  //	  bool test);
  BinQGUI();
  ~BinQGUI() {
    if (fileA) {
      DeleteSgTree(fileA);
      delete fileA;
    }
    if (fileB) {
      DeleteSgTree(fileB);
      delete fileB;
    }

    itemsFileA.clear();
    itemsFileB.clear();
    byteItemFileA.clear();
    byteItemFileB.clear();
    rowItemFileA.clear();
    rowItemFileB.clear();


    if( window ) 
      delete window;

    if (currentSelectedFile)
      delete currentSelectedFile;
    if (currentAnalysis)
      delete currentAnalysis;

    
  };
  
    void DeleteSgTree(SgNode* root);
    // public functions ---- GUI
    void run( ) ;
    void open();
    void reset();
    // handling of interaction
    void highlightFunctionRow(int, qrs::QRTable* widget);
    void unhighlightFunctionRow(int, qrs::QRTable* widget);
    void highlightInstructionRow(int, qrs::QRTable* widget);
    void unhighlightInstructionRow(int, qrs::QRTable* widget);
    // GUI
    void showFile(int row,qrs::QRTable *currentWidget, 
		  std::vector<SgNode*>& funcsFile, std::vector<Item*>& itemsFile);
    //void initAnalyses();
    // GUI elements that need to be accesses within this object but also from 
    // analyses implemented for BinQ
    QTextEdit *console;
    QTextEdit *analysisResult;
    QTextEdit *fileInfo;
    QTextEdit *sectionInfo;
    QListWidget *listWidget;
    QTextEdit *analysisInfo;


    // elements in the GUI used for scrolling and painting
    Slide *slide;
    int maxrows;
    QScrollBar* bar;
    QScrollBar* bar2;
    QScrollBar* bar3;


    // main GUI elements
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRTable *codeTableWidget;
    qrs::QRTable *codeTableWidgetDLL;
    qrs::QRTable *currentTableWidget;
    std::vector<qrs::QRTable*> codeTableWidgetADLLlist;
    std::vector<qrs::QRTable*> codeTableWidgetBDLLlist;
    QTabWidget *analysisTab;
    QTabWidget *codeTabA;
    QTabWidget *codeTabB;
    qrs::QRTable *tableWidget2;
    qrs::QRTable *codeTableWidget2;
    //qrs::QRTable *currentWidget;
    std::map< qrs::QRTable*, SgNode*> getFileForWidget;

    
    // functions that implement GUI element functionality
    void updateByteItemList();
    void insertSectionInformation(SgAsmElfSection* sec,std::string filename);
    void insertFileInformation();

    // testing several analyses at once
    std::map<std::string, int> getTestAnalysisResults();
    double getTestAnalysisTime();

  protected:
    // used for testing
    void testAnalyses(std::vector<BinAnalyses*>& analysesVec);
    // support functions
    void showFileTab();
    void createGUI();

}; //class BinQGUI

#endif
