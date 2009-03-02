#include "rose.h"
#include "BinQGui.h"
#include "MallocAndFree.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;


std::string MallocAndFree::name() {
  return "Malloc needs Free";
}

std::string MallocAndFree::getDescription() {
  return "When allocating with malloc, free should to be called in the same function";
}


void
MallocAndFree::visit(SgNode* node) {
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
    SgAsmFunctionDeclaration* instFunc = isSgAsmFunctionDeclaration(instBlock->get_parent());
    if (instFunc==NULL)
      return;
    string calleeName = inst->get_comment();

    // do this if we have found a matching free call
    if (calleeName=="malloc" || 
	//	calleeName=="realloc" || 
	calleeName=="calloc" ) {
      // go forward in this function and check for the next mov mem,reg [rax]
      // malloc returns in rax
      string funcName = instFunc->get_name();
      cerr << " Found malloc in function " << funcName << endl;
      rose_addr_t next_addr = inst->get_address() + inst->get_raw_bytes().size();
      std::set<uint64_t> succs;
      std::list<uint64_t> succList;
      succList.push_back(next_addr);
      bool movMemRegFound=false;
      std::set<uint64_t> visited;
      while (!succList.empty()) {
	uint64_t front = succList.front();
	succList.pop_front();
	SgAsmx86Instruction* succInst = isSgAsmx86Instruction(info->getInstructionAtAddress(front));
	SgNode* succBlock = NULL;
	if (project) 
	  succBlock= isSgAsmBlock(succInst->get_parent());
	else //we run IDA, this is different
	  succBlock=succInst;

	if (succBlock==NULL)
	  continue;
	SgAsmFunctionDeclaration* succFunc = isSgAsmFunctionDeclaration(succBlock->get_parent());
	if (succFunc==NULL)
	  continue;
	if (succFunc==instFunc) {
	  // the previous instruction is in the same function
	  // check if it is the instruction we are looking for, e.g. mov mem, reg
	  if (isSgAsmx86Instruction(succInst)->get_kind() == x86_mov) {
	    SgAsmOperandList * ops = succInst->get_operandList();
	    SgAsmExpressionPtrList& opsList = ops->get_operands();
	    SgAsmExpressionPtrList::iterator itOP = opsList.begin();
	    SgAsmMemoryReferenceExpression* mem=NULL;
	    SgAsmx86RegisterReferenceExpression* reg = NULL;
	    int iteration=0;
	    for (;itOP!=opsList.end();++itOP) {
	      SgAsmExpression* exp = *itOP;
	      ROSE_ASSERT(exp);
	      if (iteration==1) {
		// right hand side
		reg = isSgAsmx86RegisterReferenceExpression(exp);		
	      }
	      if (iteration==0) {
		// left hand side
		mem = isSgAsmMemoryReferenceExpression(exp);
		iteration++;
	      }
	    } //for
	    if (mem && reg) {
	      X86RegisterClass cl = reg->get_register_class();
	      int regNr = reg->get_register_number();
	      if (cl == x86_regclass_gpr && regNr==x86_gpr_ax) {
		// this mov matches, now store the address of the mem
		// so we can find out if this address is freed later.
		resolveAddr=BinQSupport::evaluateMemoryExpression(succInst,mem);
		//cerr << "MallocAndFree: Found Malloc - " << RoseBin_support::HexToString(resolveAddr) << endl;
		movMemRegFound=true;
		succList.clear();
	      }
	    }
	  }
	  // else we look further backward
	  if (movMemRegFound==false) {
	    succs = info->getPossibleSuccessors(succInst);
	    // tps : this function above does not seem to take the next instruction into account, 
	    // just jumps, so we add it
	    rose_addr_t next_addr2 = succInst->get_address() + succInst->get_raw_bytes().size();
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
      } //while

      bool foundMatchingFree=false;
      if (resolveAddr!=0) {
	SgAsmFunctionDeclaration* succFunc2=instFunc;
	SgAsmx86Instruction* succInst2=inst;
	std::set<uint64_t> visited;
	while (instFunc==succFunc2) {
	  next_addr = succInst2->get_address() + succInst2->get_raw_bytes().size();
	  succInst2 = isSgAsmx86Instruction(info->getInstructionAtAddress(next_addr));
	  ROSE_ASSERT(succInst2);
	  SgAsmBlock* succBlock2 = isSgAsmBlock(succInst2->get_parent());
	  if (succBlock2==NULL)
	    continue;
	  succFunc2 = isSgAsmFunctionDeclaration(succBlock2->get_parent());
	  if (succFunc2==NULL)
	    continue;
	  if (isSgAsmx86Instruction(succInst2)->get_kind() == x86_call) {
	    string calleeName2 = succInst2->get_comment();
	    // do this if we have found a matching free call
	    if (calleeName2=="free") { 
	      //cerr << "MallocAndFree : found free() " << endl;
	      // we have found a call to free!
	      // look backwards and check for last mov reg,addr . the address contains the pointer that needs to be freed
	      // compare that pointer with the pointer in malloc!
	      std::set<uint64_t> preds = info->getPossiblePredecessors(succInst2);
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
		      rose_addr_t addr=BinQSupport::evaluateMemoryExpression(predInst,mem);
		      if (addr==resolveAddr) {
			//cerr << ">>> MallocAndFree - Matching Free. Found address : " << RoseBin_support::HexToString(resolveAddr) << endl;
			foundMatchingFree=true;
			movRegMemFound=true;
			predList.clear();
		      }
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
	      } // while
	    }
	  } //call free
	} // while
	if (foundMatchingFree==false) {
	  //cerr << " Problem ... malloc without free!"<<endl;
	  string res = "malloc() called but no free() found: ";
	  string funcname="";
	  SgAsmBlock* b = isSgAsmBlock(inst->get_parent());
	  SgAsmFunctionDeclaration* func = NULL;
	  if (b)
	    func=isSgAsmFunctionDeclaration(b->get_parent()); 
	  if (func)
	    funcname = func->get_name();
	  res+=" ("+RoseBin_support::HexToString(inst->get_address())+") : "+unparseInstruction(inst)+
	    " <"+inst->get_comment()+">  in function: "+funcname;
	  result[inst]= res;
	}
	
      } // if resolveadd=0

    }
  }
}

void
MallocAndFree::runTraversal(SgNode* project) {
  this->traverse(project,preorder);
}

void
MallocAndFree::run(SgNode* fileA, SgNode* fileB) {
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
  SgAsmFile* file = NULL;
  if (isSgProject(fileA)) {
    project=true;
    SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
    file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
    ROSE_ASSERT(file);
    info = new VirtualBinCFG::AuxiliaryInformation(file);
  } else
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
    genericF = file->get_genericFile() ;
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
MallocAndFree::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
