#ifndef BINQA_GUI_H
#define BINQA_GUI_H
#include "rose.h"
#include <qrose.h>

#include "Item.h"
#include "BinAnalyses.h"


#include "LCS.h"
#include "Clone.h"
#include "FunctionDiff.h"
#include "AlignFunctions.h"
#include "BinCallGraph.h"
#include "BinControlFlowAnalysis.h"
#include "BinDataFlowAnalysis.h"
#include "BufferOverflow.h"
#include "InterruptAnalysis.h"
#include "BinDynamicInfo.h"
#include "ForbiddenFunctionCall.h"
#include "NullAfterFree.h"
#include "MallocAndFree.h"
#include "InitPointerToNull.h"
#include "ComplexityMetric.h"
#include "DwarfFileInfo.h"

//class Slide;
class BinQSupport;


class BinQAbstract //: public QWidget
{
  public:

  BinQAbstract() {};
  virtual ~BinQAbstract(){};

    // internal data that stores information of each table entry
    std::vector<Item*> itemsFileA;
    std::vector<Item*> itemsFileB;
    std::map<SgNode*,int> itemsNodeA;
    std::map<int,Item*> byteItemFileA;
    std::map<int,Item*> byteItemFileB;
    std::map<int,Item*> rowItemFileA;
    std::map<int,Item*> rowItemFileB;
    // GUI elements that need to be accesses within this object but also from 
    // analyses implemented for BinQ
    // the two main files
    SgNode* fileA;
    SgNode* fileB;
    SgNode* currentSelectedFile;

    // the dll files
    std::vector<SgNode*> dllFilesA;
    std::vector<SgNode*> dllFilesB;

    int screenWidth;


    std::vector<BinAnalyses*> analyses;
    std::vector<BinAnalyses*> preanalyses;
    BinAnalyses* currentAnalysis;

    // filenames for both files
    std::string fileNameA,fileNameB;
    
  protected:
    // used for testing
    bool test;
    // holds function information that is displayed in function tables
    std::vector<SgNode*> funcsFileA;
    std::vector<SgNode*> funcsFileB;

    // support class
    BinQSupport* binqsupport;

    void init();
    bool sourceFile;
    int screenHeight;
    std::vector<std::string> dllA;
    std::vector<std::string> dllB;
    void createItem(SgNode* file, std::vector<Item*>& itemsFile,std::vector<SgNode*>& funcsFile, bool dll);
    void createFunction(SgNode* file,std::vector<SgNode*>& funcsFile, bool dll);

}; //class BinQGUI

#endif
