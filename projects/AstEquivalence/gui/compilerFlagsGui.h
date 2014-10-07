#ifndef COMPASS_GUI_H
#define COMPASS_GUI_H


#include <qrose.h>
#include <QTextBrowser>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include "sqlite3x.h"

#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <stdint.h>

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



struct Element {
  uint64_t function_A ;
  uint64_t function_B ;
  uint64_t begin_index_within_function_A ;
  uint64_t end_index_within_function_A   ;
  uint64_t begin_index_within_function_B ;
  uint64_t end_index_within_function_B   ;
  std::string file_A;
  std::string function_name_A;
  std::string file_B;
  std::string function_name_B;

};


class BinaryCloneGui
{
  public:
    BinaryCloneGui(std::string dbA, std::string dbB );
    ~BinaryCloneGui();
    void run( ) ;
    void open();
    void highlightRow(int);
    void unhighlightRow(int);

    void showClone(int row);
    SgNode* disassembleFile(std::string tsv_directory);
    std::string normalizeInstructionsToHTML(std::vector<SgAsmX86Instruction*>::iterator beg, 
    std::vector<SgAsmX86Instruction*>::iterator end);

    std::pair<std::string,std::string> getAddressFromVectorsTable(uint64_t function_id, uint64_t index);

    void selectView(int selection);

    void lockBars(int selection);

  protected:
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    QTextEdit *codeWidget;
    QTextEdit *codeWidget2;
    QComboBox *comboBox;
    QComboBox *wholeFunction;
    QComboBox *dbAorB;
    QComboBox *displayResults;

    QTextBrowser *codeBrowser;
    QLineEdit *fileDataRestriction;
    QLineEdit *functionDataRestriction;
    QComboBox *checkBoxLockBars;
  private:

    std::string dbA, dbB;

    double similarity;
    int stride;
    int windowSize;
    
    int activeRow;

    std::string relativePathPrefix;
    sqlite3x::sqlite3_connection conA;
    sqlite3x::sqlite3_connection conB;

    std::map<int,int> mapAtoB;
    std::map<int,int> mapBtoA;
    std::map<int,int> mapRowtoDB;

    scoped_array_with_size<Element > vectorOfClonesA;
    scoped_array_with_size<Element > vectorOfClonesB;

    void 
    readIntoVector( sqlite3x::sqlite3_connection& cur_con,
                                scoped_array_with_size<Element >&  cur_vec );

    void insert_into_table_row(int row, Element&);

    std::pair<std::string,std::string> normalizedView;
    std::pair<std::string,std::string> unparsedView;
    std::pair<std::string,std::string> allInsnsUnparsedView;
   
}; //class BinaryCloneGui

#endif
