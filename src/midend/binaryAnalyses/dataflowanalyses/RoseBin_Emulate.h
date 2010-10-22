/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep7 07
 * Decription : Data flow Analysis
 ****************************************************/

#ifndef __RoseBin_Emulate__
#define __RoseBin_Emulate__

#include "RoseBin_DataFlowAbstract.h"

class RoseBin_Emulate : public RoseBin_DataFlowAbstract{
 private:

  rose_hash::unordered_map <uint64_t, uint64_t> memory;

  uint64_t  rax;
  uint64_t  rbx;
  uint64_t  rcx;
  uint64_t  rdx;
  uint64_t  rdi;
  uint64_t  rsi;
  uint64_t  rsp;
  uint64_t  rbp;

  bool ZF;

  // set this currently statically in file
  bool isCode64bit;

  void assignMemory(uint64_t position, uint64_t value);
  uint64_t getMemory(uint64_t position);



  void assignRegister(std::pair<X86RegisterClass, int>  code,
		      RoseBin_support::X86PositionInRegister pos,
		      uint8_t &b_val,
		      uint16_t &w_val,
		      uint32_t &dw_val,
		      uint64_t &qw_val);

  void assignRegister(std::pair<X86RegisterClass, int>  code,
		      uint64_t &qw_val);


  /*
  int64_t trackValueForRegister(SgGraphNode* node,
				std::pair<X86RegisterClass, int>  codeSearch,
				bool& cantTrack,
				SgAsmRegisterReferenceExpression* refExpr_rightHand);
  */

  void clearRegisters();
  uint64_t getRegister(std::pair<X86RegisterClass, int>  code);

  std::string printRegister(std::string text, uint64_t reg);

  uint64_t getRandomValue(int val);


  void getRegister_val(std::pair<X86RegisterClass, int>  code,
                       RoseBin_support::X86PositionInRegister pos,
                       uint8_t &b_val,
                       uint16_t &w_val,
                       uint32_t &dw_val,
                       uint64_t &qw_val);

  void getRegister_val(std::pair<X86RegisterClass, int>  code,
                       RoseBin_support::X86PositionInRegister pos,
                       uint64_t &qw_val);



 public:

  RoseBin_Emulate(GraphAlgorithms* algo):RoseBin_DataFlowAbstract(algo) {}
  ~RoseBin_Emulate() {}

  bool run(std::string& name, SgGraphNode* node, SgGraphNode* before);
  std::string evaluateRegisters();
  bool evaluateInstruction( SgAsmx86Instruction* inst, std::string& operands) ;

  bool runEdge( SgGraphNode* node, SgGraphNode* next) {
    return false;
  }

  void init(RoseBin_Graph* vg) {
    vizzGraph = vg;
    rax = 0xFFFFFFFF;
    rbx = 0xFFFFFFFF;
    rcx = 0xFFFFFFFF;
    rdx = 0xFFFFFFFF;
    rdi = 0xFFFFFFFF;
    rsi = 0xFFFFFFFF;
    rsp = 0xFFFFFFFF;
    rbp = 0xFFFFFFFF;
    isCode64bit=false;
    ZF=false;
  }

};

#endif

