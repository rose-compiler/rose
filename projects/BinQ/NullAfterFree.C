#include "BinQGui.h"
#include "NullAfterFree.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;


std::string NullAfterFree::name() {
  return "Null After Free";
}

std::string NullAfterFree::getDescription() {
  return "When memory is freed the pointer should be set to NULL";
}


void
NullAfterFree::visit(SgNode* node) {
  if (isSgAsmx86Instruction(node) && isSgAsmx86Instruction(node)->get_kind() == x86_call) {
    // this is the address of the mov instruction prior to the call
    rose_addr_t resolveAddr=0;
    SgAsmx86Instruction* inst = isSgAsmx86Instruction(node);
    SgAsmBlock* instBlock = isSgAsmBlock(inst->get_parent());
    if (instBlock==NULL)
      return;
    SgAsmFunctionDeclaration* instFunc = isSgAsmFunctionDeclaration(instBlock->get_parent());
    if (instFunc==NULL)
      return;
    string calleeName = inst->get_comment();
    if (calleeName=="free") {
      // we have found a call to free!
      // look backwards and check for last mov reg,addr . the address contains the pointer that needs to be freed
      std::set<uint64_t> preds = info->getPossiblePredecessors(inst);
      std::list<uint64_t> predList;
      std::set<uint64_t>::const_iterator it = preds.begin();
      for (;it!=preds.end();++it) 
	predList.push_back(*it);
      bool movRegMemFound=false;
      while (!predList.empty()) {
	uint64_t front = predList.front();
	predList.pop_front();
	SgAsmx86Instruction* predInst = isSgAsmx86Instruction(info->getInstructionAtAddress(front));
	//	cerr <<" Possible predecessor : " << unparseInstruction(predInst)<<endl;
	SgAsmBlock* predBlock = isSgAsmBlock(predInst->get_parent());
	if (predBlock==NULL)
	  continue;
	SgAsmFunctionDeclaration* predFunc = isSgAsmFunctionDeclaration(predBlock->get_parent());
	if (predFunc==NULL)
	  continue;
	if (predFunc==instFunc) {
	  // the previous instruction is in the same function
	  // check if it is the instruction we are looking for, e.g. mov reg,addr
	  if (isSgAsmx86Instruction(predInst)->get_kind() == x86_mov) {
	    SgAsmOperandList * ops = predInst->get_operandList();
	    SgAsmExpressionPtrList& opsList = ops->get_operands();
	    //	    rose_addr_t addrDest=0;
	    SgAsmExpressionPtrList::iterator itOP = opsList.begin();
	    SgAsmMemoryReferenceExpression* mem=NULL;
	    SgAsmx86RegisterReferenceExpression* reg=NULL;
	    int iteration=0;
	    for (;itOP!=opsList.end();++itOP) {
	      SgAsmExpression* exp = *itOP;
	      ROSE_ASSERT(exp);
	      if (iteration==1) {
		// right hand side
		mem = isSgAsmMemoryReferenceExpression(exp);
	      }
	      if (iteration==0) {
		// left hand side
		reg = isSgAsmx86RegisterReferenceExpression(exp);		
		iteration++;
	      }
	    } //for
	    if (mem && reg) {
	      // this mov matches, now store the address of the mem
	      // so we can find out if this address is freed later.
	      resolveAddr=BinQSupport::evaluateMemoryExpression(predInst,mem);
	      //cerr << ">>> NullAfterFree - Match. Found address : " << RoseBin_support::HexToString(resolveAddr) << endl;
	      movRegMemFound=true;
	      predList.clear();
	    }
	  }
	  // else we look further backward
	  if (movRegMemFound==false) {
	    preds = info->getPossiblePredecessors(predInst);
	    std::set<uint64_t>::const_iterator it = preds.begin();
	    for (;it!=preds.end();++it) 
	      predList.push_back(*it);
	  }
	}
      } //while
    } // if

    // do this if we have found a matching free call
    if (resolveAddr!=0) {
      // go forward in this function and determine if the address(pointer) is set to NULL
      // we are looking for a mov mem,val
      rose_addr_t next_addr = inst->get_address() + inst->get_raw_bytes().size();
      std::set<uint64_t> succs;
      std::list<uint64_t> succList;
      succList.push_back(next_addr);
      bool movMemValFound=false;
      while (!succList.empty()) {
	uint64_t front = succList.front();
	succList.pop_front();
	SgAsmx86Instruction* succInst = isSgAsmx86Instruction(info->getInstructionAtAddress(front));
	SgAsmBlock* predBlock = isSgAsmBlock(succInst->get_parent());
	if (predBlock==NULL)
	  continue;
	SgAsmFunctionDeclaration* predFunc = isSgAsmFunctionDeclaration(predBlock->get_parent());
	if (predFunc==NULL)
	  continue;
	if (predFunc==instFunc) {
	  // the previous instruction is in the same function
	  // check if it is the instruction we are looking for, e.g. mov reg,addr
	  if (isSgAsmx86Instruction(succInst)->get_kind() == x86_mov) {
	    SgAsmOperandList * ops = succInst->get_operandList();
	    SgAsmExpressionPtrList& opsList = ops->get_operands();
	    SgAsmExpressionPtrList::iterator itOP = opsList.begin();
	    SgAsmMemoryReferenceExpression* mem=NULL;
	    SgAsmValueExpression* Val = NULL;
	    int iteration=0;
	    for (;itOP!=opsList.end();++itOP) {
	      SgAsmExpression* exp = *itOP;
	      ROSE_ASSERT(exp);
	      if (iteration==1) {
		// right hand side
		Val = isSgAsmValueExpression(exp);
	      }
	      if (iteration==0) {
		// left hand side
		mem = isSgAsmMemoryReferenceExpression(exp);
		iteration++;
	      }
	    } //for
	    if (mem && Val) {
	      // this mov matches, now store the address of the mem
	      // so we can find out if this address is freed later.
	      rose_addr_t addr=BinQSupport::evaluateMemoryExpression(succInst,mem);
	      string valStr = BinQSupport::resolveValue(Val);
	      rose_addr_t off = RoseBin_support::HexToDec(valStr);
	      if ((addr==resolveAddr) && off==0) {
		// found the pointer=NULL inst!
		//cerr << ">>> NullAfterFree - Pointer cleared for address : " << RoseBin_support::HexToString(resolveAddr) << endl;
		movMemValFound=true;
		succList.clear();
	      }
	    }
	  }
	  // else we look further backward
	  if (movMemValFound==false) {
	    succs = info->getPossibleSuccessors(succInst);
	    std::set<uint64_t>::const_iterator it = succs.begin();
	    for (;it!=succs.end();++it) 
	      succList.push_back(*it);
	  }
	}
      } //while
      // if we didnt find the free, issue warning
      if (movMemValFound) {
	//cerr << "Found pointer=NULL"<<endl;
      } else {
	//cerr << "Didnt find pointer=NULL"<<endl;
	string res = "free() called but pointer not set to NULL : ";
	res+="  addr:"+RoseBin_support::HexToString(inst->get_address())+" : "+unparseInstruction(inst)+" <"+inst->get_comment()+">";
	result[inst]= res;
      }
    }
  }
}

void
NullAfterFree::runTraversal(SgNode* project) {
  this->traverse(project,preorder);
}

void
NullAfterFree::run(SgNode* fileA, SgNode* fileB) {
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
NullAfterFree::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
