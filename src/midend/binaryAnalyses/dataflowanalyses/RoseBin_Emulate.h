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

  void assignMemory(uint64_t position, uint64_t value) ROSE_DEPRECATED("no longer supported");
  uint64_t getMemory(uint64_t position) ROSE_DEPRECATED("no longer supported");



  void assignRegister(std::pair<X86RegisterClass, int>  code,
                      RoseBin_support::X86PositionInRegister pos,
                      uint8_t &b_val,
                      uint16_t &w_val,
                      uint32_t &dw_val,
                      uint64_t &qw_val) ROSE_DEPRECATED("no longer supported");

  void assignRegister(std::pair<X86RegisterClass, int>  code,
                      uint64_t &qw_val) ROSE_DEPRECATED("no longer supported");


  void clearRegisters() ROSE_DEPRECATED("no longer supported");
  uint64_t getRegister(std::pair<X86RegisterClass, int>  code) ROSE_DEPRECATED("no longer supported");

  std::string printRegister(std::string text, uint64_t reg) ROSE_DEPRECATED("no longer supported");

  uint64_t getRandomValue(int val) ROSE_DEPRECATED("no longer supported");


  void getRegister_val(std::pair<X86RegisterClass, int>  code,
                       RoseBin_support::X86PositionInRegister pos,
                       uint8_t &b_val,
                       uint16_t &w_val,
                       uint32_t &dw_val,
                       uint64_t &qw_val) ROSE_DEPRECATED("no longer supported");

  void getRegister_val(std::pair<X86RegisterClass, int>  code,
                       RoseBin_support::X86PositionInRegister pos,
                       uint64_t &qw_val) ROSE_DEPRECATED("no longer supported");



 public:

  RoseBin_Emulate(GraphAlgorithms* algo):RoseBin_DataFlowAbstract(algo) {}
  ~RoseBin_Emulate() {}

  bool run(std::string& name, SgGraphNode* node, SgGraphNode* before) ROSE_DEPRECATED("no longer supported");
  std::string evaluateRegisters() ROSE_DEPRECATED("no longer supported");
  bool evaluateInstruction( SgAsmX86Instruction* inst, std::string& operands) ROSE_DEPRECATED("no longer supported") ;

  bool runEdge( SgGraphNode* node, SgGraphNode* next) ROSE_DEPRECATED("no longer supported") {
    return false;
  }

  void init(RoseBin_Graph* vg) ROSE_DEPRECATED("no longer supported") {
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

