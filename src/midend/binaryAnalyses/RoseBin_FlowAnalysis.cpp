/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/

#include "rose.h"
#include "RoseBin_FlowAnalysis.h"

using namespace std;

bool RoseBin_FlowAnalysis::initialized = false;

//#include "AST_BIN_Traversal.h"

void
RoseBin_FlowAnalysis::getRootNodes(vector <SgDirectedGraphNode*>& rootNodes) {
  nrOfFunctions=0;
  ROSE_ASSERT(vizzGraph);
  rose_hash::hash_map <std::string, SgDirectedGraphNode*>::const_iterator itn = vizzGraph->nodes.begin();
  for (; itn!=vizzGraph->nodes.end();++itn) {
    string hex_address = itn->first;
    SgDirectedGraphNode* node = isSgDirectedGraphNode(itn->second);
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    if (func) {
      rootNodes.push_back(node);
      nrOfFunctions++;
    }
  }
}


bool
RoseBin_FlowAnalysis::sameParents(SgDirectedGraphNode* node, SgDirectedGraphNode* next) {
  bool same=false;
  if (isSgAsmFunctionDeclaration(node->get_SgNode())) {
    return true;
  }

  SgAsmx86Instruction* thisNode = isSgAsmx86Instruction(node->get_SgNode());
  SgAsmx86Instruction* nextNode = isSgAsmx86Instruction(next->get_SgNode());
  if (thisNode && nextNode) {
    if (!db) {
    SgAsmFunctionDeclaration* func1 = isSgAsmFunctionDeclaration(thisNode->get_parent()->get_parent());
    SgAsmFunctionDeclaration* func2 = isSgAsmFunctionDeclaration(nextNode->get_parent()->get_parent());
    if (func1==func2)
      same=true;
    } else {
    SgAsmFunctionDeclaration* func1 = isSgAsmFunctionDeclaration(thisNode->get_parent());
    SgAsmFunctionDeclaration* func2 = isSgAsmFunctionDeclaration(nextNode->get_parent());
    if (func1==func2)
      same=true;
    }
  }
  return same;
}

/*
 * This function removes blocks, so functions contain only instructions
 * deprecated!
 */
void
RoseBin_FlowAnalysis::flattenBlocks(SgAsmNode* globalNode) {
  vector<SgNode*> tree =NodeQuery::querySubTree(globalNode, V_SgAsmBlock);
  vector<SgNode*>::iterator itV = tree.begin();
  cerr << " ObjDump-BinRose:: Removing Blocks " << endl;
  for (;itV!=tree.end();itV++) {
    SgAsmBlock* block = isSgAsmBlock(*itV);
    if (block && block!=globalNode) {
      SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(block->get_parent());
      if (func) {
	info->returnTargets[func].insert(info->returnTargets[block].begin(), info->returnTargets[block].end());
	vector <SgNode*> vec =block->get_traversalSuccessorContainer();
	for (unsigned int itf = 0; itf < vec.size() ; itf++) {
	  SgAsmInstruction* finst = isSgAsmInstruction(vec[itf]);
	  finst->set_parent(func);
	  func->append_statement(finst);
	}
	func->remove_statement(block);
      }

    }
  }


}

/*
 * Converts blocks to functions (is not part of Jeremiahs disassembler - and also IDA)
 * deprecated!
 */
void
RoseBin_FlowAnalysis::convertBlocksToFunctions(SgAsmNode* globalNode) {
  vector<SgNode*> tree =NodeQuery::querySubTree(globalNode, V_SgAsmBlock);
  vector<SgNode*>::iterator itV = tree.begin();
  cerr << " ObjDump-BinRose:: Converting Blocks To Functions" << endl;
  for (;itV!=tree.end();itV++) {
    SgAsmBlock* block = isSgAsmBlock(*itV);
    if (block && block!=globalNode) {
      uint64_t addr = block->get_address();
      isSgAsmBlock(globalNode)->remove_statement(block);
      block->set_parent(NULL);
      SgAsmFunctionDeclaration* func = new SgAsmFunctionDeclaration(addr, RoseBin_support::HexToString(addr));
      info->returnTargets[func].insert(info->returnTargets[block].begin(), info->returnTargets[block].end());
      isSgAsmBlock(globalNode)->append_statement(func);
      func->set_parent(globalNode);
      vector <SgNode*> vec =block->get_traversalSuccessorContainer();
      for (unsigned int itf = 0; itf < vec.size() ; itf++) {
	SgAsmInstruction* finst = isSgAsmInstruction(vec[itf]);
	finst->set_parent(func);
	func->append_statement(finst);
      }
      block->remove_children();
    }
  }

  //  string filename="_binary_tree_func.dot";
  //AST_BIN_Traversal* trav = new AST_BIN_Traversal();
  //trav->run(globalNode, filename);

}

/*
 * Detect functions (blocks) that can be merged together.
 */
void
RoseBin_FlowAnalysis::resolveFunctions(SgAsmNode* globalNode) {
  cerr << " ObjDump-BinRose:: Detecting and merging Functions" << endl;
  vector<SgAsmFunctionDeclaration*> visitedFunctions;
  vector<SgNode*> tree =NodeQuery::querySubTree(globalNode, V_SgAsmFunctionDeclaration);
  //  vector<SgNode*>::iterator itV = tree.begin();
  int nr=0;
  while (!tree.empty()) {
    //  for (;itV!=tree.end();itV++) {
    SgAsmFunctionDeclaration* funcD = isSgAsmFunctionDeclaration(tree.back());
    tree.pop_back();
    nr++;
    if ((nr % 100)==0)
      if (RoseBin_support::DEBUG_MODE())
	cerr << " funcListSize : " << tree.size() << "  -- iteration : " << nr << "   func " << funcD->get_name() << endl;

    //SgAsmFunctionDeclaration* funcD = isSgAsmFunctionDeclaration(*itV);
    //itV++;
    ROSE_ASSERT(funcD);
    // make sure we dont visit a function twice


    vector <SgNode*> funcVec =funcD->get_traversalSuccessorContainer();
    int last = funcVec.size()-1;
    if (last<0)
      continue;
    bool hasStopCondition=false;
    for (unsigned int itf = 0; itf < funcVec.size() ; itf++) {
      SgAsmx86Instruction* finst = isSgAsmx86Instruction(funcVec[itf]);
      ROSE_ASSERT(finst);
      if (finst->get_kind() == x86_ret || finst->get_kind() == x86_hlt) {
	hasStopCondition=true;
      }
    }
    //cerr << " last : " << last << endl;
    SgAsmx86Instruction* lastInst = isSgAsmx86Instruction(funcVec[last]);
    ROSE_ASSERT(lastInst);
    SgAsmx86Instruction* nextInst = isSgAsmx86Instruction(resolveFunction(lastInst, hasStopCondition));
    if (nextInst) {
      SgAsmFunctionDeclaration* nextFunc = isSgAsmFunctionDeclaration(nextInst->get_parent());
      if (nextFunc) {
        info->returnTargets[funcD].insert(info->returnTargets[nextFunc].begin(), info->returnTargets[nextFunc].end());
	// make sure that this function is being changed and should not be covered again
	//visitedFunctions.push_back(nextFunc);
	// visit current function after alternation again
	//tree.push_back(funcD);
	// now we remove this next function and iterate thrgouh all instructions and
	// attach them to the old function
	vector <SgNode*> funcNextVec =nextFunc->get_traversalSuccessorContainer();
	for (unsigned int i=0; i < funcNextVec.size(); ++i) {
	  SgAsmInstruction* inst = isSgAsmInstruction(funcNextVec[i]);
	  ROSE_ASSERT(inst);
	  inst->set_parent(funcD);
	  funcD->append_statement(inst);
	  //nextFunc->remove_statement(inst);
	  // delete nextFunc; // should delete this later when iterator is done
	}
	nextFunc->remove_children();
	nextFunc->set_parent(NULL);
	isSgAsmBlock(globalNode)->remove_statement(nextFunc);
      }
    }
  } // for

}


SgAsmInstruction*
RoseBin_FlowAnalysis::resolveFunction(SgAsmInstruction* instx, bool hasStopCondition) {
  SgAsmx86Instruction* inst = isSgAsmx86Instruction(instx);
  if (inst==NULL) return NULL;
  SgAsmInstruction* nextFlow = inst->cfgBinFlowOutEdge(info);
  // if current node is not a controltransfer node (e.g. jmp, ret, ...),
  // then there should be a flow to a next node
  //  SgAsmx86ControlTransferInstruction* contrlInst = isSgAsmx86ControlTransferInstruction(inst);
  if (nextFlow==NULL &&
      hasStopCondition==false) { // && !isSgAsmx86Jmp(inst)) {
    // in this case, we have a ordinary node that should be connected to the next block
    // now lets find the next block and create a function for these two blocks
    uint64_t addrInst = inst->get_address();
    uint64_t size = (inst->get_raw_bytes()).size();
    uint64_t nextAddr = addrInst+size;
    rose_hash::hash_map <uint64_t, SgAsmInstruction* >::const_iterator it2 =
      rememberInstructions.find(nextAddr);
    if (it2!=rememberInstructions.end()) {
      // found the next instruction
      nextFlow = isSgAsmInstruction(it2->second);
      //if (RoseBin_support::DEBUG_MODE())
      //	cout << " function resolution: resolving next : " << nextFlow->class_name() << "    this : "
      //	     << unparser->unparseInstruction(inst) << endl;
    }
  }

  else if (nextFlow==NULL &&
	   hasStopCondition==false && inst->get_kind() == x86_jmp) {
    // in this case we want to connect to the destination
    nextFlow = inst->cfgBinFlowOutEdge(info);
    //if (RoseBin_support::DEBUG_MODE())
    // cerr << " function resolution: resolving jump " << nextFlow << "  this : " << inst->class_name() << endl;
  } else {
    if (RoseBin_support::DEBUG_MODE())
      if (!(inst->get_kind() == x86_nop || inst->get_kind() == x86_ret))
	cerr << " WARNING: function resolution::  cant resolve :  " << inst->class_name() << "(" << unparseX86Instruction(inst) << ")" << endl;
  }

  return nextFlow;
}

SgAsmInstruction*
RoseBin_FlowAnalysis::process_jumps_get_target(SgAsmx86Instruction* inst) {
  if (inst && x86InstructionIsControlTransfer(inst)) {
    //cerr << " ..................... processing jmp " << endl;
    ostringstream addrhex3;
    int addrsource = inst->get_address();
    addrhex3 << hex << setw(8) << addrsource ;
    string funcName ="";

    // get the operand and the destination address
    SgAsmOperandList* opList = inst->get_operandList();
    ROSE_ASSERT(opList);
    SgAsmExpressionPtrList ptrList = opList->get_operands();

    std::vector<SgAsmExpression*>::iterator itList= ptrList.begin();
    for (;itList!=ptrList.end();++itList) {
      SgAsmExpression* exp = *itList;
      ROSE_ASSERT(exp);
      SgAsmRegisterReferenceExpression* regRef = isSgAsmRegisterReferenceExpression(exp);

      //if (RoseBin_support::DEBUG_MODE())
      //	cout << " inst (jmp):: " << inst->get_mnemonic() << "  addr : " << addrhex3.str() << endl;
      SgAsmValueExpression* valExpr = isSgAsmValueExpression(exp);
      SgAsmMemoryReferenceExpression* memExpr = isSgAsmMemoryReferenceExpression(exp);
      string valStr = "";
      if (valExpr) {
	uint8_t byte_val=0xF;
	uint16_t word_val=0xFF;
	uint32_t double_word_val=0xFFFF;
	uint64_t quad_word_val=0xFFFFFFFFU;

	valStr =
	  RoseBin_support::resolveValue(valExpr, true,
					byte_val,
					word_val,
					double_word_val,
					quad_word_val);

	//if (RoseBin_support::DEBUG_MODE())
	//cout << "   found value ....... :: " << valStr << endl;
	funcName = valExpr->get_replacement();
	//if (funcName=="")
	//  funcName="noName";
      }
      if (memExpr) {
	continue;
	// this is a jump to data ... do not handle right now!!
      }

      // convert val string to long
      uint64_t val=0;
      if(from_string<uint64_t>(val, valStr, std::hex)) {
	ostringstream addrhex2;
	addrhex2 << hex << setw(8) << val ;
	//if (RoseBin_support::DEBUG_MODE())
	//cerr << "    looking for value ("<<valStr << " ) in InstrSet: "
	//     << val << "  " << addrhex2.str() << endl;
        rose_hash::hash_map <uint64_t, SgAsmInstruction* >::const_iterator itc =
	  rememberInstructions.find(val);
	if (itc!=rememberInstructions.end()) {
	  SgAsmInstruction* target = itc->second;

	  // we set the target (jump to for each control instruction)
	  ROSE_ASSERT(target);


	  //if (RoseBin_support::DEBUG_MODE())
	  //cout << "    >>> target found! " << target << "     funcName " << funcName << endl;
	  if (funcName!="") {
	    SgAsmNode* block = target;
	    if (!db)
	      block = isSgAsmNode(target->get_parent());
	    ROSE_ASSERT(block);
	    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(block->get_parent());

	    if (func) {
	      string fname = func->get_name();
	      uint64_t val_f=0;
	      if(from_string<uint64_t>(val_f, fname, std::hex)) {
		// func name is a hex number
		func->set_name(funcName);
		//		inst->set_comment(funcName);
	      } else {
		// its a name
	      }
	    }
	  }
	  return target;
	} else {
	  //if (RoseBin_support::DEBUG_MODE())
	  //  cerr << "    >>>>>>>>>>>>>>> !!! OPS :: Target not found ...  \n" << endl;
	}
      }
      else{
	//	std::cerr << "FlowAnalysis ::  from_string failed .. " << std::endl;
	if (valStr!="")
	cerr << " WARNING: Cant convert string to long - in process_jump  :: " << regRef->class_name() <<
	  " inst :: " << inst->get_mnemonic() << "  addr : " << addrhex3.str() << " target : " << valStr << endl;
      }
    }

  }
  return NULL;
}

void
RoseBin_FlowAnalysis::initFunctionList(SgAsmNode* globalNode) {
  vector<SgNode*> tree =NodeQuery::querySubTree(globalNode, V_SgAsmInstruction);
  vector<SgNode*>::iterator itV = tree.begin();
  for (;itV!=tree.end();itV++) {
    SgAsmInstruction* inst = isSgAsmInstruction(*itV);
    if (inst) {
      uint64_t addr = inst->get_address();
      rememberInstructions[addr] = inst;
    }
  }
}

void printAST(SgAsmNode* globalNode) {
  std::cerr << " ++++++++++++++++++++++++++++++= printing AST +++++++++++++++++++++++\n\n " << endl;
  std::string filename="_binary_tree_func.dot";
  AST_BIN_Traversal* trav = new AST_BIN_Traversal();
  trav->run(globalNode, filename);
}

void
RoseBin_FlowAnalysis::process_jumps() {

  cerr << "\n >>>>>>>>> processing jumps ... " << endl;
  rose_hash::hash_map <uint64_t, SgAsmInstruction* >::iterator it;
  for (it=rememberInstructions.begin();it!=rememberInstructions.end();++it) {
    SgAsmx86Instruction* inst = isSgAsmx86Instruction(it->second);
    if (inst->get_kind() == x86_call) {
      //cerr << "Found call at " << std::hex << inst->get_address() << endl;
      SgAsmx86Instruction* target = isSgAsmx86Instruction(process_jumps_get_target(inst));
      if (target) {
        //cerr << "Target is " << std::hex << target->get_address() << endl;
	// inst->get_targets().push_back(target);
	// we set the sources (for each node)
        info->incomingEdges[target].insert(inst->get_address());
	// tps: changed this algorithm so that it runs in
	// linear time!
        ROSE_ASSERT (target->get_parent());
	if (target->get_parent()) {
	  // ROSE_ASSERT(target->get_parent());
	  SgAsmNode* b_b = target;
	  if (!db)
	    b_b = isSgAsmNode(target->get_parent());
	  ROSE_ASSERT(b_b);
	  SgAsmFunctionDeclaration* b_func = isSgAsmFunctionDeclaration(b_b->get_parent());

	  if (b_func) {
	    // (16/Oct/07) tps: this is tricky, it appears that sometimes the target can
	    // be just a jmp to a new location, so we should forward this information to the correct
	    // function.
	    // Therefore we need to check if the current function has a return statement.
	    // If not, we want to forward this information.
	    if (target->get_kind() == x86_jmp) {
	      //cerr << " >>>>>>>> found a jmp target - number of children: " << b_func->get_traversalSuccessorContainer().size() << endl;
	      if (b_func->get_numberOfTraversalSuccessors()==1) {
		SgAsmx86Instruction* target2 = isSgAsmx86Instruction(process_jumps_get_target(inst));
		if (target2) {
		  b_b = target2;
		  if (!db)
		    b_b = isSgAsmNode(target2->get_parent());
		  b_func = isSgAsmFunctionDeclaration(b_b->get_parent());
		}
	      }
	    }


	    if (inst->get_parent()) {
              //cerr << "Inst has a parent" << endl;
	      if (inst->get_comment()=="")
	      	inst->set_comment(""+b_func->get_name());
              SgAsmInstruction* inst_after = info->getInstructionAtAddress(inst->get_address() + inst->get_raw_bytes().size()); // inst->cfgBinFlowOutEdge(info);
              if (inst_after) {
                //cerr << "Added dest " << std::hex << isSgAsmStatement(inst_after)->get_address() << " for function" << endl;
                b_func->append_dest(isSgAsmStatement(inst_after));
              }
	    }
	  } else {
	    cerr << " NO FUNCTION DETECTED ABOVE BLOCK . " << endl;
	  }

	} else {
	  cerr << "   WARNING :: process_jumps: target has no parent ... i.e. no FunctionDeclaration to it " <<
	    target->class_name() << endl;
	}
      } else {
	if (inst)
	  cerr << "    WARNING :: process_jumps: No target found for node " << RoseBin_support::HexToString(inst->get_address())
	       << "   " << inst->get_mnemonic() << endl;
      }
    } else {

      // might be a jmp
      SgAsmx86Instruction* target = isSgAsmx86Instruction(process_jumps_get_target(inst));
      if (target) {
	// inst->get_targets().push_back(target);
	// we set the sources (for each node)
        info->incomingEdges[target].insert(inst->get_address());
      }
    }
  }
  //cerr << "\n >>>>>>>>> processing jumps ... done. " << endl;

  //  cerr << "\n >>>>>>>>> resolving RET jumps ... " << endl;
  rose_hash::hash_map <uint64_t, SgAsmInstruction* >::iterator it2;
  for (it2=rememberInstructions.begin();it2!=rememberInstructions.end();++it2) {
    //int id = it2->first;
    SgAsmx86Instruction* target = isSgAsmx86Instruction(it2->second);
    ROSE_ASSERT (target);
#if 1
    if (target->get_kind() == x86_ret) {
      SgAsmNode* b_b = target;
      if (!db)
	b_b = isSgAsmNode(target->get_parent());
      SgAsmFunctionDeclaration* parent = isSgAsmFunctionDeclaration(b_b->get_parent());
      if (parent) {
	//ROSE_ASSERT(parent);
	std::vector <SgAsmStatement*> dest_list = parent->get_dest();
	//        info->indirectJumpAndReturnTargets[target].clear();
        for (size_t i = 0; i < dest_list.size(); ++i) {
          ROSE_ASSERT (isSgAsmInstruction(dest_list[i]));
          //cerr << "Adding ret target " << std::hex << dest_list[i]->get_address() << " to " << std::hex << target->get_address() << endl;
          //info->indirectJumpAndReturnTargets[target].insert(dest_list[i]->get_address());
          info->incomingEdges[isSgAsmInstruction(dest_list[i])].insert(target->get_address());
        }

	std::vector <SgAsmStatement*>::iterator it3 = dest_list.begin();
	for (; it3!=dest_list.end();++it3) {
	  SgAsmInstruction* dest = isSgAsmInstruction(*it3);
	  if (dest) {
	    dest->append_sources(target);
	    //cerr << " appending source to " << dest->get_address() << "   target: " << target->get_address() << endl;
	  }
	} // for
      } else { // if parent
	cerr << "   ERROR :: RET jumps :: no parent found for ret : " << target->class_name() << endl;
	//exit (0);
      }
    } // if ret
#endif
  }
  cerr << " >>>>>>>>> resolving RET jumps ... done." << endl;
}


uint64_t
RoseBin_FlowAnalysis::getAddressForNode(SgDirectedGraphNode* node) {
  ROSE_ASSERT(node);
  SgAsmInstruction* inst = isSgAsmInstruction(node->get_SgNode());
  uint64_t addr = 0;
  if (inst) {
    addr = inst->get_address();
  } else {
    // this is probably not an instruction
  }
  return addr;
}

void
RoseBin_FlowAnalysis::createInstToNodeTable() {
  //  tabletype::const_iterator it = deftable.begin();

// DQ (4/23/2009): We want the type defined in the base class.
// typedef rose_hash::hash_map <std::string, SgDirectedGraphNode*> nodeType;

  nodeType nodes = vizzGraph->nodes;
  nodeType::iterator it = nodes.begin();
  for (;it!=nodes.end();++it) {
    SgDirectedGraphNode* sgNode = it->second;
    //const multitype& mm = it->second;
    //int sizeOfMap = mm.size();
    //    cout << " size of map " << sizeOfMap << endl;
    // to make sure that only entries are copied that
    // are valid. This is a problem of ida python right now
    // it seems that there are double entries of nodes,
    // ie. a node exists two times == 2x address
    // this cant be allowed since the address is the key
    //if (sizeOfMap>1)
    deftable_instr[getAddressForNode(sgNode)]=sgNode;
  }


  if (RoseBin_support::DEBUG_MODE() &&
      nodes.size()!=deftable_instr.size())
    cerr << "ERROR :: def size table : " << nodes.size() <<
      " size table_instr : " << deftable_instr.size() << endl;
  // if the sizes are not equal, then we
  // have two nodes with the same address
  // this can currently happen due to a bug in the
  // ida to python scripts
  //ROSE_ASSERT(deftable.size()==deftable_instr.size());
  //ROSE_ASSERT(usetable.size()==usetable_instr.size());
}


/****************************************************
 * traverse the binary AST
 ****************************************************/
void
RoseBin_FlowAnalysis::visit(SgNode* node) {

  //  cerr << " traversing node " << binNode << endl;

  if (isSgAsmFunctionDeclaration(node) ) {
    SgAsmFunctionDeclaration* binDecl = isSgAsmFunctionDeclaration(node);
    string name = binDecl->get_name();
    ostringstream addrhex;
    addrhex << hex << setw(8) << binDecl->get_address() ;
    if (name=="") {
      name=addrhex.str();
      binDecl->set_name(name);
    }
    SgAsmStatement* stat = NULL;
    //    SgAsmStatementPtrList& list = binDecl->get_statementList();

    vector<SgAsmInstruction*> list;
    FindInstructionsVisitor vis;
    AstQueryNamespace::querySubTree(binDecl, std::bind2nd( vis, &list ));

    int sizeList = list.size();
    if (sizeList==0) {
      //cerr << " this function is empty!! " << endl;
      return;
    }

    //if ((func_nr % 1)==0)
    //  if (RoseBin_support::DEBUG_MODE())
    //    cout  << analysisName << " Func Nr: " << (++func_nr) << "  blocks:" <<
    //	   sizeList << "  ***************** checking function : " << name << endl;

    if (forward_analysis) {
      stat = list.front();
    } else {
      // get the last instruction in a function (backward control flow)
      stat = list.back();
    }
    ROSE_ASSERT(stat);

    //   if (RoseBin_support::DEBUG_MODE())
    //    cout << ">>>>>>>>>>>>>. checking statement in function : " << name << " .. " << stat->class_name() << endl;
    if (isSgAsmInstruction(stat)) {
      SgAsmInstruction* inst = isSgAsmInstruction(stat);
      ROSE_ASSERT(inst);
      // check the control flow of the first instruction in a function
      string typeFunction ="function";
      SgDirectedGraphNode* src=NULL;
      if (analysisName=="callgraph") {
	src = vizzGraph->createNode (name, typeFunction, binDecl->get_address(), vizzGraph->graph->get_graph_id(), false, binDecl);
      } else {
	src = vizzGraph->createNode (name, typeFunction, binDecl->get_address(), vizzGraph->graph->get_graph_id(), true, binDecl);
	string mnemonic=inst->get_mnemonic();
	SgDirectedGraphNode* trg = vizzGraph->createNode (mnemonic, typeNode, inst->get_address(), vizzGraph->graph->get_graph_id(),false, inst);
	string unp_name = unparseInstructionWithAddress(inst);
	trg->append_properties(SB_Graph_Def::name,unp_name);
	if (analysisName=="dfa")
	  trg->append_properties(SB_Graph_Def::dfa_standard,unp_name);
	SgDirectedGraphEdge* edge = vizzGraph->createEdge ( typeFunction, vizzGraph->graph->get_graph_id(), src, binDecl->get_address(), trg, inst->get_address());
	vizzGraph->setProperty(SB_Graph_Def::type, edge, RoseBin_support::ToString(SB_Edgetype::cfg));
      }

      local_visited.clear();
      worklist_forthisfunction.push(inst);

      funcDecl = binDecl;
      funcDeclNode = src;
      checkControlFlow(inst, sizeList, sizeList, name, func_nr);
    } else {
      cerr << "This is not an Instruction " << endl;
    }
  }
}


void
RoseBin_FlowAnalysis::checkControlFlow( SgAsmInstruction* binInst,
					int functionSize, int countDown,
					string& currentFunctionName, int func_nr) {

  while (!worklist_forthisfunction.empty()) {
    SgAsmInstruction* binInst = worklist_forthisfunction.top();
    worklist_forthisfunction.pop();
    ROSE_ASSERT(binInst);

    countDown--;

    int address = binInst->get_address();
    ostringstream addrhex;
    addrhex << hex << setw(8) << address ;


    vector <VirtualBinCFG::CFGEdge> vec;
    if (forward_analysis) {
      vec = binInst->cfgBinOutEdges(info);
      if (isSgAsmx86Instruction(binInst) && isSgAsmx86Instruction(binInst)->get_kind() == x86_call) {
        // vec.push_back(VirtualBinCFG::CFGEdge(VirtualBinCFG::CFGNode(binInst), VirtualBinCFG::CFGNode(info->getInstructionAtAddress(binInst->get_address() + binInst->get_raw_bytes().size())), info));
      }
    }
    else
      vec = binInst->cfgBinInEdges(info);


    string name = binInst->get_mnemonic();
    //    if (RoseBin_support::DEBUG_MODE())
    //         cout << " " << addrhex.str() << "  " << func_nr << " :: " << functionSize <<
    // 	"/" << countDown << "  ---------- next CFG instruction : " << name <<   "  vecSize : " << vec.size() << endl;



    for (int i=0; i < (int)vec.size(); i++) {
      VirtualBinCFG::CFGEdge edge = vec[i];
      VirtualBinCFG::CFGNode cfg_target = edge.target();
      VirtualBinCFG::CFGNode cfg_source = edge.source();
      if (!forward_analysis) {
	cfg_target = edge.source();
	cfg_source = edge.target();
      }
      SgAsmInstruction* bin_target = isSgAsmInstruction(cfg_target.getNode());
      SgAsmInstruction* thisbin = isSgAsmInstruction(cfg_source.getNode());
      ROSE_ASSERT(thisbin);
      SgAsmx86Instruction* thisbinX86 = isSgAsmx86Instruction(thisbin);
      ROSE_ASSERT (thisbinX86);

      string src_mnemonic = thisbin->get_mnemonic();
      int src_address = thisbin->get_address();
      if (analysisName=="callgraph")
	src_address = funcDecl->get_address();
      ostringstream addrhex_s;
      addrhex_s << hex << setw(8) << src_address ;

      SgDirectedGraphNode* src =NULL;
      string hexStr = addrhex_s.str();
      if (analysisName!="callgraph") {
	src= vizzGraph->checkIfGraphNodeExists(hexStr);
	if (src==NULL) {
	  src= vizzGraph->createNode (src_mnemonic, typeNode, src_address, vizzGraph->graph->get_graph_id(), false, thisbin);

	  string unp_name = unparseInstructionWithAddress(thisbin);
	  src->append_properties(SB_Graph_Def::name,unp_name);
	  if (analysisName=="dfa")
	    src->append_properties(SB_Graph_Def::dfa_standard,unp_name);
	}
	ROSE_ASSERT(src);
        if (thisbinX86->get_kind() == x86_call) {
          uint64_t returnAddr = thisbinX86->get_address() + thisbinX86->get_raw_bytes().size();
          SgAsmInstruction* retInsn = info->getInstructionAtAddress(returnAddr);
	  if (retInsn) {
	    //worklist_forthisfunction.push(retInsn);
	    //ostringstream tgthex_s;
	    //tgthex_s << hex << setw(8) << returnAddr ;
	    //string tgtStr = tgthex_s.str();
	    //SgDirectedGraphNode* tgt = vizzGraph->checkIfGraphNodeExists(tgtStr);

	    // tps (25 Aug 2008) : this line seems broken!
	    //string mne = retInsn->get_mnemonic();

	    //if (!tgt) {tgt = vizzGraph->createNode(mne, typeNode, returnAddr, vizzGraph->graph->get_graph_id(), false, retInsn);}
	    // cerr << " ------> Creating return edge : " << thisbinX86->get_address() << " " << returnAddr << endl;
	    // vizzGraph->createEdge( typeEdge, vizzGraph->graph->get_graph_id(), src, thisbinX86->get_address(), tgt, returnAddr);


	  }
        }
      }

      else if (analysisName=="callgraph") {
	// These are special cases that annotate the call graph (nodes)
	// so that the visualization can pick up the properties and color correctly
	if (thisbinX86->get_kind() == x86_jmp) {
	  if (thisbinX86->cfgBinOutEdges(info).empty()) {
	    funcDeclNode->append_properties(SB_Graph_Def::nodest_jmp,RoseBin_support::ToString("nodest_jmp"));
	  }
	}
	else if (thisbinX86->get_kind() == x86_call) {
	  //cerr << "CallGRAPH: Found call : " <<
	  //  RoseBin_support::HexToString(VirtualBinCFG::CFGNode(thisbinX86).getNode()->get_address()) << " to " <<
	  //  RoseBin_support::HexToString(VirtualBinCFG::CFGNode(info->getInstructionAtAddress(thisbinX86->get_address() + thisbinX86->get_raw_bytes().size())).getNode()->get_address()) <<  endl;

          vector<VirtualBinCFG::CFGEdge> dests = thisbinX86->cfgBinOutEdges(info);
          dests.push_back(VirtualBinCFG::CFGEdge(VirtualBinCFG::CFGNode(thisbinX86), VirtualBinCFG::CFGNode(info->getInstructionAtAddress(thisbinX86->get_address() + thisbinX86->get_raw_bytes().size())), info));
	  if (!dests.empty()) {
	    SgAsmNode* parent = isSgAsmNode(dests[0].target().getNode()->get_parent());
	    if (!db)
	      parent = isSgAsmNode(parent->get_parent());
	    if (parent) {
	      SgAsmFunctionDeclaration* funcdestparent = isSgAsmFunctionDeclaration(parent);
	      string trg_func_name = funcdestparent->get_name();
	      if (trg_func_name==currentFunctionName) {
		funcDeclNode->append_properties(SB_Graph_Def::itself_call,RoseBin_support::ToString("itself_call"));
	      }
	    }
	  } else {
	    funcDeclNode->append_properties(SB_Graph_Def::nodest_call,RoseBin_support::ToString("nodest_call"));
	    //cerr << " no destination found for call " << addrhex.str() << endl;
	  }
	}
	else if (thisbinX86->get_kind() == x86_int) {
	  funcDeclNode->append_properties(SB_Graph_Def::interrupt,RoseBin_support::ToString("interrupt"));
	}
      }


      if (bin_target!=NULL) {
	string trg_func_name = "";
	int trg_func_address =1;
	string hexStrf = "";

	SgAsmFunctionDeclaration* funcDeclparent=NULL;
	if (analysisName=="callgraph") {
	  SgAsmNode* parent = dynamic_cast<SgAsmNode*>(bin_target->get_parent());
	  if (parent==NULL)
	    continue;
	  if (!db)
	    parent = isSgAsmNode(parent->get_parent());
	  ROSE_ASSERT(parent);

	  funcDeclparent = isSgAsmFunctionDeclaration(parent);
	  ROSE_ASSERT(funcDeclparent);

	  trg_func_name = funcDeclparent->get_name();
	  trg_func_address = funcDeclparent->get_address();
	  ostringstream addrhex_tf;
	  addrhex_tf << hex << setw(8) << trg_func_address ;
	  hexStrf = addrhex_tf.str();
	  //cerr << " CALLGRAPH TARGET PARENT : " << hexStrf << endl;
	}

	string trg_mnemonic = bin_target->get_mnemonic();
	int trg_address = bin_target->get_address();
	ostringstream addrhex_t;
	addrhex_t << hex << setw(8) << trg_address ;

		if (RoseBin_support::DEBUG_MODE())
	 cout << "     OUTEDGES TO: vec[" << i << "/" << vec.size() << "]  :" <<
	   addrhex_t.str() << "  " << trg_mnemonic << endl;

	string hexStr = addrhex_t.str();
	SgDirectedGraphNode* trg=NULL;
	if (analysisName=="callgraph")
	  trg = vizzGraph->checkIfGraphNodeExists(hexStrf);
	else
	  trg = vizzGraph->checkIfGraphNodeExists(hexStr);

	bool target_visited = false;

// DQ (4/23/2009): We want the type defined in the base class.
// rose_hash::hash_map <string, SgAsmInstruction*>::iterator vis = local_visited.find(hexStr);
   rose_hash::hash_map <string, SgAsmInstruction*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator vis = local_visited.find(hexStr);

	if (vis!=local_visited.end())
	  target_visited=true;

	if (trg==NULL) {
	  if (analysisName=="callgraph") {
	    //	    cerr << " >>> TARGET FUNC NAME " << trg_func_name << endl;
	    trg = vizzGraph->createNode (trg_func_name, typeNode, trg_func_address, vizzGraph->graph->get_graph_id(),false, funcDeclparent);
	  }	  else
	    trg = vizzGraph->createNode (trg_mnemonic, typeNode, trg_address, vizzGraph->graph->get_graph_id(),false, bin_target);

	  string unp_name = unparseInstructionWithAddress(bin_target);
	  //cout << " (target==NULL) unparse name : " << unp_name << endl;
	  trg->append_properties(SB_Graph_Def::name,unp_name);
	  if (analysisName=="dfa")
	    trg->append_properties(SB_Graph_Def::dfa_standard,unp_name);

	} else {
	  string unp_name = unparseInstructionWithAddress(bin_target);
	  //cout << "    unparse name : " << unp_name << endl;
	  trg->append_properties(SB_Graph_Def::name,unp_name);
	  if (analysisName=="dfa")
	    trg->append_properties(SB_Graph_Def::dfa_standard,unp_name);
	}


	ROSE_ASSERT(trg);
	local_visited[hexStr] = bin_target;

	string name="";
	if (analysisName=="callgraph")
	  name = RoseBin_support::ToString(src_address)+RoseBin_support::ToString(trg_func_address);
	else
	  name = RoseBin_support::ToString(src_address)+RoseBin_support::ToString(trg_address);

	bool exists = vizzGraph->checkIfGraphEdgeExists(src, trg);
	if (!exists) {
	  if (analysisName=="callgraph") {
	    if (currentFunctionName!=trg_func_name && thisbinX86->get_kind() != x86_ret) {
	      SgDirectedGraphEdge* edge = vizzGraph->createEdge( typeEdge, vizzGraph->graph->get_graph_id(), funcDeclNode, src_address, trg, trg_func_address);
	      //cerr << "CallGraph : create edge : " << RoseBin_support::HexToString(src_address) << " to func : " << RoseBin_support::HexToString(trg_func_address) << endl;
	      vizzGraph->setProperty(SB_Graph_Def::type, edge, RoseBin_support::ToString(SB_Edgetype::cfg));
	    }
	  } else {
	    //string addr = RoseBin_support::HexToString(binInst->get_address());
	    //if (addr==" 8048392" || addr==" 80482fd")
	    //  cerr << " >>>>>>>>>> found " << addr << "  -- target_address : " << RoseBin_support::HexToString(trg_address) << endl;
	    SgDirectedGraphEdge* edge =vizzGraph->createEdge( typeEdge, vizzGraph->graph->get_graph_id(), src, src_address, trg, trg_address);
	    vizzGraph->setProperty(SB_Graph_Def::type, edge, RoseBin_support::ToString(SB_Edgetype::cfg));
	  }
	}



	if (analysisName!="callgraph") {
	// handle return edges
	SgAsmStatementPtrList sources = thisbin->get_sources();
	SgAsmStatementPtrList::iterator it = sources.begin();
	for (;it!=sources.end();++it) {
	  SgAsmInstruction* instT = isSgAsmInstruction(*it);
	  //cerr << " This node is called from : " << instT->get_address() << endl;
	  ostringstream addr_t;
	  addr_t << hex << setw(8) << instT->get_address() ;
	  SgDirectedGraphNode* trg =NULL;
	  string hexStr = addr_t.str();
	  trg= vizzGraph->checkIfGraphNodeExists(hexStr);
	  if (trg==NULL) {
	    string hexa = RoseBin_support::HexToString(instT->get_address());
	    hexa = hexa.substr(1,hexa.size());
	    string name = "0x"+hexa+":"+instT->get_mnemonic();
	    trg= vizzGraph->createNode (name, typeNode, instT->get_address(), vizzGraph->graph->get_graph_id(), false, instT);
	  }

	  bool exists = vizzGraph->checkIfGraphEdgeExists( trg,src);
	  if (!exists) {
	    bool same = sameParents(trg,src);
	    if (!same) {
	      SgDirectedGraphEdge* edge =vizzGraph->createEdge( typeEdge, vizzGraph->graph->get_graph_id(),  trg, instT->get_address(), src, src_address);
	      vizzGraph->setProperty(SB_Graph_Def::type, edge, RoseBin_support::ToString(SB_Edgetype::cfg));
	    }
	  }
	}
	}

	if (!target_visited) {
	  // check if target is in the same function!!!
	  SgAsmNode* block = bin_target;
	  if (!db)
	    block = isSgAsmNode(bin_target->get_parent());
	  ROSE_ASSERT(block);
	  SgAsmFunctionDeclaration* funcPar = isSgAsmFunctionDeclaration(block->get_parent());
	  if (funcPar) {
	    string nameFunc = funcPar->get_name();
	    if (nameFunc==currentFunctionName) {
	      //checkControlFlow(bin_target, functionSize, countDown, currentFunctionName);
	      worklist_forthisfunction.push(bin_target);
	    }
	  } else {
	    cerr << " ERROR:: Target Instruction has no parent! " << bin_target->class_name() << endl;
	  }
	} // if visited
      } else {
	nr_target_missed++;
	if (binInst)
	  cerr << " WARNING:: no target found for " << RoseBin_support::HexToString(binInst->get_address()) << " " << binInst->class_name() << endl;
      }
    }
  }

  //  if (RoseBin_support::DEBUG_MODE())
  //  cout << " ------------------------ done with instr: " << name << " " << addrhex.str() << endl;
}

