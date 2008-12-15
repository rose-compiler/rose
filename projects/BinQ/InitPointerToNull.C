#include "BinQGui.h"
#include "InitPointerToNull.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;


std::string InitPointerToNull::name() {
  return "Init Pointer to Null";
}

std::string InitPointerToNull::getDescription() {
  return "When declaring a pointer it should be initialized to NULL";
}


void
InitPointerToNull::visit(SgNode* node) {
  if (isSgAsmFunctionDeclaration(node)) {
    memoryWrites.clear();
    memoryRead.clear();
  } else

  if (isSgAsmx86Instruction(node) && isSgAsmx86Instruction(node)->get_kind() == x86_mov) {
    // this is the address of the mov instruction prior to the call
    //rose_addr_t resolveAddr=0;
    SgAsmx86Instruction* inst = isSgAsmx86Instruction(node);
    SgAsmBlock* instBlock = isSgAsmBlock(inst->get_parent());
    if (instBlock==NULL)
      return;
    SgAsmFunctionDeclaration* instFunc = isSgAsmFunctionDeclaration(instBlock->get_parent());
    if (instFunc==NULL)
      return;

    // we have found a mov instruction
    // we need to check if it is a   mov mem, (value or reg) // assignment of variable
    // or we find a                  mov reg, mem // usage of variable
    // make sure a variable is assigned before used
    SgAsmOperandList * ops = inst->get_operandList();
    SgAsmExpressionPtrList& opsList = ops->get_operands();
    SgAsmExpressionPtrList::iterator itOP = opsList.begin();
    SgAsmMemoryReferenceExpression* memL=NULL;
    SgAsmMemoryReferenceExpression* memR=NULL;
    SgAsmx86RegisterReferenceExpression* regL=NULL;
    SgAsmx86RegisterReferenceExpression* regR=NULL;
    SgAsmValueExpression* Val = NULL;
    int iteration=0;
    for (;itOP!=opsList.end();++itOP) {
      SgAsmExpression* exp = *itOP;
      ROSE_ASSERT(exp);
      if (iteration==1) {
	// right hand side
	memR = isSgAsmMemoryReferenceExpression(exp);
	regR = isSgAsmx86RegisterReferenceExpression(exp);
	Val = isSgAsmValueExpression(exp);
      }
      if (iteration==0) {
	// left hand side
	memL = isSgAsmMemoryReferenceExpression(exp);
	regL = isSgAsmx86RegisterReferenceExpression(exp);
	iteration++;
      }
    } //for
    if ((memL && regR) || (memL && Val)) {
      // could be assignment to address
      rose_addr_t addr=BinQSupport::evaluateMemoryExpression(inst,memL);      
      bool containsBP = BinQSupport::memoryExpressionContainsRegister(x86_regclass_gpr,x86_gpr_bp, memL);
      if (containsBP) {
	// this is memory write with offset to BP
	// remember this memory location as a write
	if (debug)
	cerr << "found a memory write (REG) : " << RoseBin_support::HexToString(inst->get_address())<<" "<<unparseInstruction(inst)<<endl;
	memoryWrites.insert(addr);
      }
    } else if (regL && memR) {
      // could be usage of address
      rose_addr_t addr=BinQSupport::evaluateMemoryExpression(inst,memR);      
      bool containsBP = BinQSupport::memoryExpressionContainsRegister(x86_regclass_gpr,x86_gpr_bp, memR);
      if (containsBP) {
	// this is memory read with offset to BP
	// did we see a write for this? If not, it is not initialized!
	std::set<rose_addr_t>::const_iterator it = memoryWrites.find(addr);
	if (it!=memoryWrites.end()) {
	  // found write, everything is good
	if (debug)
	  cerr << "found a read with matching write : " << RoseBin_support::HexToString(inst->get_address())<<" "<<unparseInstruction(inst)<<endl;
	} else {
	  std::set<rose_addr_t>::const_iterator it2 = memoryRead.find(addr);
	  if (it2!=memoryRead.end()) {
	    // found this case before
	  } else {
	if (debug)
	    cerr << " This variable might not be initialized : " << RoseBin_support::HexToString(inst->get_address())<<" "<< unparseInstruction(inst) << endl;
	    string res = "This variable might not be initialized : ";
	    res+="  addr:"+RoseBin_support::HexToString(inst->get_address())+" : "+unparseInstruction(inst)+" <"+inst->get_comment()+">";
	    result[inst]= res;
	    memoryRead.insert(addr);
	  }
	}
      }
    }
  }
}


void
InitPointerToNull::runTraversal(SgNode* project) {
  this->traverse(project,preorder);
}

void
InitPointerToNull::run(SgNode* fileA, SgNode* fileB) {
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
InitPointerToNull::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
