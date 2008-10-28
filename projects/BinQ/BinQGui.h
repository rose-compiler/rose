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

#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <stdint.h>
#include "BinQSupport.h"
#include <QDesktopWidget>
#include <QApplication>

#include "Item.h"

class Slide;





template <typename T>
class scoped_array_with_size {
  boost::scoped_array<T> sa;
  size_t theSize;

  public:
  scoped_array_with_size(): sa(), theSize(0) {}
  scoped_array_with_size(size_t s): sa(new T[s]), theSize(s) {}

  void allocate(size_t s) {
    sa.reset(new T[s]);
    theSize = s;
  }
  size_t size() const {return theSize;}
  T* get() const {return sa.get();}

  T& operator[](size_t i) {return sa[i];}
  const T& operator[](size_t i) const {return sa[i];}

  private:
  scoped_array_with_size(const scoped_array_with_size<T>&); // Not copyable
};


class BinQGUI
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
    std::map<int,int> posRowA;
    std::map<int,int> posRowB;
    QTextEdit *analysisResult;

  protected:
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRTable *codeTableWidget;

    qrs::QRTable *tableWidget2;
    qrs::QRTable *codeTableWidget2;

    QComboBox *comboBox;
    qrs::QRProgress *wholeFunction;
    Slide *slide;
    QListWidget *listWidget;
    

    QTextBrowser *codeBrowser;
    QLineEdit *smallerThanRestriction;
    QLineEdit *largerThanRestriction;

  private:

    std::vector<SgAsmFunctionDeclaration*> funcsFileA;
    std::vector<SgAsmFunctionDeclaration*> funcsFileB;
    BinQSupport* binqsupport;
    
    std::string fileNameA,fileNameB;
    SgNode* fileA;
    SgNode* fileB;

    int screenWidth;
    int screenHeight;

   
}; //class BinQGUI

#endif
