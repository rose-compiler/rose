#include "BinQGui.h"
#include "ComplexityMetric.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;


std::string ComplexityMetric::name() {
  return "Complexity Metric";
}

std::string ComplexityMetric::getDescription() {
  return "Determines the Complexity of each function";
}


void
ComplexityMetric::visit(SgNode* node) {
  if (!isSgAsmx86Instruction(node))
    return;
  SgAsmx86Instruction* inst = isSgAsmx86Instruction(node);
  if (inst->get_kind() == x86_jmp ||
      inst->get_kind() == x86_farjmp ||
      x86InstructionIsConditionalFlagControlTransfer(inst) ||
      x86InstructionIsConditionalControlTransfer(inst)) {
    SgAsmBlock* instBlock = isSgAsmBlock(inst->get_parent());
    if (instBlock==NULL)
      return;
    SgAsmFunctionDeclaration* instFunc = isSgAsmFunctionDeclaration(instBlock->get_parent());
    if (instFunc==NULL)
      return;
    if (lastFunction!=instFunc && lastFunction!=NULL ) {
      string res = "Complexity : "+RoseBin_support::ToString(complexity);
      res+="  "+lastFunction->get_name();
      if (complexity>max)
	result[inst]= res;
      if (debug)
	cerr <<"Function : " << lastFunction->get_name() << "  Complexity : " << RoseBin_support::ToString(complexity) << endl;
      lastFunction=instFunc;
      complexity=0;
    } else {
      lastFunction=instFunc;
      complexity++;
      if (debug && complexity>max)
	cerr <<"  >>Function : " << lastFunction->get_name() << "  Complexity : " << RoseBin_support::ToString(complexity) << 
	  "   inst: " <<RoseBin_support::HexToString(inst->get_address())<<" : "<<unparseInstruction(inst)<<endl;
    }
  }
}

void
ComplexityMetric::runTraversal(SgNode* project) {
  max=50;
  lastFunction=NULL;
  complexity=0;
  this->traverse(project,preorder);
}

void
ComplexityMetric::run(SgNode* fileA, SgNode* fileB) {
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
ComplexityMetric::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
