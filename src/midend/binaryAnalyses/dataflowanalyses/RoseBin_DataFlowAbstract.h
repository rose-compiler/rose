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

class RoseBin_Variable  {
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

  RoseBin_DataTypes::DataTypes getType() {
    return type;
  }

  int getLength() { return length;}
  std::string getName() { return name;}

  std::string toString() {
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



class RoseBin_DataFlowAbstract {
 protected:
   rose_hash::hash_map <uint64_t, RoseBin_Variable*> variables;

   typedef rose_hash::hash_map <std::string, uint64_t,rose_hash::hash_string,rose_hash::eqstr_string> variablesReverseType;
// rose_hash::hash_map <std::string, uint64_t> variablesReverse;
   variablesReverseType variablesReverse;

   rose_hash::hash_map <uint64_t, RoseBin_Variable*> memory;

  RoseBin_Graph* vizzGraph;

  // definition of def-use data-structures. 
  // will need those for other analyses
  typedef std::multimap< std::pair<X86RegisterClass, int>, SgDirectedGraphNode*> multitype;
  //typedef std::map< SgDirectedGraphNode*, multitype> tabletype;
  //typedef __gnu_cxx::hash_multimap< std::pair<X86RegisterClass, int> , SgDirectedGraphNode*> multitype;
  typedef rose_hash::hash_map< SgDirectedGraphNode*, multitype> tabletype;


  // statistics
  int nrOfMemoryWrites;
  int nrOfRegisterWrites;

  std::set < SgDirectedGraphNode* > 
    getAnyFor(const multitype* multi, std::pair<X86RegisterClass, int> initName);

 public:
  tabletype deftable;
  tabletype usetable;


  RoseBin_DataFlowAbstract() {}
  virtual ~RoseBin_DataFlowAbstract() {}

  virtual bool run(std::string& name, SgDirectedGraphNode* node,SgDirectedGraphNode* before  ) =0;
  virtual bool runEdge(SgDirectedGraphNode* node, SgDirectedGraphNode* next)=0;
  virtual void init(RoseBin_Graph* vg)=0;

  SgDirectedGraphNode* getPredecessor(SgDirectedGraphNode* node);
  SgDirectedGraphNode* getSuccessor(SgDirectedGraphNode* node);

  int getDefinitionSize() {
    return deftable.size();
  }
  int getUsageSize() { return usetable.size();}

  int64_t check_isRegister(SgDirectedGraphNode* node,
			   SgAsmx86Instruction* inst,
			   std::pair<X86RegisterClass, int> codeSearch,
			   bool rightSide,
			   std::vector<std::pair<X86RegisterClass, int> >& regsOfInterest,
			   bool& cantTrack);



  int64_t check_isLeftSideRegister(SgAsmx86Instruction* inst,
				   std::pair<X86RegisterClass, int>  codeSearch);

  uint64_t getValueInExpression(SgAsmValueExpression* valExp);


  // should be removed after the reference from unparse_visitor is solved
  void getRegister_val(std::pair<X86RegisterClass, int>  code,
		   X86PositionInRegister pos,
		       uint64_t &qw_val) {};

  int64_t trackValueForRegister(
				SgDirectedGraphNode* node,
				std::pair<X86RegisterClass, int>  codeSearch,
				bool& cantTrack,
				SgAsmx86RegisterReferenceExpression* refExpr_rightHand);
  

  std::pair<X86RegisterClass, int>  
    check_isRegister(SgDirectedGraphNode* node, SgAsmx86Instruction* inst, 
		     bool rightSide, bool& memoryReference );

  SgAsmExpression* getOperand(SgAsmx86Instruction* inst,
						bool rightSide );

  uint64_t getValueInMemoryRefExp(SgAsmExpression* ref);

  bool isInstructionAlteringOneRegister(SgAsmx86Instruction* inst);
  bool altersMultipleRegisters(std::vector<std::pair<X86RegisterClass, int> >& codes,
			      SgAsmx86Instruction* inst);

  bool sameParents(SgDirectedGraphNode* node, SgDirectedGraphNode* next);

  void printDefTableToFile(std::string file);

  std::set < SgDirectedGraphNode* > 
    getDefFor(SgDirectedGraphNode* node, std::pair<X86RegisterClass, int>  initName) ;

  std::set < SgDirectedGraphNode* > 
    getUseFor(SgDirectedGraphNode* node, std::pair<X86RegisterClass, int>  initName);


    const std::multimap < std::pair<X86RegisterClass, int>  , SgDirectedGraphNode* >& 
    getDefMultiMapFor(SgDirectedGraphNode* node);

    const std::multimap< std::pair<X86RegisterClass, int>  , SgDirectedGraphNode* > &
    getUseMultiMapFor(SgDirectedGraphNode* node);

    uint64_t getValueOfInstr( SgAsmx86Instruction* inst,  bool rightSide );


  RoseBin_Variable* createVariable(uint64_t position,
				   std::vector<uint64_t> pos, 
				   std::string name, RoseBin_DataTypes::DataTypes type, std::string description,
				   int length, 
				   std::vector<uint64_t> value,
				   bool memoryRef);

  RoseBin_Variable* getVariable(uint64_t pos);
  RoseBin_Variable* getVariable(std::string var);

};

#endif

