#include "BinQGui.h"
#include "BinDynamicInfo.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;

std::string ForbiddenFunctionCall::name() {
  return "Forbidden Function Call";
}

std::string ForbiddenFunctionCall::getDescription() {
  return "Contains a black list of functions that should not be called. Depends on DynamicInfo analysis.";
}


void
ForbiddenFunctionCall::visit(SgNode* node) {
  if (isSgAsmx86Instruction(node) && isSgAsmx86Instruction(node)->get_kind() == x86_call) {
    SgAsmx86Instruction* inst = isSgAsmx86Instruction(node);
    string calleeName = inst->get_comment();
    std::vector<std::string>::const_iterator it = blackList.begin();
    for (;it!=blackList.end();++it) {
      string name = *it;
      if (name==calleeName) {
	//cerr << " match : " << name << endl;
	string res = "This is a function that should not be called : ";
	 res+=name+"  "+inst->unparseToString();
	result[inst]= res;
      }
    }
  }
}

void
ForbiddenFunctionCall::runTraversal(SgNode* project) {
  blackList.clear();
  blackList.push_back("malloc");
  blackList.push_back("_malloc");
  blackList.push_back("free");

  this->traverse(project,preorder);
}

void
ForbiddenFunctionCall::run(SgNode* fileA, SgNode* fileB) {
  instance=NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();

  if (isSgProject(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
    QString res = QString("This is not a valid file for this analysis");
    instance->analysisResult->append(res);  
    return;
  }

  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);
  info = new VirtualBinCFG::AuxiliaryInformation(file);

  if (debug)
    cerr << " Running forbidden function call ... " << endl;
  if (!testFlag) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    QString res = QString("Looking at dynamic information : %1").arg(file->get_name().c_str());
    instance->analysisResult->append(res);  
  }


  genericF = file->get_genericFile() ;
  runTraversal(isSgProject(fileA));


  if (instance) {
    QString res = QString("\n>>>>>>>>>>>>>>>> Resolving call addresses to names ...");
    instance->analysisResult->append(res);  
  }
}



void
ForbiddenFunctionCall::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
