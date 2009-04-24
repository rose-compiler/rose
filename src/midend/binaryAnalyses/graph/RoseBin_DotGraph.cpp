/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 27 Sep07
 * Decription : DotGraph Visualization
 ****************************************************/

#include "rose.h"
#include "RoseBin_DotGraph.h"
#include <cctype>

using namespace std;
using namespace VirtualBinCFG;

#include "../RoseBin_FlowAnalysis.h"

void 
RoseBin_DotGraph::printProlog(  std::ofstream& myfile, string& fileType) {
  myfile << "digraph \"" << fileType << "\" {\n" << endl;
}

void 
RoseBin_DotGraph::printEpilog(  std::ofstream& myfile) {
  myfile << "}\n" << endl;
}

void 
RoseBin_DotGraph::printNodesCallGraph(std::ofstream& myfile) {
  //cerr << " Preparing graph - Nr of Nodes : " << nodes.size() << endl;

  int counter=nodes.size();

// DQ (4/23/2009): Added a typedef to refactor the specification of the type (and add the explicit reference to the hash function).
// rose_hash::hash_set <std::string> funcNames;
  typedef rose_hash::hash_set <std::string,rose_hash::hash_string> funcNamesType;
  funcNamesType funcNames;

  nodeType resultSet;

  typedef std::multimap < std::string, 
    std::pair <std::string, SgDirectedGraphNode*> > callNodeType;
  callNodeType callMap;
  callMap.clear();

  nodeType::iterator itn2 = nodes.begin();
  for (; itn2!=nodes.end();++itn2) {
    counter--;
    pair<string, SgDirectedGraphNode*> nt = *itn2;
    string hex_address = itn2->first;
    SgDirectedGraphNode* node = isSgDirectedGraphNode(itn2->second);
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    string funcName = func->get_name();
    int pos = funcName.find("+");
    if (pos<=0) pos=funcName.find("-");
    if (pos<=0) pos=funcName.length();
    funcName=funcName.substr(0,pos);
    bool found = true;

 // rose_hash::hash_set <std::string>::iterator funcNames_it=funcNames.find(funcName);
    funcNamesType::iterator funcNames_it=funcNames.find(funcName);

    if (funcNames_it==funcNames.end()) {
      funcNames.insert(funcName);
      found =false;
    }

    if (RoseBin_support::DEBUG_MODE())
      if ((counter % 10000)==0)
	cout << " preparing function " << counter << endl;
    if (!found)
      resultSet[itn2->first]=itn2->second;
    else
      callMap.insert(make_pair ( funcName, nt )) ;
  }

  //cerr << " Number of nodes in inverseMap : " << callMap.size() << endl;


  //cerr << " Writing graph to DOT - Nr of Nodes : " << nodes.size() << endl;
  int funcNr=0;
  nodeType::iterator itn = resultSet.begin();
  for (; itn!=resultSet.end();++itn) {
    string hex_address = itn->first;
    SgDirectedGraphNode* node = isSgDirectedGraphNode(itn->second);
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    ROSE_ASSERT(node);
    // specifies that this node has no destination address
    nodest_jmp = false;
    // specifies that there is a node that has a call error (calling itself)
    error =false;
    // specifies a call to a unknown location
    nodest_call = false;
    // specifies where its an int instruction
    interrupt = false;
    if (func) {
      map < int , string> node_p = node->get_properties();
      map < int , string>::iterator prop = node_p.begin();
      string name = "noname";
      string type = node->get_type();
      for (; prop!=node_p.end(); ++prop) {
	int addr = prop->first;
	// cerr << " dot : property for addr : " << addr << " and node " << hex_address << endl;
	if (addr==RoseBin_Def::nodest_jmp)
	  nodest_jmp = true;
	else if (addr==RoseBin_Def::itself_call)
	  error = true;
	else if (addr==RoseBin_Def::nodest_call)
	  nodest_call = true;
	else if (addr==RoseBin_Def::interrupt)
	  interrupt = true;
	else if (addr==RoseBin_Def::name)
	  name = prop->second;
      }
      

      funcNr++;
      if (RoseBin_support::DEBUG_MODE())
	cout << " Unparser Function : " << funcNr  << endl;

      string add ="";
      //if (grouping) {
      //	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
      //} else {
      if (nodest_jmp)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else if (nodest_call)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else if (interrupt)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=blue,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else if (error)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=lightblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=purple,fontname=\"7x13bold\",fontcolor=black,style=filled";
      //}

      string funcName = func->get_name();
      int pos = funcName.find("+");
      if (pos<=0) pos=funcName.find("-");
      if (pos<=0) pos=funcName.length();
      funcName=funcName.substr(0,pos);

      RoseBin_support::checkText(name);
      //cerr << " +++ funcName " << funcName << "     name: " << name << endl;
 
      myfile << "subgraph \"cluster_" <</*name*/ hex_address<< "\" { \n"; 
      myfile << "\"" << hex_address << "\"[label=\"" << hex_address << ":" 
	     << funcName << "\\n type = " << type << "\\n \"" << add <<"];\n"; 
      //	     << name << "\\n type = " << type << "\\n \"" << add <<"];\n"; 

      callNodeType::iterator inv = callMap.lower_bound(funcName);
      for (;inv!=callMap.upper_bound(funcName);++inv) {
	pair <std::string, SgDirectedGraphNode*>  itn = inv->second;
	string hex_address_n = itn.first;
	SgDirectedGraphNode* node = isSgDirectedGraphNode(itn.second);
	SgNode* internal = node->get_SgNode();
	SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
	string name_n="noname";
	if (func)
	  name_n=func->get_name();

	string type_n = node->get_type();
	map < int , string> node_p = node->get_properties();
	map < int , string>::iterator prop = node_p.begin();
	/*
	for (; prop!=node_p.end(); ++prop) {
	  int addr = prop->first;
	  // cerr << " dot : property for addr : " << addr << " and node " << hex_address << endl;
	  	  if (addr==RoseBin_Def::name)
	  name_n = prop->second;
	}
	*/
	//	cerr << "    +++ funcName " << funcName << "   name_n: " << name_n << endl;
	myfile << "\"" << hex_address_n << "\"[label=\"" << hex_address_n << ":" 
	       << name_n << "\\n type = " << type_n << "\\n \"" << add <<"];\n"; 
	//	<< name_n << "\\n type = " << type_n << "\\n \"" << add <<"];\n"; 
	
      }


      myfile << "} \n"; 
    } 
  }

}

void 
RoseBin_DotGraph::printNodes(    bool dfg, RoseBin_FlowAnalysis* flow, bool forward_analysis,
				 std::ofstream& myfile, string& recursiveFunctionName) {
  //ROSE_ASSERT(unparser);
  //bool firstFunc = true;
  //  cerr << " grouping : " << grouping << endl;
  // traverse nodes and visualize results of graph
  if ((dynamic_cast<RoseBin_CallGraphAnalysis*>(flow))!=NULL) {
    //cerr << " >>>> its a callgraph " << endl;
    printNodesCallGraph(myfile);
    return;
  }
  //cerr << " Preparing graph - Nr of Nodes : " << nodes.size() << "  forward analysis : " << forward_analysis << endl;
  int counter=nodes.size();
  inverse_nodesMap.clear();
  nodeType::iterator itn2 = nodes.begin();
  for (; itn2!=nodes.end();++itn2) {
    counter--;
    pair<string, SgDirectedGraphNode*> nt = *itn2;
    string hex_address = itn2->first;
    SgDirectedGraphNode* node = isSgDirectedGraphNode(itn2->second);
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    if (func)
      continue;
    SgAsmInstruction* bin_inst = isSgAsmInstruction(internal);
    SgAsmFunctionDeclaration* funcDecl_parent = 
      isSgAsmFunctionDeclaration(bin_inst->get_parent());
    if (    funcDecl_parent ==NULL)
      funcDecl_parent = isSgAsmFunctionDeclaration(bin_inst->get_parent()->get_parent());
    if (funcDecl_parent==NULL) {
      cerr << " ERROR : printNodes preparation . No parent found for node : " << bin_inst->class_name() <<
	"  " << hex_address << endl;
      continue;
    }
    if (RoseBin_support::DEBUG_MODE())
      if ((counter % 10000)==0)
	cout << " preparing function " << counter << endl;			     
    inverse_nodesMap.insert(make_pair ( funcDecl_parent, nt )) ;
  }


  //cerr << " Number of nodes in inverseMap : " << inverse_nodesMap.size() << endl;

  //cerr << " Writing graph to DOT - Nr of Nodes : " << nodes.size() << endl;
  int funcNr=0;
  nodeType::iterator itn = nodes.begin();
  for (; itn!=nodes.end();++itn) {
    string hex_address = itn->first;
    SgDirectedGraphNode* node = isSgDirectedGraphNode(itn->second);
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    ROSE_ASSERT(node);
    // specifies that this node has no destination address
    nodest_jmp = false;
    // specifies that there is a node that has a call error (calling itself)
    error =false;
    // specifies a call to a unknown location
    nodest_call = false;
    // specifies where its an int instruction
    interrupt = false;
    if (func) {
      map < int , string> node_p = node->get_properties();
      map < int , string>::iterator prop = node_p.begin();
      string name = "noname";
      string type = node->get_type();
      for (; prop!=node_p.end(); ++prop) {
	int addr = prop->first;
	// cerr << " dot : property for addr : " << addr << " and node " << hex_address << endl;
	if (addr==RoseBin_Def::nodest_jmp)
	  nodest_jmp = true;
	else if (addr==RoseBin_Def::itself_call)
	  error = true;
	else if (addr==RoseBin_Def::nodest_call)
	  nodest_call = true;
	else if (addr==RoseBin_Def::interrupt)
	  interrupt = true;
	else if (addr==RoseBin_Def::name)
	  name = prop->second;
      }
      

      funcNr++;
      if (RoseBin_support::DEBUG_MODE())
	cout << " Unparser Function : " << funcNr  << endl;

      string add ="";
      //if (grouping) {
      //	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
      //} else {
      if (nodest_jmp)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else if (nodest_call)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else if (interrupt)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=blue,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else if (error)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=lightblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=purple,fontname=\"7x13bold\",fontcolor=black,style=filled";
      //}

      RoseBin_support::checkText(name);
      if (grouping)
	myfile << "subgraph \"cluster_" <</*name*/ hex_address<< "\" { \n"; 
      myfile << "\"" << hex_address << "\"[label=\"" << hex_address << ":" 
	     << name << "\\n type = " << type << "\\n \"" << add <<"];\n"; 

      printInternalNodes(dfg, forward_analysis, myfile, name, func);
      if (grouping)
	myfile << "} \n"; 
    } 
  }
}
  


void 
RoseBin_DotGraph::printInternalNodes(    bool dfg, bool forward_analysis,
					 std::ofstream& myfile, string& recursiveFunctionName,
					 SgAsmFunctionDeclaration* p_binFunc) {

  inverseNodeType::iterator inv = inverse_nodesMap.lower_bound(p_binFunc);
  for (;inv!=inverse_nodesMap.upper_bound(p_binFunc);++inv) {
    pair <std::string, SgDirectedGraphNode*>  itn = inv->second;

    string hex_address = itn.first;
    SgDirectedGraphNode* node = isSgDirectedGraphNode(itn.second);
    string type = node->get_type();


    string name = "noname";
    string eval = "";
    string regs = "";

    // specifies that this node has no destination address
    nodest_jmp = false;
    // specifies that there is a node that has a call error (calling itself)
    error =false;
    // specifies a call to a unknown location
    nodest_call = false;
    // specifies where its an int instruction
    interrupt = false;
    // specifies whether a node has been visited (dfa)
    checked = false;

    dfa_standard = false;
    dfa_resolved_func =false;
    dfa_unresolved_func=false;
    dfa_conditional=false;
    bool bufferoverflow=false;
    string dfa_variable="";
    bool dfa_variable_found =false;
    string dfa_info="";
    string visitedCounter="";
    string variable="";
    map < int , string> node_p = node->get_properties();
    map < int , string>::iterator prop = node_p.begin();
    //string type = node->get_type();
    for (; prop!=node_p.end(); ++prop) {
      int addr = prop->first;
      //      cerr << " dot : property for addr : " << addr << " and node " << hex_address << " is " << prop->second << endl;
      if (addr==RoseBin_Def::name)
	name = prop->second;
      else if (addr==RoseBin_Def::eval)
	eval = prop->second;
      else if (addr==RoseBin_Def::regs)
	regs = prop->second;
      else if (addr==RoseBin_Def::variable)
	variable = prop->second;
      else if (addr==RoseBin_Def::nodest_jmp)
	nodest_jmp = true;
      else if (addr==RoseBin_Def::itself_call)
	error = true;
      else if (addr==RoseBin_Def::dfa_bufferoverflow) {
	bufferoverflow = true;
	dfa_variable = prop->second;
      } else if (addr==RoseBin_Def::nodest_call)
	nodest_call = true;
      else if (addr==RoseBin_Def::interrupt)
	interrupt = true;
      else if (addr==RoseBin_Def::done)
	checked = true;
      else if (addr==RoseBin_Def::dfa_standard) 
	dfa_standard = true;
      else if (addr==RoseBin_Def::dfa_resolved_func) {
	dfa_resolved_func = true;
	dfa_info = prop->second;
      } else if (addr==RoseBin_Def::dfa_unresolved_func) {
	dfa_unresolved_func = true;
	dfa_info = prop->second;
      } else if (addr==RoseBin_Def::dfa_variable) {
	dfa_variable = prop->second;
	dfa_variable_found = true;
      } else if (addr==RoseBin_Def::dfa_conditional_def) {
	dfa_conditional = true;
      } else if (addr==RoseBin_Def::visitedCounter) {
	visitedCounter = prop->second;
      } else {
	cerr << " *************** dotgraph: unknown property found :: " << addr << endl;
      }
    }

    ROSE_ASSERT(node);
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    if (func) 
      continue;
    SgAsmInstruction* bin_inst = isSgAsmInstruction(internal);
    SgAsmFunctionDeclaration* funcDecl_parent = 
      isSgAsmFunctionDeclaration(bin_inst->get_parent());
    if (    funcDecl_parent ==NULL)
      funcDecl_parent = isSgAsmFunctionDeclaration(bin_inst->get_parent()->get_parent());
    if (funcDecl_parent==NULL) {
      cerr << " ERROR : InternalNodes . No parent found for node : " << bin_inst->class_name() <<
	"  " << hex_address << endl;
      continue;
    }
    string name_parent = funcDecl_parent->get_name();
    if (name_parent!=recursiveFunctionName)
      continue;

    SgAsmx86Instruction* inst = isSgAsmx86Instruction(internal);

    if (inst) {
      type += " " + toString(inst->get_kind());
    }

    string add = "";
    if (inst->get_kind() == x86_call || inst->get_kind() == x86_ret ) {
      if (nodest_call)
      	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Blue\",fillcolor=orange,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else if (error)
      	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Blue\",fillcolor=lightblue,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else 
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=pink,fontname=\"7x13bold\",fontcolor=black,style=filled";
    } else if (inst->get_kind() == x86_jmp) {
      if (nodest_jmp)
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
      else
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
    } else
      if (x86InstructionIsControlTransfer(inst)) {
	if (inst->get_kind() == x86_int)
	  add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=blue,fontname=\"7x13bold\",fontcolor=black,style=filled";
	else
	  add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
      } else
	add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";

    if (checked)
      add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=grey,fontname=\"7x13bold\",fontcolor=black,style=filled";

    if (dfa_standard)
      add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=yellow,fontname=\"7x13bold\",fontcolor=black,style=filled";
    if (dfa_resolved_func)
      add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled";
    if (dfa_unresolved_func) 
      add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
    if (dfa_conditional) 
      add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=lightgreen,fontname=\"7x13bold\",fontcolor=black,style=filled";
    if (dfa_variable_found) 
      add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled";
    if (bufferoverflow) {
      add = ",shape=ellipse,regular=0, sides=5,peripheries=1,color=\"Black\",fillcolor=black,fontname=\"7x13bold\",fontcolor=white,style=filled";
      type = "Buffer Overflow" ;
    }
    regs += eval;
    string hex_name=hex_address.substr(1,hex_address.length());
    if (hex_name.size()>1 && hex_name[0]==' ')
      hex_name=hex_name.substr(1,hex_name.size());
    hex_name="0x"+hex_name;
    string nameL="0x";
    for (unsigned int j=2;j<name.size();++j){
      char c = name[j];
      if (c==' ') continue;
      else if (c==':')
	break;
      else
	nameL+=name[j];
      //	else 
      //  cerr << " not a digit : " << name[j] << endl;
    }
    //      string nameL=name.substr(0,9);
    if (type!="function") {
      if (hex_name!=nameL)
	cerr << " hexName : ." << hex_name << ". == ." << nameL << ".   out of : " << name << endl;
      //	cerr << " ERROR ................... SOMETHING WRONG HERE . ALLOWING THIS FOR NOW . " << endl;
      ROSE_ASSERT(hex_name==nameL);
    }
    myfile << "\"" << hex_address << "\"[label=\""  << name << "\\n" << dfa_info << dfa_variable <<
      " visited: " << visitedCounter <<"\\n" << 
      "type = " << type << "\\n" << variable << "\\n" << regs << "\"" << add <<"];\n"; 
  }

}


void RoseBin_DotGraph::printEdges( bool forward_analysis, std::ofstream& myfile, bool mergedEdges) {
  if (mergedEdges) {
    createUniqueEdges();
    printEdges_single(forward_analysis, myfile);
  } else
    printEdges_multiple(forward_analysis, myfile);
}

void RoseBin_DotGraph::printEdges_single( bool forward_analysis, std::ofstream& myfile) {
  // traverse edges and visualize results of graph
  cerr << " Writing singleEdge graph to DOT - Nr of unique Edges : " << unique_edges.size() << " compare to edges: " <<
    edges.size() << endl;
  int edgeNr=0;
  edgeTypeUnique::iterator it = unique_edges.begin();
  for (; it!=unique_edges.end();++it) {
    edgeNr++;
    if ((edgeNr % 20000) == 0)
      cout << " Writing graph to DOT - Nr of Edges : " << edges.size() << "/" << edgeNr << endl;
    SgDirectedGraphEdge* edge = it->second;
    printEdges(forward_analysis, myfile, edge);
  }
  //  nodesMap.clear();
}

void RoseBin_DotGraph::printEdges_multiple( bool forward_analysis, std::ofstream& myfile) {
  // traverse edges and visualize results of graph
  cerr << " Writing multiEdge graph to DOT - Nr of unique Edges : " << unique_edges.size() << " compare to edges: " <<
    edges.size() << endl;
  int edgeNr=0;
  edgeType::iterator it = edges.begin();
  for (; it!=edges.end();++it) {
    edgeNr++;
    if ((edgeNr % 20000) == 0)
      cout << " Writing graph to DOT - Nr of Edges : " << edges.size() << "/" << edgeNr << endl;
    SgDirectedGraphEdge* edge = it->second;
    printEdges(forward_analysis, myfile, edge);
  }
  //nodesMap.clear();
}


void RoseBin_DotGraph::printEdges( bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge ) {
  // traverse edges and visualize results of graph
  /*
    cerr << " Writing graph to DOT - Nr of Edges : " << edges.size() << endl;
    int edgeNr=0;
    edgeType::iterator it = edges.begin();
    for (; it!=edges.end();++it) {
    edgeNr++;
    if ((edgeNr % 5000) == 0)
    cout << " Writing graph to DOT - Nr of Edges : " << edges.size() << "/" << edgeNr << endl;
    //    string name = it->first;
    SgDirectedGraphEdge* edge = it->second;
  */
  SgDirectedGraphNode* source = isSgDirectedGraphNode(edge->get_from());
  SgDirectedGraphNode* target = isSgDirectedGraphNode(edge->get_to());

#if 0
  // extra check to ensure that nodes exist. If not, skip
  nodeType::iterator itn2 = nodes.begin();
  bool foundS=false;
  bool foundT=false;
  for (; itn2!=nodes.end();++itn2) {
    SgDirectedGraphNode* n = itn2->second;
    if (n==source) foundS=true;
    if (n==target) foundT=true;
  }
  if (foundS==false || foundT==false) {
    cerr <<"WARNING :: printEdges - edge not found. " << endl; 
    return;
    if (source && target) {
      SgAsmFunctionDeclaration* src = isSgAsmFunctionDeclaration(source->get_SgNode());
      SgAsmFunctionDeclaration* trg = isSgAsmFunctionDeclaration(target->get_SgNode());
      if (src && trg) {
	cerr <<"WARNING :: printEdges - edge not found: " << 
	  RoseBin_support::HexToString(src->get_address()) << " -> " << 
	  RoseBin_support::HexToString(trg->get_address()) << endl;
	//return;
      } else {
	if (src==NULL)
	  cerr <<"WARNING :: printEdges - src == NULL " << source->get_SgNode()->class_name() <<endl;
	else 
	  cerr <<"WARNING :: printEdges - trg == NULL " << target->get_SgNode()->class_name() <<endl;
      }
    } else {
      cerr <<"WARNING :: printEdges - source or target == NULL " <<endl;
    }
  }
#endif
  ROSE_ASSERT(source);
  ROSE_ASSERT(target);
  string from_hex = source->get_name();
  string to_hex =target->get_name();

  // fix the problem that instructions and functions may have the same address (key)
  if (grouping) {
    // fix this only if we use groups (clusters)
    if (isSgAsmFunctionDeclaration(source->get_SgNode()))
      from_hex+="_f";
    if (isSgAsmFunctionDeclaration(target->get_SgNode()))
      to_hex+="_f";
  }


  string edgeLabel="";
  map < int , string> edge_p = edge->get_properties();
  map < int , string>::iterator prop = edge_p.begin();
  //string type = node->get_type();
  for (; prop!=edge_p.end(); ++prop) {
    int addr = prop->first;
    // cerr << " dot : property for addr : " << addr << " and node " << hex_address << endl;
    if (addr==RoseBin_Def::edgeLabel)
      edgeLabel = prop->second;
  }

  string output = "\"" + from_hex + "\" -> \"" + to_hex + "\"[label=\"" + edgeLabel  + "\"" + "];\n";  
  SgAsmx86Instruction* contrl = isSgAsmx86Instruction(source->get_SgNode());
  if (contrl) {
    // we use either dest or dest_list
    // dest is used for single destinations during cfg run
    // dest_list is used for a static cfg image
    vector<VirtualBinCFG::CFGEdge> outEdges = contrl->cfgBinOutEdges(info);
    if (contrl->get_kind() == x86_call || isAsmUnconditionalBranch(contrl)) {
      SgAsmInstruction* next = info->getInstructionAtAddress(contrl->get_address() + contrl->get_raw_bytes().size());
      if (next) {
	//	  outEdges.push_back(VirtualBinCFG::CFGEdge(VirtualBinCFG::CFGNode(contrl, info), VirtualBinCFG::CFGNode(next, info), info));
      }
    }
    SgAsmInstruction* dest = outEdges.empty() ? NULL : outEdges.front().target().getNode();
    if (!x86InstructionIsControlTransfer(contrl) || contrl->get_kind() == x86_ret) dest = NULL;
    bool dest_list_empty = true;
    if (contrl->get_kind() == x86_ret)
      dest_list_empty = outEdges.empty();

    SgAsmInstruction* nextNode = isSgAsmInstruction(target->get_SgNode());
    ROSE_ASSERT(nextNode);
    if (dest) {
      string add = "";
      string type = "jmp_if";
      if (contrl->get_kind() == x86_call || contrl->get_kind() == x86_ret) {
	add= ",color=\"Red\",  style=\"dashed\"";
	type = "call";
      } else if (contrl->get_kind() == x86_jmp) {
	add= ",color=\"Red\",  style=\"dashed\"";
	type = "jmp";
      } else 
	add = ",color=\"Green\",  style=\"dashed\"";
      if (dest==nextNode) 
	output =  "\"" + from_hex + "\" -> \"" + to_hex + "\"[label=\"" + type +"\\n"+ edgeLabel+ "\""  + add +  "];\n"; 
      // the destination is the next node after a control transfer
      // for this, if the source is a call or jmp, we want to hide
      // the edge
      else 
	if (forward_analysis && 
	    (contrl->get_kind() == x86_call || contrl->get_kind() == x86_jmp)) {
	  add = ",color=\"Yellow\",  style=\"solid\"";
	  type="";
	  output =  "\"" + from_hex + "\" -> \"" + to_hex + "\"[label=\"" + type  + "\\n"+ edgeLabel+"\""  + add +  "];\n"; 
	}
    } else 
      if (contrl->get_kind() == x86_ret ) { //&& dest_list_empty) {
	// in case of a multiple return
	string add= ",color=\"Blue\",  style=\"dashed\"";
	string type = "ret";
	output =  "\"" + from_hex + "\" -> \"" + to_hex + "\"[label=\"" + type  + "\\n"+ edgeLabel+"\""  + add +  "];\n"; 
      } 
  }

  string type_n = getProperty(RoseBin_Def::type, edge);
  if (type_n==RoseBin_support::ToString(RoseBin_Edgetype::usage)) {
    string add= ",color=\"Black\",  style=\"dashed\"";
    output =  "\"" + from_hex + "\" -> \"" + to_hex + "\"[label=\""+ edgeLabel+"\""  + add +  "];\n";       
  }

  if (!(forward_analysis)) {
    SgAsmx86Instruction* thisNode = isSgAsmx86Instruction(source->get_SgNode());      
    SgAsmx86Instruction* nextNode = isSgAsmx86Instruction(target->get_SgNode());
    if (thisNode && nextNode) {
      SgAsmFunctionDeclaration* f_1 = isSgAsmFunctionDeclaration(thisNode->get_parent());
      SgAsmFunctionDeclaration* f_2 = isSgAsmFunctionDeclaration(nextNode->get_parent());
      if (f_1==NULL)
	f_1 = isSgAsmFunctionDeclaration(thisNode->get_parent()->get_parent());
      if (f_2==NULL)
	f_2 = isSgAsmFunctionDeclaration(nextNode->get_parent()->get_parent());
      if (f_1==f_2)
	if (nextNode->get_kind() == x86_call || nextNode->get_kind() == x86_jmp) {
	  string add = ",color=\"Green\",  style=\"invis\"";
	  string type="";
	  output =  "\"" + from_hex + "\" -> \"" + to_hex + "\"[label=\"" + type +"\\n"+ edgeLabel + "\""  + add +  "];\n"; 
	}
    }
  }

  myfile << output;
  //}
}


