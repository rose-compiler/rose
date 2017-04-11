/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep7 07
 * Decription : Data flow Analysis
 ****************************************************/

#ifndef __RoseBin_DataFlowAbstract__
#define __RoseBin_DataFlowAbstract__

#include <stdio.h>
#include <iostream>
#include "RoseBin_Graph.h"
//#include "RoseBin.h"
#include "GraphAlgorithms.h"

class RoseBin_Variable  {
 public:
  //remove later!
  //  typedef rose_hash::unordered_map <std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string> nodeType;
  //typedef rose_graph_node_edge_hash_multimap edgeType;
  //  typedef rose_graph_hash_multimap nodeType;

 private:
  uint64_t position;
  std::string name;
  RoseBin_DataTypes::DataTypes type;
  std::string description;
  int length;
  std::vector<uint64_t> value;
 public:
  RoseBin_Variable(uint64_t pos,
                   std::string n,
                   RoseBin_DataTypes::DataTypes t, std::string d, int l,
                   std::vector<uint64_t> v) {
    position=pos;
    name =n;
    type =t;
    description=d;
    length=l;
    value = v;
  }

  RoseBin_DataTypes::DataTypes getType() ROSE_DEPRECATED("no longer supported") {
    return type;
  }

  int getLength() ROSE_DEPRECATED("no longer supported") { return length;}
  std::string getName() ROSE_DEPRECATED("no longer supported") { return name;}

  std::string toString() ROSE_DEPRECATED("no longer supported") {
    std::string values="";
    std::vector<uint64_t>::iterator it = value.begin();
    for (;it!=value.end();++it) {
      uint64_t var_int = *it;
      std::string var_str = RoseBin_support::HexToString(var_int);
      values += ""+ var_str + " ";
    }
    std::string val = "("+RoseBin_support::HexToString(position)+") "+name+"("
      +description+ ") ["+
      RoseBin_support::getTypeName(type)+" "+
      RoseBin_support::ToString(length)+" Values: ("+values+")] ";
    return val;
  }
};



class RoseBin_DataFlowAbstract { //: public GraphAlgorithms {
 protected:
   rose_hash::unordered_map <uint64_t, RoseBin_Variable*> variables;
   GraphAlgorithms* g_algo;
// CH (4/9/2010): Use boost::unordered instead
//#ifdef _MSC_VER
#if 0
//   typedef rose_hash::unordered_map <std::string, uint64_t,rose_hash::hash_string> variablesReverseType;
   typedef rose_hash::unordered_map <std::string, uint64_t> variablesReverseType;
#else
   // CH (4/13/2010): Use boost::hash<string> instead
   //typedef rose_hash::unordered_map <std::string, uint64_t,rose_hash::hash_string,rose_hash::eqstr_string> variablesReverseType;
   typedef rose_hash::unordered_map <std::string, uint64_t> variablesReverseType;
#endif

// rose_hash::unordered_map <std::string, uint64_t> variablesReverse;
   variablesReverseType variablesReverse;

   rose_hash::unordered_map <uint64_t, RoseBin_Variable*> memory;



  // definition of def-use data-structures.
  // will need those for other analyses
  typedef std::multimap< std::pair<X86RegisterClass, int>, SgGraphNode*> multitype;
  //typedef std::map< SgGraphNode*, multitype> tabletype;
  //typedef __gnu_cxx::hash_multimap< std::pair<X86RegisterClass, int> , SgGraphNode*> multitype;
// CH (4/9/2010): Use boost::unordered instead
//#ifdef _MSC_VER
#if 0
//  typedef rose_hash::unordered_map< SgGraphNode*, multitype,rose_hash::hash_graph_node> tabletype;
        // tps (12/07/2009) This seemed to work before with the above line, hmm..
  typedef rose_hash::unordered_map< SgGraphNode*, multitype> tabletype;
#else
  typedef rose_hash::unordered_map< SgGraphNode*, multitype,rose_hash::hash_graph_node,rose_hash::eqstr_graph_node> tabletype;
#endif

  // statistics
  int nrOfMemoryWrites;
  int nrOfRegisterWrites;

  std::set < SgGraphNode* >
    getAnyFor(const multitype* multi, std::pair<X86RegisterClass, int> initName) ROSE_DEPRECATED("no longer supported");

 public:
  tabletype deftable;
  tabletype usetable;
  RoseBin_Graph* vizzGraph;

// DQ (5/9/2009): Move a (two) non-pure-virtual out-of-line virtual member functions to be 
// defined first in the class so that the vtable will be put into the translation unit where
// these are defined (this avoid the error: undefined reference to `vtable for RoseBin_DefUseAnalysis'. 
  SgGraphNode* getPredecessor(SgGraphNode* node) ROSE_DEPRECATED("no longer supported");
  SgGraphNode* getSuccessor(SgGraphNode* node) ROSE_DEPRECATED("no longer supported");

  RoseBin_DataFlowAbstract(GraphAlgorithms* algo) {g_algo=algo;}
  virtual ~RoseBin_DataFlowAbstract() {}

  virtual bool run(std::string& name, SgGraphNode* node,SgGraphNode* before  ) =0;
  virtual bool runEdge(SgGraphNode* node, SgGraphNode* next)=0;
  virtual void init(RoseBin_Graph* vg)=0;

  int getDefinitionSize() ROSE_DEPRECATED("no longer supported") {
    return deftable.size();
  }
  int getUsageSize() ROSE_DEPRECATED("no longer supported") { return usetable.size();}

  int64_t check_isRegister(SgGraphNode* node,
                           SgAsmX86Instruction* inst,
                           std::pair<X86RegisterClass, int> codeSearch,
                           bool rightSide,
                           std::vector<std::pair<X86RegisterClass, int> >& regsOfInterest,
                           bool& cantTrack) ROSE_DEPRECATED("no longer supported");



  int64_t check_isLeftSideRegister(SgAsmX86Instruction* inst,
                                   std::pair<X86RegisterClass, int>  codeSearch) ROSE_DEPRECATED("no longer supported");

  uint64_t getValueInExpression(SgAsmValueExpression* valExp) ROSE_DEPRECATED("no longer supported");

  int64_t trackValueForRegister(
                                SgGraphNode* node,
                                std::pair<X86RegisterClass, int>  codeSearch,
                                bool& cantTrack,
                                SgAsmRegisterReferenceExpression* refExpr_rightHand) ROSE_DEPRECATED("no longer supported");


  std::pair<X86RegisterClass, int>
    check_isRegister(SgGraphNode* node, SgAsmX86Instruction* inst,
                     bool rightSide, bool& memoryReference, bool& registerReference ) ROSE_DEPRECATED("no longer supported");

  SgAsmExpression* getOperand(SgAsmX86Instruction* inst,
                                                bool rightSide ) ROSE_DEPRECATED("no longer supported");

  uint64_t getValueInMemoryRefExp(SgAsmExpression* ref) ROSE_DEPRECATED("no longer supported");

  bool isInstructionAlteringOneRegister(SgAsmX86Instruction* inst);
  bool altersMultipleRegisters(std::vector<std::pair<X86RegisterClass, int> >& codes,
                              SgAsmX86Instruction* inst) ROSE_DEPRECATED("no longer supported");

  bool sameParents(SgGraphNode* node, SgGraphNode* next) ROSE_DEPRECATED("no longer supported");

  void printDefTableToFile(std::string file) ROSE_DEPRECATED("no longer supported");

  std::set < SgGraphNode* >
    getDefFor(SgGraphNode* node, std::pair<X86RegisterClass, int>  initName)  ROSE_DEPRECATED("no longer supported");

  std::set < SgGraphNode* >
    getUseFor(SgGraphNode* node, std::pair<X86RegisterClass, int>  initName) ROSE_DEPRECATED("no longer supported");


    const std::multimap < std::pair<X86RegisterClass, int>  , SgGraphNode* >&
    getDefMultiMapFor(SgGraphNode* node) ROSE_DEPRECATED("no longer supported");

    const std::multimap< std::pair<X86RegisterClass, int>  , SgGraphNode* > &
    getUseMultiMapFor(SgGraphNode* node) ROSE_DEPRECATED("no longer supported");

    uint64_t getValueOfInstr( SgAsmX86Instruction* inst,  bool rightSide ) ROSE_DEPRECATED("no longer supported");


  RoseBin_Variable* createVariable(uint64_t position,
                                   std::vector<uint64_t> pos,
                                   std::string name, RoseBin_DataTypes::DataTypes type, std::string description,
                                   int length,
                                   std::vector<uint64_t> value,
                                   bool memoryRef) ROSE_DEPRECATED("no longer supported");

  RoseBin_Variable* getVariable(uint64_t pos) ROSE_DEPRECATED("no longer supported");
  RoseBin_Variable* getVariable(std::string var) ROSE_DEPRECATED("no longer supported");

};

#endif

