#include "BinQGui.h"
#include "BinDynamicInfo.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;

std::string DynamicInfo::name() {
  return "Binary Dynamic Info";
}

std::string DynamicInfo::getDescription() {
  return "Test to determine more information about dynamic libraries.";
}


void
DynamicInfo::run() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();

  ROSE_ASSERT(isSgProject(instance->fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(instance->fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  cerr << " Running dynamic info ... " << endl;
  ROSE_ASSERT(instance);
  ROSE_ASSERT(instance->analysisTab);
  instance->analysisTab->setCurrentIndex(1);
  QString res = QString("Looking at dynamic information ");
  instance->analysisResult->append(res);  
  
  
}
