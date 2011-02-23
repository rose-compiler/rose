/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 01 Oct07
 * Decription : GMLGraph Visualization
 ****************************************************/
// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 17,9 MB to 8,4MB
#include "sage3basic.h"
#include "RoseBin_GmlGraph.h"
#include "RoseBin_CallGraphAnalysis.h"

using namespace std;
using namespace VirtualBinCFG;

#include "../RoseBin_FlowAnalysis.h"

void
RoseBin_GMLGraph::printProlog(  std::ofstream& myfile, string& fileType) {
  myfile << "graph  [\n" << endl;
}

void
RoseBin_GMLGraph::printEpilog(  std::ofstream& myfile) {
  myfile << "]\n" << endl;
}

void
RoseBin_GMLGraph::printNodes(    bool dfg, RoseBin_FlowAnalysis* flow,bool forward_analysis,
                                 std::ofstream& myfile, string& recursiveFunctionName) {
  //bool firstFunc = true;
  // traverse nodes and visualize results of graph

  funcMap.clear();
  nodesMap.clear();
  //cerr << " Preparing graph - Nr of Nodes : " << nodes.size() << "  edges : " << edges.size() << endl;
  //SgGraphNodeList* gnodes = get_nodes();

  //  rose_graph_hash_multimap& nodes = get_nodes()->get_nodes();
  rose_graph_integer_node_hash_map nodes = get_node_index_to_node_map();
  int counter=nodes.size();
  int count=0;
  rose_graph_integer_node_hash_map::iterator itn2 = nodes.begin();
  for (; itn2!=nodes.end();++itn2) {
    counter++;
    count++;
    pair<int, SgGraphNode*> nt = *itn2;
    //    string hex_address = itn2->first;
    SgGraphNode* node = isSgGraphNode(itn2->second);
    string hex_address =node->get_name();
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    if (func) {
      vector<SgNode*> list;
      FindInstructionsVisitorx86 vis;
#ifdef _MSC_VER
//#pragma message ("WARNING: Removed reference to AstQueryNamespace::querySubTree()")
//        ROSE_ASSERT(false);

          // CH (4/7/2010): Workaround for MSVC
          vector<SgAsmx86Instruction*> temp_list;
          AstQueryNamespace::querySubTree(func, std::bind2nd( vis, &temp_list ));
          list.resize(temp_list.size());
          std::copy(temp_list.begin(), temp_list.end(), list.begin());
#else
          AstQueryNamespace::querySubTree(func, std::bind2nd( vis, &list ));
#endif
      int validInstructions = func->nrOfValidInstructions(list);
      funcMap[func]=counter;
      nodesMap[func]=count;
      string name = func->get_name();
      string text = "node [\n   id " + RoseBin_support::ToString(counter) + "\n  id_ " +
        RoseBin_support::ToString(counter) + "\n  label \"" + name + "\"\n  ";
      text +="   nrinstr_ "+RoseBin_support::ToString(validInstructions)+" \n";
      text+= " isGroup 1\n isGroup_ 1\n ]\n";

      if (name=="frame_dummy") {
        //cerr << text << endl;
        vector<SgNode*> succs = func->get_traversalSuccessorContainer();
        vector<SgNode*>::iterator j = succs.begin();
        //cerr << " ------------- free_dummy"<<endl;
        int ii=0;
        for (;j!=succs.end();j++) {
          //SgNode* n = *j;
          //cerr << " Node contained at pos:"<<ii<<"  - " << n->class_name() << endl;
          ii++;
        }
        //cerr << " number of validInstructions: " << validInstructions << endl;
      }



      if (grouping)
        myfile << text;
    }
    SgAsmInstruction* bin_inst = isSgAsmInstruction(internal);
    if (bin_inst)
      nodesMap[bin_inst]=count;

  }

  //cerr << " Writing graph to GML - Nr of Nodes : " << nodes.size() << endl;
  int pos=0;
  rose_graph_integer_node_hash_map::iterator itn = nodes.begin();
  for (; itn!=nodes.end();++itn) {
    pos++;
    //    string hex_address = itn->first;
    SgGraphNode* node = isSgGraphNode(itn->second);
    string hex_address = node->get_name();
    SgNode* internal = node->get_SgNode();
    SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(internal);
    string text="";
    // specifies that this node has no destination address
    nodest_jmp = false;
    // specifies that there is a node that has a call error (calling itself)
    error =false;
    // specifies a call to a unknown location
    nodest_call = false;
    // specifies where its an int instruction
    interrupt = false;
    if (func) {
      string name = func->get_name();
      //cerr << " if part name : " << name << endl;
      ROSE_ASSERT(node);
      if (grouping==false) {
        map < int , string> node_p = node->get_properties();
        map < int , string>::iterator prop = node_p.begin();
        string name = "noname";
        string type = "removed";//node->get_type();
        for (; prop!=node_p.end(); ++prop) {
          int addr = prop->first;
          //cerr << " gml : property for addr : " << addr << endl;
          if (addr==SgGraph::nodest_jmp)
            nodest_jmp = true;
          else if (addr==SgGraph::itself_call)
            error = true;
          else if (addr==SgGraph::nodest_call)
            nodest_call = true;
          else if (addr==SgGraph::interrupt)
            interrupt = true;
          //      else
          //  name = prop->second;
        }
      }

      int parent = funcMap[func];
      RoseBin_support::checkText(name);
      int length = name.length();
      text = "node [\n   id " + RoseBin_support::ToString(pos) + "\n   label \"" + name + "\"\n";
      if (nodest_jmp) {
        text += "  graphics [ h 30.0 w " + RoseBin_support::ToString(length*7) + " type \"circle\" fill \"#FF0000\"  ]\n";
        text +="   Node_Color_ \"FF0000\" \n";
      }      else if (nodest_call) {
        text += "  graphics [ h 30.0 w " + RoseBin_support::ToString(length*7) + " type \"circle\" fill \"#FF9900\"  ]\n";
        text +="   Node_Color_ \"FF9900\" \n";
      }      else if (interrupt) {
        text += "  graphics [ h 30.0 w " + RoseBin_support::ToString(length*7) + " type \"circle\" fill \"#0000FF\"  ]\n";
        text +="   Node_Color_ \"0000FF\" \n";
      }      else if (error) {
        text += "  graphics [ h 30.0 w " + RoseBin_support::ToString(length*7) + " type \"circle\" fill \"#66FFFF\"  ]\n";
        text +="   Node_Color_ \"66FFFF\" \n";
      }else {
        text += "  graphics [ h 30.0 w " + RoseBin_support::ToString(length*7) + " type \"circle\" fill \"#9933FF\"  ]\n";
        text +="   Node_Color_ \"9933FF\" \n";
      }
      text +="   gid "+RoseBin_support::ToString(parent)+" \n";
      text +="   skip_ 1 \n";
      text +="   gid_ "+RoseBin_support::ToString(parent)+" ]\n";
      // skip functions for now
      //      if (skipFunctions)
      //        text ="";
    } /*not a func*/ else {
      SgAsmx86Instruction* bin_inst = isSgAsmx86Instruction(internal);
      //cerr << " else part " << endl;
      SgAsmFunctionDeclaration* funcDecl_parent = NULL;
      if (bin_inst) {
        funcDecl_parent = isSgAsmFunctionDeclaration(bin_inst->get_parent());
        if (funcDecl_parent==NULL)
          funcDecl_parent = isSgAsmFunctionDeclaration(bin_inst->get_parent()->get_parent());
      }
      if (funcDecl_parent==NULL) {
        cerr << " ERROR : printNodes preparation . No parent found for node : " << bin_inst->class_name() <<
          "  " << hex_address << endl;
        continue;
      }
      if ((pos % 10000)==0)
        cout << " GMLGraph:: printing GML Nodes : " << pos << endl;
      string name = getInternalNodes(node, forward_analysis,bin_inst);
      int parent=0;
      map <SgAsmFunctionDeclaration*, int>::iterator its = funcMap.find(funcDecl_parent);
      if (its!=funcMap.end())
        parent = funcMap[funcDecl_parent];
      if (parent==0)
        cerr << " GMLGraph parent == 0 " << endl;

      if (onlyControlStructure && x86InstructionIsControlTransfer(bin_inst)) {
        text = "node [\n   id " + RoseBin_support::ToString(pos) + "\n" + name ;
        int instrnr = funcDecl_parent->get_childIndex(bin_inst);
        text +="   instrnr_ "+RoseBin_support::ToString(instrnr)+" \n";
        text +="   gid_ "+RoseBin_support::ToString(parent)+" \n";
        text +="   gid "+RoseBin_support::ToString(parent)+" ]\n";
      } else {
        text = "node [\n   id " + RoseBin_support::ToString(pos) + "\n" + name ;
        int instrnr = funcDecl_parent->get_childIndex(bin_inst);
        text +="   instrnr_ "+RoseBin_support::ToString(instrnr)+" \n";
        text +="   gid_ "+RoseBin_support::ToString(parent)+" \n";
        text +="   gid "+RoseBin_support::ToString(parent)+" ]\n";
      }
    }

    myfile << text;
    //    cerr << " this node : " << text << endl;
  }
  funcMap.clear();
}



std::string
RoseBin_GMLGraph::getInternalNodes(  SgGraphNode* node,
                                     bool forward_analysis, SgAsmNode* internal) {

  SgAsmInstruction* bin_inst = isSgAsmInstruction(internal);
  SgAsmx86Instruction* control = isSgAsmx86Instruction(internal);
  // get the unparser string!
  string eval = "";
  string name="noname";
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
  string dfa_info="";
  string dfa_variable="";
  string visitedCounter="";

  map < int , string> node_p = node->get_properties();
  map < int , string>::iterator prop = node_p.begin();
  string type = "removed";//node->get_type();
  for (; prop!=node_p.end(); ++prop) {
    int addr = prop->first;
    // cerr << " dot : property for addr : " << addr << " and node " << hex_address << endl;
    if (addr==SgGraph::name)
      name = prop->second;
    else if (addr==SgGraph::eval)
      eval = prop->second;
    else if (addr==SgGraph::regs)
      regs = prop->second;
    else if (addr==SgGraph::nodest_jmp)
      nodest_jmp = true;
    else if (addr==SgGraph::itself_call)
      error = true;
    else if (addr==SgGraph::nodest_call)
      nodest_call = true;
    else if (addr==SgGraph::interrupt)
      interrupt = true;
    else if (addr==SgGraph::done)
      checked = true;
    else if (addr==SgGraph::dfa_standard)
      dfa_standard = true;
    else if (addr==SgGraph::dfa_resolved_func) {
      dfa_resolved_func = true;
      dfa_info = prop->second;
    } else if (addr==SgGraph::dfa_unresolved_func) {
      dfa_unresolved_func = true;
      dfa_info = prop->second;
    } else if (addr==SgGraph::dfa_variable) {
      dfa_variable = prop->second;
    } else if (addr==SgGraph::visitedCounter) {
      visitedCounter = prop->second;
    } else {
      cerr << " *************** dotgraph: unknown property found :: " << addr << endl;
    }
  }

  if (bin_inst) {
    type += " " + bin_inst->class_name();
  }

  string add = "";
  string typeNode = "";
  if (control->get_kind() == x86_call || control->get_kind() == x86_ret) {
    typeNode += " Type_ \"[ 67108864 FUNCTION_NODE ]\" \n";
    if (nodest_call)
      add = " FF9900 ";
    else if (error)
      add = " 3399FF ";
    else
      add = " FFCCFF ";
  } else if (control->get_kind() == x86_jmp) {
    typeNode += " Type_ \"[  67108864 FILE_NODE ]\" \n";
    if (nodest_jmp)
      add = " FF0000 ";
    else
      add = " 00FF00 ";
  } else
    if (x86InstructionIsControlTransfer(control)) {
      typeNode += " Type_ \"[  67108864 CLASS_NODE ]\" \n";
      if (control->get_kind() == x86_int)
        add = " 0000FF ";
      else
        add = " 008800 ";
    } else {
      add = " FFFF66 ";
    }
  if (checked)
    add = " 777777 ";

  if (dfa_standard)
    add = " FFFF00 ";
  if (dfa_resolved_func)
    add = " 00FF00 ";
  if (dfa_unresolved_func)
    add = " FF0000 ";

  string nodeStr = "";

  regs+=eval;
  // cant get the extra register info printed in gml format
  // because multiline is not supported? (tps 10/18/07)
  name = name/*+" " +regs + "  " +dfa_variable+" "+"vis:"+visitedCounter */;
  nodeStr= "   label \"" + name+"\"\n "+typeNode;
  int length = name.length();


  SgAsmx86Instruction* pre = NULL; // isSgAsmx86Instruction(bin_inst->cfgBinFlowInEdge());
  if (pre==NULL) {
    // first node
    nodeStr +="   first_ 1 \n";
  } else {
    if (pre->get_kind() == x86_ret || pre->get_kind() == x86_hlt) {
      // this instruction must be suspicious
      add =" 0000FF ";
    }
  }
  nodeStr += "  Node_Color_ " + add + "  \n";
  nodeStr += "  graphics [ h 30.0 w " + RoseBin_support::ToString(length*7) + " type \"rectangle\" fill \"#" + add +  "\"  ]\n";



  return nodeStr;
}

void RoseBin_GMLGraph::printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                        bool forward_analysis, std::ofstream& myfile, bool mergedEdges) {
  if (mergedEdges) {
    createUniqueEdges();
    printEdges_single(info, forward_analysis, myfile);
  } else
    printEdges_multiple(info, forward_analysis, myfile);
}

void RoseBin_GMLGraph::printEdges_single( VirtualBinCFG::AuxiliaryInformation* info,
                        bool forward_analysis, std::ofstream& myfile) {
  // traverse edges and visualize results of graph
  //SgGraphEdgeList* gedges = get_edges();
        rose_graph_integer_edge_hash_multimap edges =get_node_index_to_edge_multimap_edgesOut();
        //cerr << " Writing singleEdge graph to GML - Nr of unique Edges : " << unique_edges.size() << " compare to edges: " <<
        //edges.size() << endl;
  int edgeNr=0;
  rose_graph_integer_edge_hash_multimap::iterator it = unique_edges.begin();
  for (; it!=unique_edges.end();++it) {
    edgeNr++;
    if ((edgeNr % 5000) == 0)
      cout << " Writing graph to GML - Nr of Edges : " << edges.size() << "/" << edgeNr << endl;
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it->second);
    printEdges(info, forward_analysis, myfile, edge);
  }
  nodesMap.clear();
}

void RoseBin_GMLGraph::printEdges_multiple( VirtualBinCFG::AuxiliaryInformation* info,
                        bool forward_analysis, std::ofstream& myfile) {
  // traverse edges and visualize results of graph
  //  SgGraphEdgeList* gedges = get_edges();
  rose_graph_integer_edge_hash_multimap edges =get_node_index_to_edge_multimap_edgesOut();
  //cerr << " Writing multiEdge graph to GML - Nr of unique Edges : " << unique_edges.size() << " compare to edges: " <<
  // edges.size() << endl;
  int edgeNr=0;
  rose_graph_integer_edge_hash_multimap::iterator it = edges.begin();
  for (; it!=edges.end();++it) {
    edgeNr++;
    if ((edgeNr % 5000) == 0)
      cout << " Writing graph to GML - Nr of Edges : " << edges.size() << "/" << edgeNr << endl;
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it->second);
    printEdges(info, forward_analysis, myfile, edge);
  }
  nodesMap.clear();
}


void RoseBin_GMLGraph::printEdges( VirtualBinCFG::AuxiliaryInformation* info, bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge) {
  // traverse edges and visualize results of graph
    SgGraphNode* source = isSgGraphNode(edge->get_from());
    SgGraphNode* target = isSgGraphNode(edge->get_to());
    ROSE_ASSERT(source);
    ROSE_ASSERT(target);

    string edgeLabel="";
    map < int , string> edge_p = edge->get_properties();
    map < int , string>::iterator prop = edge_p.begin();
    //string type = node->get_type();
    for (; prop!=edge_p.end(); ++prop) {
      int addr = prop->first;
      // cerr << " dot : property for addr : " << addr << " and node " << hex_address << endl;
      if (addr==SgGraph::edgeLabel)
        edgeLabel = prop->second;
      if (edgeLabel.length()>1)
        if (edgeLabel[0]!='U')
          edgeLabel="";
    }

    SgAsmStatement* binStat_s = isSgAsmStatement(source->get_SgNode());
    SgAsmStatement* binStat_t = isSgAsmStatement(target->get_SgNode());
    if (binStat_s==NULL || binStat_t==NULL) {
      //cerr << "binStat_s==NULL || binStat_t==NULL" << endl;
    } else {
      map <SgAsmStatement*, int>::iterator it_s = nodesMap.find(binStat_s);
      map <SgAsmStatement*, int>::iterator it_t = nodesMap.find(binStat_t);
      int pos_s=0;
      int pos_t=0;
      if (it_s!=nodesMap.end())
        pos_s = it_s->second;
      if (it_t!=nodesMap.end())
        pos_t = it_t->second;

      if (pos_s==0 || pos_t==0) {
        //cerr << " GMLGraph edge, node == 0 " << endl;
      }

      string output = "edge [\n  label \""+edgeLabel+"\"\n source " + RoseBin_support::ToString(pos_s) +
        "\n   target " + RoseBin_support::ToString(pos_t) + "\n";

      // ------------------
      SgAsmx86Instruction* contrl = isSgAsmx86Instruction(source->get_SgNode());
      string add = "";
      if (contrl && x86InstructionIsControlTransfer(contrl)) {
        // the source is a control transfer function

        // we use either dest or dest_list
        // dest is used for single destinations during cfg run
        // dest_list is used for a static cfg image
        vector<VirtualBinCFG::CFGEdge> outEdges = contrl->cfgBinOutEdges(info);
        SgAsmx86Instruction* dest = isSgAsmx86Instruction(outEdges.empty() ? NULL : outEdges.back().target().getNode());
        bool dest_list_empty = true;
        if (contrl->get_kind() == x86_ret)
          dest_list_empty = outEdges.empty();

        SgAsmInstruction* nextNode = isSgAsmInstruction(target->get_SgNode());
        ROSE_ASSERT(nextNode);

        if (dest) {
          //string type = "jmp_if";
          if (dest==nextNode) {
            if (contrl->get_kind() == x86_call || contrl->get_kind() == x86_ret) {
              add += "   graphics [ type \"line\" style \"dashed\" arrow \"last\" fill \"#FF0000\" ]  ]\n";
            } else if (contrl->get_kind() == x86_jmp) {
              add += "   graphics [ type \"line\" style \"dashed\" arrow \"last\" fill \"#FF0000\" ]  ]\n";
            } else
              add += "   graphics [ type \"line\" style \"dashed\" arrow \"last\" fill \"#00FF00\" ]  ]\n";
          } else
            if (forward_analysis &&
                (contrl->get_kind() == x86_call || contrl->get_kind() == x86_jmp)) {
              add += "   graphics [ type \"line\" arrow \"last\" fill \"#FFFF00\" ]  ]\n";
            }
        } else
          if (contrl->get_kind() == x86_ret ) { //&& dest_list_empty) {
            // in case of a multiple return
            add += "   graphics [ type \"line\" style \"dashed\" arrow \"last\" fill \"#3399FF\" ]  ]\n";
          }
      }

      string type_n = getProperty(SgGraph::type, edge);
      if (type_n==RoseBin_support::ToString(SgGraph::usage)) {
        add = "   graphics [ type \"line\" style \"dashed\" arrow \"last\" fill \"#000000\" ]  ]\n";
      }

      // skip the function declaration edges for now
      //      bool blankOutput=false;
      //if (skipFunctions)
      //if (isSgAsmFunctionDeclaration(binStat_s))
      //  blankOutput=true;
      if (skipInternalEdges) {
        SgAsmx86Instruction* contrl = isSgAsmx86Instruction(source->get_SgNode());
        if (contrl && x86InstructionIsControlTransfer(contrl) && contrl->get_kind() != x86_ret) {
          if (contrl->get_kind() == x86_call)
            output += "  Edge_Color_ FF0000  \n  Type_ \"[ 33554432 CALL_EDGE ]\" \n";
          else if (contrl->get_kind() == x86_jmp)
            output += "  Edge_Color_ 00FF00  \n  Type_ \"[ 33554432 FILECALL_EDGE ]\" \n";
          else
            output += "  Edge_Color_ 0000FF  \n   ";
        }
        //else
        //  blankOutput=true;
      }

      if (add=="")
        output += "   graphics [ type \"line\" arrow \"last\" fill \"#000000\" ]  ]\n";
      else output +=add;

      myfile << output;
    }

    //  }
  // ----------
    //  nodesMap.clear();

}

