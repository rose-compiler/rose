/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 7Sep07
 * Decription : Dataflow analysis
 ****************************************************/
#include "rose.h"
#include "RoseBin_DefUseAnalysis.h"

using namespace std;

/**********************************************************
 *  Replace an element in the table
 *********************************************************/
void RoseBin_DefUseAnalysis::replaceElement(SgDirectedGraphNode* sgNode,
					    std::pair<X86RegisterClass, int> initName) {
  // if the node is contained but not identical, then we overwrite it
  // otherwise, we do nothing
  deftable[sgNode].erase(deftable[sgNode].lower_bound(initName), deftable[sgNode].upper_bound(initName));
  deftable[sgNode].insert(make_pair(initName,sgNode));
}


/**********************************************************
 *  Search for the value for a certain key in the multimap
 *********************************************************/
bool RoseBin_DefUseAnalysis::searchMulti(const multitype* multi, std::pair<X86RegisterClass, int> initName) {
  multitype::const_iterator it1, it2;
  pair <multitype::const_iterator, multitype::const_iterator> iter = multi->equal_range(initName);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    // value is contained
    return true;
  }
  return false;
}

/**********************************************************
 *  Search for the value for a certain key in the multimap
 *********************************************************/
bool 
RoseBin_DefUseAnalysis::searchMulti(const multitype* multi, 
				    std::pair<X86RegisterClass, int> initName,
					 SgDirectedGraphNode* val) {
  multitype::const_iterator it1, it2;
  pair <multitype::const_iterator, multitype::const_iterator> iter = multi->equal_range(initName);
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphNode* node = it1->second;
    //exact match ?
    //node && val && 
    if (node==val)
      return true;
  }
  return false;
}

/**********************************************************
 *  print out the table containing all nodes
 *********************************************************/
void RoseBin_DefUseAnalysis::printDefMap() {
  printAnyMap(&deftable);
}

/**********************************************************
 *  print out the table containing all nodes
 *********************************************************/
void RoseBin_DefUseAnalysis::printUseMap() {
  printAnyMap(&usetable);
}

/**********************************************************
 *  print out the table containing all nodes
 *********************************************************/
void RoseBin_DefUseAnalysis::printAnyMap(tabletype* tabl) {
  int pos = 0;
  cout << "\n **************** MAP ************************** " << endl;
  for (tabletype::const_iterator i = tabl->begin(); i != tabl->end(); ++i) {
    pos++;
    SgDirectedGraphNode* sgNode = (*i).first;
    string label="NULL";
    multitype multi = (*i).second;
    if (sgNode && isSgAsmInstruction(sgNode->get_SgNode())) {
      string name = sgNode->get_name();
      int theNode = isSgAsmInstruction(sgNode->get_SgNode())->get_address();
      label = ""+RoseBin_support::HexToString(theNode) + " var: " + name; 
      cout << pos << ": " << label << endl;
      printMultiMap(&multi);
    } else
      cout << pos << ": NULL ------------- " << endl;
  }
  cout << " **************** MAP ************************** \n" << endl;
}

/**********************************************************
 *  print out the multimap
 *********************************************************/
void RoseBin_DefUseAnalysis::printMultiMap(const multitype* multi) {
  for (multitype::const_iterator j = multi->begin(); j != multi->end(); ++j) {
    std::pair<X86RegisterClass, int> sgInitMM = (*j).first;
    SgDirectedGraphNode* sgNodeMM = (*j).second;

    string registerName = unparseX86Register(sgInitMM.first, 
                                             sgInitMM.second,
                                             x86_regpos_qword);

    ROSE_ASSERT(sgNodeMM);
    cout << "  ..  initName:" << registerName << " ( " <<  
      // RoseBin_support::ToString(isSgAsmInstruction(sgInitMM->get_SgNode())->get_address()) << 
      " ) " << " - SgNode " <<
      RoseBin_support::HexToString(isSgAsmInstruction(sgNodeMM->get_SgNode())->get_address()) << endl;
  }
}



/**********************************************************
 *  For a CFG Node, follow the two incoming edges and get
 *  the other node (than theNodeBefore)
 *********************************************************/
void 
RoseBin_DefUseAnalysis::getOtherInNode(vector <SgDirectedGraphNode*>& otherNodes,
				       SgDirectedGraphNode* cfgNode, 
				       SgDirectedGraphNode* oneNode){
  vector <SgDirectedGraphNode*> in_edges;
  vizzGraph->getPredecessors(cfgNode, in_edges);
  vector<SgDirectedGraphNode* >::const_iterator i = in_edges.begin();
  for (; i != in_edges.end(); ++i) {
    SgDirectedGraphNode* inNode= *i;
    if (inNode!=oneNode) {
      //otherNode = inNode;
      otherNodes.push_back(inNode);
      if (RoseBin_support::DEBUG_MODE())
	cout << "getOtherInNode:: other: " << inNode << "  previous: " <<
	  oneNode << "  size of in: " << in_edges.size() << endl;
    }
  }
  //return otherNode;
}


/**********************************************************
 *  check if two multimaps are equal
 *********************************************************/
bool RoseBin_DefUseAnalysis::checkElementsForEquality(const multitype* t1, const multitype* t2) {
  // if every element of t2 is contained in t1, then no change
  // occured in the map
  typedef set<pair<std::pair<X86RegisterClass, int>, SgDirectedGraphNode*> > st;
  st s1(t1->begin(), t1->end());
  st s2(t2->begin(), t2->end());
  assert (s1.size() == t1->size());

  if (s2.size() != t2->size())
      printMultiMap(t2);
  assert (s2.size() == t2->size());
  return s1 != s2;
}


/**********************************************************
 *  Union of two maps
 *********************************************************/
void RoseBin_DefUseAnalysis::mapDefUnion(SgDirectedGraphNode* before, SgDirectedGraphNode* other, SgDirectedGraphNode* sgNode) {
  mapAnyUnion(&deftable, before, other, sgNode);
}

/**********************************************************
 *  Union of two maps
 *********************************************************/
void RoseBin_DefUseAnalysis::mapUseUnion(SgDirectedGraphNode* before, SgDirectedGraphNode* other, SgDirectedGraphNode* sgNode) {
  mapAnyUnion(&usetable, before, other, sgNode);
}

/**********************************************************
 *  Union of two maps
 *********************************************************/
void RoseBin_DefUseAnalysis::mapAnyUnion(tabletype* tabl, SgDirectedGraphNode* before, 
					 SgDirectedGraphNode* other, SgDirectedGraphNode* sgNode) {

  
  
  bool beforeFound = true;
  if ((*tabl).find(before)==(*tabl).end())
    beforeFound = false;
  bool otherFound = true;
  if ((*tabl).find(other)==(*tabl).end())
    otherFound = false;

  if (!beforeFound) {
    if (!otherFound)
      (*tabl)[sgNode].clear();
    else 
      (*tabl)[sgNode]=(*tabl)[other];
  } else {
    if (!otherFound) 
      (*tabl)[sgNode]=(*tabl)[before];
    else {

      const multitype& multiA  = (*tabl)[before];
      const multitype& multiB  = (*tabl)[other];
      std::set<std::pair<std::pair<X86RegisterClass, int>, SgDirectedGraphNode*> > s_before(multiA.begin(), multiA.end());
      // ROSE_ASSERT (s_before.size() == (*tabl)[before].size());
#if 0
      std::set<std::pair<std::pair<X86RegisterClass, int>, SgDirectedGraphNode*> > s_other((*tabl)[other].begin(), (*tabl)[other].end());
      ROSE_ASSERT (s_other.size() == (*tabl)[other].size());
#endif
      
      if (RoseBin_support::DEBUG_MODE())
	cout << sgNode->get_name() <<"  Elements before union  a (before) : " << multiA.size() << "  b (other) : " << multiB.size() << endl;
      
      s_before.insert(multiB.begin(), multiB.end());
      multitype multiC(s_before.begin(), s_before.end());
#if 0
      set_union(multiA.begin(), multiA.end(), multiB.begin(), multiB.end(),
		inserter(multiC, multiC.end()),
		std::less<std::pair<std::pair<X86RegisterClass, int>, SgDirectedGraphNode*> >() );
#endif
      (*tabl)[sgNode].swap(multiC);
      //(*tabl)[sgNode]=multiC;
	  
      if (RoseBin_support::DEBUG_MODE()) {
	cout << sgNode->get_name() << "  Elements after union  c: " << multiC.size()  << endl;
	//      printDefMap();
      }
      
          }
      }

#if 0
  std::set<std::pair<std::pair<X86RegisterClass, int>, SgDirectedGraphNode*> > s((*tabl)[sgNode].begin(), (*tabl)[sgNode].end());
  ROSE_ASSERT (s.size() == (*tabl)[sgNode].size());
#endif

}


/**********************************************************
 *  Search for the value for a certain key in the map
 *********************************************************/
bool RoseBin_DefUseAnalysis::searchDefMap(SgDirectedGraphNode* node) {
  return searchMap(&deftable, node);
}

/**********************************************************
 *  Search for the value for a certain key in the map
 *********************************************************/
bool RoseBin_DefUseAnalysis::searchMap(const tabletype* ltable, SgDirectedGraphNode* node) {
  bool isCurrentValueContained=false;
  tabletype::const_iterator i =ltable->find(node);
  if (i!=ltable->end())
    isCurrentValueContained=true;
  return isCurrentValueContained;
}


/**********************************************************
 *  Add an element to the table
 *********************************************************/
void RoseBin_DefUseAnalysis::addDefElement(SgDirectedGraphNode* sgNode,
					   std::pair<X86RegisterClass, int> initName,
					   SgDirectedGraphNode* defNode) {
  addAnyElement(&deftable, sgNode, initName, defNode);
}

/**********************************************************
 *  Add an element to the table
 *********************************************************/
void RoseBin_DefUseAnalysis::addUseElement(SgDirectedGraphNode* sgNode,
					   std::pair<X86RegisterClass, int> initName,
					   SgDirectedGraphNode* defNode) {
  addAnyElement( &usetable, sgNode, initName, defNode);
}

/**********************************************************
 *  Add an element to the table
 *********************************************************/
void RoseBin_DefUseAnalysis::addAnyElement(tabletype* tabl, SgDirectedGraphNode* sgNode,
					   std::pair<X86RegisterClass, int> initName,
					   SgDirectedGraphNode* defNode) {
  bool contained = searchMulti(&(*tabl)[sgNode], initName, defNode);
  if (!contained)
    (*tabl)[sgNode].insert(make_pair(initName, defNode));
}


string 
RoseBin_DefUseAnalysis::getElementsAsStringForNode(bool def,SgDirectedGraphNode* node) {
  string label="";
  multitype mm;
  if (def)
    mm = getDefMultiMapFor(node);
  else
    mm = getUseMultiMapFor(node);
  multitype::iterator it;
  for (it=mm.begin(); it!=mm.end();++it) {
    string registerName = unparseX86Register(it->first.first, it->first.second,
                                             x86_regpos_qword);
    SgDirectedGraphNode* n = it->second;
    ROSE_ASSERT(n);
    SgAsmInstruction* inst = isSgAsmInstruction(n->get_SgNode());
    ROSE_ASSERT(inst);
    int addr = inst->get_address();
    string addr_str = RoseBin_support::HexToString(addr);
    if (def)
      label += "Def of "+registerName+" at "+addr_str+"\\n";
    else
      label += "Use of "+registerName+" at "+addr_str+"\\n";
  }
  return label;
}

bool
RoseBin_DefUseAnalysis::runEdge(SgDirectedGraphNode* node, 
			    SgDirectedGraphNode* next) {
  // follow control flow through all nodes
  // and assign for each edge the defMap

  set<SgDirectedGraphEdge*> edges = vizzGraph->getEdge(node,next);
  set<SgDirectedGraphEdge*>::iterator it = edges.begin();
  for (;it!=edges.end();++it) {
    SgDirectedGraphEdge* edge = *it;
    if (edge) {
      string type_n = vizzGraph->getProperty(SB_Graph_Def::type, edge);
      string label = "";
      if (type_n==RoseBin_support::ToString(SB_Edgetype::usage))
	label = getElementsAsStringForNode(false,next);
      else if (type_n==RoseBin_support::ToString(SB_Edgetype::cfg)) {
	label = getElementsAsStringForNode(true,node);
	//label += getElementsAsStringForNode(false,node);
      }
      vizzGraph->setProperty(SB_Graph_Def::edgeLabel, edge, label);
      //cerr << " setting property to edge : " << label << endl;
    }
  }
  return false;
}



/**********************************************************
 * plain copy of the table
 *********************************************************/
void 
RoseBin_DefUseAnalysis::handleCopy(bool def,SgDirectedGraphNode* sgNode,
				      SgDirectedGraphNode* sgNodeBefore) {
  ROSE_ASSERT(sgNode);
  vector <SgDirectedGraphNode*> pre;
  vizzGraph->getPredecessors(sgNode, pre);
  int nrOfInEdges = pre.size();
  if (sgNodeBefore==NULL) 
    sgNodeBefore = pre.back();
  ROSE_ASSERT(sgNodeBefore);

  if (nrOfInEdges<=1 ) {
    /* --debug
    multitype oldTable = getDefMultiMapFor(sgNode);
    if (RoseBin_support::DEBUG_MODE())
      cout << " DefCopy :: nrOfElements in oldTable : " << oldTable.size() << endl;
    // ---- */
    if (vizzGraph->isValidCFGEdge(sgNode,sgNodeBefore))
      if (def)
	mapDefUnion(sgNodeBefore, NULL, sgNode);
      else
	mapUseUnion(sgNodeBefore, NULL, sgNode);
    /* -- debug
    multitype dTable = getDefMultiMapFor(sgNode);
    if (RoseBin_support::DEBUG_MODE())
      cout << " DefCopy :: nrOfElements in newTable : " << dTable.size() << endl;
    // ----- */
  } else {
    // otherwise, it we have more than one in-edge, we union the maps
    //if (RoseBin_support::DEBUG_MODE())
    //  cout << " DefCopy :: inEdges > 1 : union of sgNodeBefore and sgNode. "  << endl;
    if (vizzGraph->isValidCFGEdge(sgNode, sgNodeBefore))
      if (def)
	mapDefUnion(sgNodeBefore, NULL, sgNode);
      else
	mapUseUnion(sgNodeBefore, NULL, sgNode);

    vector <SgDirectedGraphNode*> inNodes;
    getOtherInNode(inNodes, sgNode, sgNodeBefore);
    vector <SgDirectedGraphNode*>::iterator it = inNodes.begin();
    for (;it!=inNodes.end();++it) {
      SgDirectedGraphNode* otherInNode = *it;
      if (otherInNode!=NULL) {
	if (vizzGraph->isValidCFGEdge(sgNode, otherInNode))
	  if (def)
	    mapDefUnion(sgNode, otherInNode, sgNode);
	  else
	    mapUseUnion(sgNode, otherInNode, sgNode);
      } else {
	cout << " !!!!!!! ERROR !!!!!!!!!   Other In Node is NULL ! " << endl;
      }
    }
  }
}


void 
RoseBin_DefUseAnalysis::clearRegisters() {
  deftable.clear();
  //deftable_instr.clear();
  usetable.clear();
  //usetable_instr.clear();
}

bool 
RoseBin_DefUseAnalysis::run(string& name, SgDirectedGraphNode* node,
			    SgDirectedGraphNode* nodeBefore ) {
  // check known function calls and resolve variables
  ROSE_ASSERT(node);
  bool hasChanged=false;

  multitype oldMul = getDefMultiMapFor(node);

  if (RoseBin_support::DEBUG_MODE()) 
    cout << " .. running DefUseAnalysis on Node : " << name << endl;
  SgAsmx86Instruction* asmNode = isSgAsmx86Instruction(node->get_SgNode());
  if (asmNode) {
    // find out if instruction is a definition (altering instruction)
    vector<std::pair<X86RegisterClass, int> > codes;
    bool alteringManyInstructions = altersMultipleRegisters(codes, asmNode);
    bool alteringInstruction = false;
    bool definition=false;
    bool use=false;

    // check if right hand side has use of register
    bool isMemoryReferenceR = false;
    std::pair<X86RegisterClass, int> codeR = 
      check_isRegister(node, asmNode, true, isMemoryReferenceR); // true == right hand side
    if (!isMemoryReferenceR && 
	codeR.first != x86_regclass_unknown) {
      use = true;
    }

    // check definition of left hand side
    if (alteringManyInstructions==false)
      alteringInstruction = isInstructionAlteringOneRegister(asmNode);
    else definition=true;

    if (alteringInstruction) {
      bool isMemoryReference = false;
      std::pair<X86RegisterClass, int> code = 
	check_isRegister(node, asmNode, false, isMemoryReference); // false == left hand side
      
      // make sure that there is a register on the left hand side
      if (isMemoryReference) {
	// we have found a write to a memory location
	if (RoseBin_support::DEBUG_MODE()) 
	  cout << "    Instruction does alter memory . " <<  endl;
	node->append_properties(SB_Graph_Def::dfa_unresolved_func,"");
	nrOfMemoryWrites++;
      } else 
	if (code.first != x86_regclass_unknown) {
	  // we have found a write to a register
	  // find out the registerName
	  string registerName = unparseX86Register(code.first, code.second, 
						   x86_regpos_qword);
	  // for visualization add property that this
	  // node is accessing and changing a register
	  if (RoseBin_support::DEBUG_MODE()) 
	    cout << "    Instruction does alter register . " << registerName << endl;
	  codes.push_back(code);
	  definition=true;
	}
    } else {
      if (RoseBin_support::DEBUG_MODE()) 
	cout << "    Instruction does not alter register or memory. " << endl;
    }

    if (definition) {
      bool condInst = RoseBin_support::isConditionalInstruction(asmNode);
      condInst = condInst || RoseBin_support::isConditionalFlagInstruction(asmNode);

      handleCopy(true,node, nodeBefore);

      // iterator through each register - if more than one is being changed
      while (codes.size()>0) {
	std::pair<X86RegisterClass, int> code = codes.back();
	codes.pop_back();

	bool isRegisterContained = false;
	bool isExactNodeContained = false;
	multitype mul = getDefMultiMapFor(node);
	if (mul.size()>0) {
	  isRegisterContained = searchMulti(&mul, code);
	  isExactNodeContained = searchMulti(&mul, code, node);
	}

	// color the node as definition (green)
	node->append_properties(SB_Graph_Def::dfa_resolved_func,"");
    
	if (isRegisterContained==false) {
	  nrOfRegisterWrites++;
	  // current value is not contained, merge it to the map
	  if (RoseBin_support::DEBUG_MODE()) 
	    cout << " current Value is not contained . " << endl;
	  // add this register to the def table
	  addDefElement(node, code, node);
	} else {
	  // current value is contained
	  if (RoseBin_support::DEBUG_MODE()) 
	    cout << " current Value is contained . " <<  endl;
	  //multitype oldTable = getDefMultiMapFor(node);
	  // add a new element if it is a conditional definition
	  // otherwise default : replace that element
	  if (condInst) {
	    if (RoseBin_support::DEBUG_MODE()) {
	      string regName = unparseX86Register(code.first, code.second, x86_regpos_qword);
	      cout << " conditional isnt : " << unparseInstruction(asmNode)<<
		"   regName " << regName << "   exactnode contained : " << RoseBin_support::resBool(isExactNodeContained) << endl;
	    }
	    addDefElement(node, code, node);
	    node->append_properties(SB_Graph_Def::dfa_conditional_def,"");
	  }
	  else
	    replaceElement(node, code);
	}
      }
    } // if definition
    else {
      if (RoseBin_support::DEBUG_MODE()) 
	cout << " >> current node is not a definition. COPYING !!! "  << endl;
      handleCopy(true,node, nodeBefore);
    }

    if (use) {
      // we have a usage of a register
      string registerNameR = unparseX86Register(codeR.first, codeR.second, 
						x86_regpos_qword);
      if (RoseBin_support::DEBUG_MODE()) 
	cout << ",,,Found usage of register: " << registerNameR << endl;

      bool isRegisterContained = false;
      multitype mul = getUseMultiMapFor(node);
      if (mul.size()>0) 
	isRegisterContained = searchMulti(&mul, codeR);
      if (!isRegisterContained) 
	addUseElement(node, codeR, node);

      // we add an edge to the graph (for usage)
      string name = "";
      set<SgDirectedGraphNode*> nodeTargetset = getDefFor(node, codeR);
      set<SgDirectedGraphNode*>::iterator it_t = nodeTargetset.begin();
      for (;it_t!=nodeTargetset.end();++it_t) {
	SgDirectedGraphNode* nodeTarget = *it_t;
	if (nodeTarget) {
	  SgAsmInstruction* inst_t = isSgAsmInstruction(nodeTarget->get_SgNode());
	  if (inst_t) {
	    int addr_t = inst_t->get_address();
	    name = RoseBin_support::HexToString(addr_t)+" use : " +registerNameR+"\\n";
	    set<SgDirectedGraphEdge*> edges = vizzGraph->getEdge(nodeTarget, node);
	    set<SgDirectedGraphEdge*>::iterator ed = edges.begin();
	    bool usageEdge=false;
	    string type_n="not";
	    for (;ed!=edges.end();++ed){
	      SgDirectedGraphEdge* edge = *ed;
	      if (edge)
		type_n = vizzGraph->getProperty(SB_Graph_Def::type, edge);
		if (type_n==RoseBin_support::ToString(SB_Edgetype::usage))
		usageEdge=true;
	    }
	    // if there is no usage edge, we want to create one...
	    if (usageEdge==false) {
	      SgDirectedGraphEdge* edge = 
		vizzGraph->createEdge(type_n, 0, nodeTarget, addr_t, node, asmNode->get_address() );
		vizzGraph->setProperty(SB_Graph_Def::name, edge, name);
		vizzGraph->setProperty(SB_Graph_Def::type, edge, RoseBin_support::ToString(SB_Edgetype::usage));
	    }
	  }
	} // if nodetarget
      } // for
    }

  } // is an instruction
  //  nodeBefore = node;

  multitype newMul = getDefMultiMapFor(node);  
  hasChanged =  checkElementsForEquality(&oldMul, &newMul);
  if (hasChanged)
    ROSE_ASSERT(oldMul.size()< newMul.size());
  return hasChanged;
}



