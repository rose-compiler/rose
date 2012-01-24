#include "rose.h"
#include "BinQGui.h"
#include "NullAfterFree.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>



using namespace qrs;
using namespace std;
using namespace __gnu_cxx;
using namespace VirtualBinCFG;

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
    string calleeName = inst->get_comment();
    if (calleeName=="free" || calleeName=="_free") {
      //cerr << "  >>>> NullAfterFree : found free() at : " << 
      //      RoseBin_support::HexToString(inst->get_address() )<< endl;
      // we have found a call to free!
      // look backwards and check for last mov reg,addr . the address contains the pointer that needs to be freed
      std::set<uint64_t> preds = info->getPossiblePredecessors(inst);
      std::list<uint64_t> predList;
      std::set<uint64_t>::const_iterator it = preds.begin();
      for (;it!=preds.end();++it) 
	predList.push_back(*it);
      bool movRegMemFound=false;
      std::set<uint64_t> visited;
      while (!predList.empty()) {
	uint64_t front = predList.front();
	predList.pop_front();
	SgAsmx86Instruction* predInst = isSgAsmx86Instruction(info->getInstructionAtAddress(front));

	predInst = BinQSupport::checkIfValidPredecessor(front,predInst);
	if (predInst==NULL)
	  // break;
	  ROSE_ASSERT(predInst);

	//	cerr <<" Possible predecessor : " << unparseInstruction(predInst)<<
	//  RoseBin_support::HexToString(predInst->get_address() ) <<endl;
	SgNode* predBlock = NULL;
	if (project) 
	  predBlock= isSgAsmBlock(predInst->get_parent());
	else //we run IDA, this is different
	  predBlock=predInst;
	//	cerr <<" Possible predecessor : " << unparseInstruction(predInst)<<endl;
#if 0
	SgAsmBlock* predBlock = isSgAsmBlock(predInst->get_parent());
#endif
	if (predBlock==NULL)
	  continue;
	SgAsmFunction* predFunc = isSgAsmFunction(predBlock->get_parent());
	if (predFunc==NULL)
	  continue;
	if (predFunc==instFunc) {
	  // the previous instruction is in the same function
	  // check if it is the instruction we are looking for, e.g. mov reg,addr
	  if (isSgAsmx86Instruction(predInst)->get_kind() == x86_mov) {
	    SgAsmOperandList * ops = predInst->get_operandList();
	    SgAsmExpressionPtrList& opsList = ops->get_operands();
	    //	    rose_addr_t addrDest=0;
	    SgAsmExpressionPtrList::const_iterator itOP = opsList.begin();
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
	      //cerr << ">>> NullAfterFree - predecessor found. Found address : " << RoseBin_support::HexToString(resolveAddr) << endl;
	      movRegMemFound=true;
	      predList.clear();
	    }
	  }
	  // else we look further backward
	  if (movRegMemFound==false) {
	    preds = info->getPossiblePredecessors(predInst);
	    std::set<uint64_t>::const_iterator it = preds.begin();
	    for (;it!=preds.end();++it) {
	      std::set<uint64_t>::const_iterator vis = visited.find(*it);
	      if (vis!=visited.end()) {
		// dont do anything 
	      } else {
		predList.push_back(*it);
		visited.insert(*it);
	      }
	    }
	  }
	}
	//	cerr << "predList.size == " << predList.size() << endl;
      } //while
    } // if

  
      rose_addr_t next_addr = inst->get_address() + inst->get_raw_bytes().size();

      next_addr = BinQSupport::checkIfValidAddress(next_addr,inst);

      //cerr << "     >>>> NullAfterFree : Looking for next call wrapper at : " << 
      //	RoseBin_support::HexToString(inst->get_address() )<< endl;

      std::set<uint64_t> succs;
      std::list<uint64_t> succList;
      succList.push_back(next_addr);
      bool movMemValFound=false;
      std::set<uint64_t> visited;

      // -------------------- WINDOWS SPECIFIC --------------------------
#if 0
      if (genericF==NULL) {
	// IDA-FILE -- assume its windows for now. Is there a way to check this?
	SgAsmx86Instruction* succInst =NULL;
	// find the instruction after the next function call (wrapper)
	while (!succList.empty()) {
	  uint64_t front = succList.front();
	  succList.pop_front();
	  succInst = isSgAsmx86Instruction(info->getInstructionAtAddress(front));
	  SgNode* succBlock = NULL;
	  if (project) 
	    succBlock= isSgAsmBlock(succInst->get_parent());
	  else //we run IDA, this is different
	    succBlock=succInst;

	  if (succBlock==NULL)
	    continue;
	  SgAsmFunction* succFunc = isSgAsmFunction(succBlock->get_parent());
	  if (succFunc==NULL)
	    continue;
	  if (succFunc==instFunc) {
	    succs = info->getPossibleSuccessors(succInst);
	    // tps : this function above does not seem to take the next instruction into account, 
	    // just jumps, so we add it
	    rose_addr_t next_addr2 = succInst->get_address() + succInst->get_raw_bytes().size();

	    next_addr2 = BinQSupport::checkIfValidAddress(next_addr2, succInst);

	    //cerr << " Checking next addr : " << RoseBin_support::HexToString(next_addr2) << endl;
	    if (isSgAsmx86Instruction(succInst) && isSgAsmx86Instruction(succInst)->get_kind() == x86_call) {
	      // if another call found, stop here
	      rose_addr_t next_addr = succInst->get_address() + succInst->get_raw_bytes().size();

	      next_addr = BinQSupport::checkIfValidAddress(next_addr, succInst);

	      //cerr << " Windows workaround starts here: " << RoseBin_support::HexToString(next_addr) << endl;
	      succList.push_back(next_addr);
	      break;
	    }
	    succs.insert(next_addr2);
	    std::set<uint64_t>::const_iterator it = succs.begin();
	    for (;it!=succs.end();++it) {
	      std::set<uint64_t>::const_iterator vis = visited.find(*it);
	      if (vis!=visited.end()) {
		// dont do anything 
	      } else {
		succList.push_back(*it);
		visited.insert(*it);
	      }
	    }
	  }
	}

      }
#endif
      // -----------------------------------------------


    // do this if we have found a matching free call
    if (resolveAddr!=0) {
      // go forward in this function and determine if the address(pointer) is set to NULL
      // we are looking for a mov mem,val
      while (!succList.empty()) {
	uint64_t front = succList.front();
	succList.pop_front();
	SgAsmx86Instruction* succInst = isSgAsmx86Instruction(info->getInstructionAtAddress(front));
	//cerr << "       Checking next after free: " << 
	//  RoseBin_support::HexToString(succInst->get_address()) << endl;
	SgNode* predBlock = NULL;
	if (project) 
	  predBlock= isSgAsmBlock(succInst->get_parent());
	else //we run IDA, this is different
	  predBlock=succInst;
#if 0
	SgAsmBlock* predBlock = isSgAsmBlock(succInst->get_parent());
#endif
	if (predBlock==NULL)
	  continue;
	SgAsmFunction* predFunc = isSgAsmFunction(predBlock->get_parent());
	if (predFunc==NULL)
	  continue;
	if (predFunc==instFunc) {
	  // the previous instruction is in the same function
	  // check if it is the instruction we are looking for, e.g. mov reg,addr
	  if (isSgAsmx86Instruction(succInst)->get_kind() == x86_mov) {
	    SgAsmOperandList * ops = succInst->get_operandList();
	    SgAsmExpressionPtrList& opsList = ops->get_operands();
	    SgAsmExpressionPtrList::const_iterator itOP = opsList.begin();
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
	    //cerr << "   Checking node : " << unparseInstruction(isSgAsmx86Instruction(succInst)) << 
	    //  " val : " << Val << " mem : " << mem<< endl;
	    if (mem && Val) {
	      // this mov matches, now store the address of the mem
	      // so we can find out if this address is freed later.
	      rose_addr_t addr=BinQSupport::evaluateMemoryExpression(succInst,mem);
	      string valStr = BinQSupport::resolveValue(Val);
	      rose_addr_t off = RoseBin_support::HexToDec(valStr);
	      //cerr << ">>> NullAfterFree - mem && val found : " << RoseBin_support::HexToString(addr) << endl;
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
	    // tps : this function above does not seem to take the next instruction into account, 
	    // just jumps, so we add it
	    rose_addr_t next_addr2 = succInst->get_address() + succInst->get_raw_bytes().size();

	    next_addr = BinQSupport::checkIfValidAddress(next_addr2,succInst);

	    succs.insert(next_addr2);
	    //	    cerr <<"  No successor found - next node " << succs.size() << "  " <<  
	    //  unparseInstruction(succInst2) << endl;
	    std::set<uint64_t>::const_iterator it = succs.begin();
	    for (;it!=succs.end();++it) {
	      std::set<uint64_t>::const_iterator vis = visited.find(*it);
	      if (vis!=visited.end()) {
		// dont do anything 
	      } else {
		succList.push_back(*it);
		visited.insert(*it);
	      }
	    }
	  }
	}
	//	cerr << "succList.size == " << succList.size() << endl;
      } //while
      // if we didnt find the free, issue warning
      if (movMemValFound) {
	//cerr << "Found pointer=NULL"<<endl;
      } else {
	//cerr << "Didnt find pointer=NULL"<<endl;
	string res = "free() called && pointer!=NULL: ";
	string funcname="";
	SgAsmBlock* b = isSgAsmBlock(inst->get_parent());
	SgAsmFunction* func = NULL;
	if (b)
	  func=isSgAsmFunction(b->get_parent()); 
	if (func)
	  funcname = func->get_name();
	res+=" ("+RoseBin_support::HexToString(inst->get_address())+") : "+unparseInstruction(inst)+
	  " <"+inst->get_comment()+">  in function: "+funcname;
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

  if (isSgProject(fileA)==NULL && isSgAsmBlock(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
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
NullAfterFree::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
