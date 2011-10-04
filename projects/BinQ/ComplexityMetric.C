#include "rose.h"
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
    SgNode* instBlock = NULL;
    if (project) 
      instBlock= isSgAsmBlock(inst->get_parent());
    else //we run IDA, this is different
      instBlock=inst;

    if (instBlock==NULL)
      return;
    SgAsmFunction* instFunc = isSgAsmFunction(instBlock->get_parent());
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
  max=45;
  lastFunction=NULL;
  complexity=0;
  this->traverse(project,preorder);
}

void
ComplexityMetric::run(SgNode* fileA, SgNode* fileB) {
  instance=NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();

  if (isSgProject(fileA)==NULL && isSgAsmBlock(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!  fileA = " << fileA->class_name() << endl;
    if (!testFlag) {
      QString res = QString("This is not a valid file for this analysis");
      instance->analysisResult->append(res);  
    }
    return;
  }

  project=false;
  SgAsmGenericFile* file = NULL;
  if (isSgProject(fileA)) {
    project=true;
    SgBinaryComposite* binary = isSgBinaryComposite(isSgProject(fileA)->get_fileList()[0]);
    file = binary != NULL ? binary->get_binaryFile() : NULL;
    ROSE_ASSERT(file);
    info = new VirtualBinCFG::AuxiliaryInformation(file);
  }
  else
    info = new VirtualBinCFG::AuxiliaryInformation(fileA);
    
  if (!testFlag) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    if (isSgProject(fileA)) {
      QString res = QString("Looking at dynamic information : %1").arg(file->get_name().c_str());
      instance->analysisResult->append(res);  
    }
  }

  if (isSgProject(fileA)) {
    genericF = file;
    runTraversal(isSgProject(fileA));
  }
  else
    runTraversal(isSgAsmBlock(fileA));

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
