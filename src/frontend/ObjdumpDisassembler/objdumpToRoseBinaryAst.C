#include <rose.h>
#include <string>
#include <stack>
#include <map>
#include <cstdio>
#include <stdint.h>
#include <errno.h>
#include "sageSupport.h"

#include "RoseBin_support.h"

using namespace std;

// From sageSupport.C:
FILE* popenReadFromVector(const vector<string>& argv);
int pcloseFromVector(FILE* f);

namespace ObjdumpToRoseBinaryAst {

  enum StackEntryKind {SEK_INT, SEK_STRING, SEK_NODE};

  struct StackEntry {
    StackEntryKind kind;
    uint64_t intVal;
    string stringVal;
    SgAsmNode* nodeVal;

    StackEntry(): kind(SEK_INT), intVal(999), stringVal(), nodeVal(0) {}

    StackEntry(const StackEntry& e): kind(e.kind), intVal(e.intVal), stringVal(e.stringVal), nodeVal(e.nodeVal) {}

    uint64_t asInt() const {
      ROSE_ASSERT (kind == SEK_INT);
      return intVal;
    }

    string asString() const {
      ROSE_ASSERT (kind == SEK_STRING);
      return stringVal;
    }

    SgAsmNode* asNode() const {
      ROSE_ASSERT (kind == SEK_NODE);
      return nodeVal;
    }
  };

  static StackEntry seInt(uint64_t i) {
    StackEntry se;
    se.kind = SEK_INT;
    se.intVal = i;
    se.nodeVal = 0;
    return se;
  }

  static StackEntry seString(const string& s) {
    StackEntry se;
    se.intVal = 0;
    se.kind = SEK_STRING;
    se.stringVal = s;
    se.nodeVal = 0;
    return se;
  }

  static StackEntry seNode(SgAsmNode* n) {
    StackEntry se;
    se.kind = SEK_NODE;
    se.intVal = 0;
    se.nodeVal = n;
    return se;
  }

  static void printSE(FILE* f, const StackEntry& se) {
    switch (se.kind) {
      case SEK_INT: fprintf(f, "int(%lu)", (unsigned long)se.intVal); break;
      case SEK_STRING: fprintf(f, "string(\"%s\")", se.stringVal.c_str()); break;
      case SEK_NODE: fprintf(f, "node(%p)", se.nodeVal); break;
      default: fprintf(f, "invalid_se"); break;
    }
  }

  static void printStack(FILE* f, stack<StackEntry> st) { // Make copy to allow mutation
    fprintf(f, "Stack:");
    while (!st.empty()) {
      fprintf(f, " ");
      printSE(f, st.top());
      st.pop();
    }
    fprintf(f, "\n");
  }

#define HANDLEREG(name, roseName, posName) \
  do { \
    if (regName == name) { \
      reg = SgAsmRegisterReferenceExpression::roseName; \
      pos = SgAsmRegisterReferenceExpression::posName; \
      return; \
    } \
  } while (0);

  static void getX86RegisterInfo(const string& regName,
                                 SgAsmRegisterReferenceExpression::x86_register_enum& reg,
                                 SgAsmRegisterReferenceExpression::x86_position_in_register_enum& pos) {
    HANDLEREG("rax", rAX, qword);
    HANDLEREG("rbx", rBX, qword);
    HANDLEREG("rcx", rCX, qword);
    HANDLEREG("rdx", rDX, qword);
    HANDLEREG("rsi", rSI, qword);
    HANDLEREG("rdi", rDI, qword);
    HANDLEREG("rsp", rSP, qword);
    HANDLEREG("rbp", rBP, qword);
    HANDLEREG("r8",  r8,  qword);
    HANDLEREG("r9",  r9,  qword);
    HANDLEREG("r10", r10, qword);
    HANDLEREG("r11", r11, qword);
    HANDLEREG("r12", r12, qword);
    HANDLEREG("r13", r13, qword);
    HANDLEREG("r14", r14, qword);
    HANDLEREG("r15", r15, qword);
    HANDLEREG("eax",  rAX, dword);
    HANDLEREG("ebx",  rBX, dword);
    HANDLEREG("ecx",  rCX, dword);
    HANDLEREG("edx",  rDX, dword);
    HANDLEREG("esi",  rSI, dword);
    HANDLEREG("edi",  rDI, dword);
    HANDLEREG("esp",  rSP, dword);
    HANDLEREG("ebp",  rBP, dword);
    HANDLEREG("r8d",  r8,  dword);
    HANDLEREG("r9d",  r9,  dword);
    HANDLEREG("r10d", r10, dword);
    HANDLEREG("r11d", r11, dword);
    HANDLEREG("r12d", r12, dword);
    HANDLEREG("r13d", r13, dword);
    HANDLEREG("r14d", r14, dword);
    HANDLEREG("r15d", r15, dword);
    HANDLEREG("ax",   rAX, word);
    HANDLEREG("bx",   rBX, word);
    HANDLEREG("cx",   rCX, word);
    HANDLEREG("dx",   rDX, word);
    HANDLEREG("si",   rSI, word);
    HANDLEREG("di",   rDI, word);
    HANDLEREG("sp",   rSP, word);
    HANDLEREG("bp",   rBP, word);
    HANDLEREG("r8w",  r8,  word);
    HANDLEREG("r9w",  r9,  word);
    HANDLEREG("r10w", r10, word);
    HANDLEREG("r11w", r11, word);
    HANDLEREG("r12w", r12, word);
    HANDLEREG("r13w", r13, word);
    HANDLEREG("r14w", r14, word);
    HANDLEREG("r15w", r15, word);
    HANDLEREG("al",   rAX, low_byte);
    HANDLEREG("bl",   rBX, low_byte);
    HANDLEREG("cl",   rCX, low_byte);
    HANDLEREG("dl",   rDX, low_byte);
    HANDLEREG("sil",  rSI, low_byte);
    HANDLEREG("dil",  rDI, low_byte);
    HANDLEREG("spl",  rSP, low_byte);
    HANDLEREG("bpl",  rBP, low_byte);
    HANDLEREG("r8b",  r8,  low_byte);
    HANDLEREG("r9b",  r9,  low_byte);
    HANDLEREG("r10b", r10, low_byte);
    HANDLEREG("r11b", r11, low_byte);
    HANDLEREG("r12b", r12, low_byte);
    HANDLEREG("r13b", r13, low_byte);
    HANDLEREG("r14b", r14, low_byte);
    HANDLEREG("r15b", r15, low_byte);
    HANDLEREG("ah",   rAX, high_byte);
    HANDLEREG("bh",   rBX, high_byte);
    HANDLEREG("ch",   rCX, high_byte);
    HANDLEREG("dh",   rDX, high_byte);
    HANDLEREG("cs",   CS, word);
    HANDLEREG("ds",   DS, word);
    HANDLEREG("es",   ES, word);
    HANDLEREG("ss",   SS, word);
    HANDLEREG("fs",   FS, word);
    HANDLEREG("gs",   GS, word);
    HANDLEREG("rip",  rIP, qword);
    HANDLEREG("eip",  rIP, dword);
    HANDLEREG("ip",   rIP, word);
    if (regName.size() == 3 && regName.substr(0, 2) == "st" && regName[2] >= '0' && regName[2] <= '7') {
      reg = SgAsmRegisterReferenceExpression::x86_register_enum(SgAsmRegisterReferenceExpression::STnumbered + (regName[2] - '0'));
      pos = SgAsmRegisterReferenceExpression::all;
    } else if (regName.size() == 3 && regName.substr(0, 2) == "mm" && regName[2] >= '0' && regName[2] <= '7') {
      reg = SgAsmRegisterReferenceExpression::x86_register_enum(SgAsmRegisterReferenceExpression::MM + (regName[2] - '0'));
      pos = SgAsmRegisterReferenceExpression::all;
    } else if (regName.size() > 3 && regName.substr(0, 3) == "xmm") {
      reg = SgAsmRegisterReferenceExpression::x86_register_enum(SgAsmRegisterReferenceExpression::XMM + atoi(regName.c_str() + 3));
      pos = SgAsmRegisterReferenceExpression::all;
    } else {
      fprintf(stderr, "Bad register '%s'\n", regName.c_str());
      abort();
    }
  }

#undef HANDLEREG

  static SgAsmType* sizeCommandToType(const string& cmd) {
    if (cmd == "sizeByte") {
      return SgAsmTypeByte::createType();
    } else if (cmd == "sizeWord") {
      return SgAsmTypeWord::createType();
    } else if (cmd == "sizeDWord") {
      return SgAsmTypeDoubleWord::createType();
    } else if (cmd == "sizeQWord") {
      return SgAsmTypeQuadWord::createType();
    } else if (cmd == "sizeFloat") {
      return SgAsmTypeSingleFloat::createType();
    } else if (cmd == "sizeDouble") {
      return SgAsmTypeDoubleFloat::createType();
    } else if (cmd == "sizeLongDouble") {
      return SgAsmTypeDoubleFloat::createType(); // FIXME -- needs a separate type
    } else {
      return NULL;
    }
  }

  static void dispatchCommand(const string& cmd, stack<StackEntry>& st,
                              map<uint64_t, SgAsmInstruction*>& insns, map<uint64_t, SgAsmBlock*>& basicBlocks,
                              map<uint64_t, uint64_t>& nextAddress) {
    // fprintf(stderr, "dispatchCommand '%s' ", cmd.c_str());
    // printStack(stderr, st);
    if (cmd == "reg") {
      string regName = st.top().asString();
      st.pop();
      SgAsmRegisterReferenceExpression* e = new SgAsmRegisterReferenceExpression();
      SgAsmRegisterReferenceExpression::x86_register_enum reg = SgAsmRegisterReferenceExpression::undefined_general_register;
      SgAsmRegisterReferenceExpression::x86_position_in_register_enum pos = SgAsmRegisterReferenceExpression::undefined_position_in_register;
      getX86RegisterInfo(regName, reg, pos);
      e->set_x86_register_code(reg);
      e->set_x86_position_in_register_code(pos);
      st.push(seNode(e));
    } else if (cmd == "constant") {
      int con = st.top().asInt();
      st.pop();
      SgAsmQuadWordValueExpression* e = new SgAsmQuadWordValueExpression();
      e->set_value(con);
      st.push(seNode(e));
    } else if (cmd == "annotate") {
      string annotation = st.top().asString();
      st.pop();
      SgAsmExpression* expr = isSgAsmExpression(st.top().asNode());
      // Note -- don't pop here to keep the AST node on the stack after we change it
      ROSE_ASSERT (expr);
      expr->set_replacement(annotation);
    } else if (cmd == "segment") {
      SgAsmMemoryReferenceExpression* baseAddr = isSgAsmMemoryReferenceExpression(st.top().asNode());
      st.pop();
      SgAsmRegisterReferenceExpression* segReg = isSgAsmRegisterReferenceExpression(st.top().asNode());
      st.pop();
      ROSE_ASSERT (baseAddr);
      ROSE_ASSERT (segReg);
      baseAddr->set_segment(segReg);
      segReg->set_parent(baseAddr);
      st.push(seNode(baseAddr));
    } else if (cmd == "sizeUnknown") {
      // Do nothing
      ROSE_ASSERT (!st.empty());
      SgAsmExpression* e1 = isSgAsmExpression(st.top().asNode());
      ROSE_ASSERT (e1);
      // std::cerr << "Found operand with unknown size: " << e1->class_name() << std::endl;
    } else if (cmd == "sizeByte" || cmd == "sizeWord" || cmd == "sizeDWord" || cmd == "sizeQWord" || cmd == "sizeFloat" || cmd == "sizeDouble" || cmd == "sizeLongDouble") {
      SgAsmExpression* e1 = isSgAsmExpression(st.top().asNode());
      st.pop();
      ROSE_ASSERT (e1);
      if (isSgAsmQuadWordValueExpression(e1)) {
        if (cmd == "sizeByte") {
          SgAsmByteValueExpression* e = new SgAsmByteValueExpression();
          e->set_value(isSgAsmQuadWordValueExpression(e1)->get_value());
          st.push(seNode(e));
        } else if (cmd == "sizeWord") {
          SgAsmWordValueExpression* e = new SgAsmWordValueExpression();
          e->set_value(isSgAsmQuadWordValueExpression(e1)->get_value());
          st.push(seNode(e));
        } else if (cmd == "sizeDWord") {
          SgAsmDoubleWordValueExpression* e = new SgAsmDoubleWordValueExpression();
          e->set_value(isSgAsmQuadWordValueExpression(e1)->get_value());
          st.push(seNode(e));
        } else if (cmd == "sizeQWord") {
          st.push(seNode(e1));
        } else {
          ROSE_ASSERT (!"Bad size cmd");
        }
      } else if (isSgAsmMemoryReferenceExpression(e1)) {
        // Set the type of the node based on the size
        SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(e1);
        SgAsmType* newType = sizeCommandToType(cmd);
        if (newType) mr->set_type(newType);
        st.push(seNode(mr));
      } else if (isSgAsmRegisterReferenceExpression(e1)) {
        // Set the type of the node based on the size
        SgAsmRegisterReferenceExpression* rr = isSgAsmRegisterReferenceExpression(e1);
        SgAsmType* newType = sizeCommandToType(cmd);
        if (newType) rr->set_type(newType);
        st.push(seNode(rr));
      } else {
        st.push(seNode(e1));
      }
    } else if (cmd == "add") {
      SgAsmExpression* e2 = isSgAsmExpression(st.top().asNode());
      st.pop();
      SgAsmExpression* e1 = isSgAsmExpression(st.top().asNode());
      st.pop();
      ROSE_ASSERT (e1); ROSE_ASSERT (e2);
      SgAsmBinaryExpression* e = new SgAsmBinaryAdd();
      e1->set_parent(e);
      e2->set_parent(e);
      e->set_lhs(e1);
      e->set_rhs(e2);
      st.push(seNode(e));
    } else if (cmd == "mul") {
      SgAsmExpression* e2 = isSgAsmExpression(st.top().asNode());
      st.pop();
      SgAsmExpression* e1 = isSgAsmExpression(st.top().asNode());
      st.pop();
      ROSE_ASSERT (e1); ROSE_ASSERT (e2);
      SgAsmBinaryExpression* e = new SgAsmBinaryMultiply();
      e1->set_parent(e);
      e2->set_parent(e);
      e->set_lhs(e1);
      e->set_rhs(e2);
      st.push(seNode(e));
    } else if (cmd == "mem_read") {
      SgAsmExpression* e1 = isSgAsmExpression(st.top().asNode());
      st.pop();
      ROSE_ASSERT (e1);
      SgAsmMemoryReferenceExpression* e = new SgAsmMemoryReferenceExpression();
      e1->set_parent(e);
      e->set_address(e1);
      st.push(seNode(e));
    } else if (cmd == "lock") {
      fprintf(stderr, "Found lock prefix -- ROSE cannot represent this correctly\n");
    } else if (cmd == "repe") {
      fprintf(stderr, "Found repe prefix -- ROSE cannot represent this correctly\n");
    } else if (cmd == "repne") {
      fprintf(stderr, "Found repne prefix -- ROSE cannot represent this correctly\n");
    } else if (cmd == "insn") {
      unsigned int numOperands = st.top().asInt();
      st.pop();
      ROSE_ASSERT (numOperands < 10);
      SgAsmOperandList* operands = new SgAsmOperandList();
      SgAsmExpressionPtrList operandList;
      for (unsigned int i = 0; i < numOperands; ++i) {
        SgAsmExpression* e = isSgAsmExpression(st.top().asNode());
        ROSE_ASSERT (e);
        st.pop();
        operandList.insert(operandList.begin(), e); // Reverse order because we are getting the values from a stack
        e->set_parent(operands);
      }
      operands->set_operands(operandList);
      string mnemonic = st.top().asString();
      st.pop();
      string hexCodes = st.top().asString();
      st.pop();
      string fullCode = st.top().asString();
      st.pop();
      uint64_t next_addr = st.top().asInt();
      st.pop();
      uint64_t address = st.top().asInt();
      st.pop();
   // fprintf(stderr, "Got insn at address 0x%08lX\n", (unsigned long)address);
      SgAsmInstruction* instruction = 0;
      {
#include "instruction_x86.inc"
      }
      ROSE_ASSERT (instruction);
      operands->set_parent(instruction);
      instruction->set_operandList(operands);
      instruction->set_comment(fullCode);
      string rawBytes;
      ROSE_ASSERT (hexCodes.size() % 2 == 0);
      for (size_t i = 0; i < hexCodes.size(); i += 2) {
        char c;
        int numVals = sscanf(hexCodes.c_str() + i, "%2hhx", &c);
        if (numVals != 1) {fprintf(stderr, "Invalid hex digits '%2.2s'\n", hexCodes.c_str() + i); abort();}
        rawBytes.push_back(c);
      }
      instruction->set_raw_bytes(rawBytes);
      ROSE_ASSERT (insns.find(address) == insns.end());
      insns[address] = instruction;
      nextAddress[address] = next_addr;
      ROSE_ASSERT (st.empty());
      // fprintf(stderr, "Got insn 0x%08lX = '%s'\n", (unsigned long)address, fullCode.c_str());
    } else if (cmd == "basic_block_start") {
      uint64_t addr = st.top().asInt();
      st.pop();
      ROSE_ASSERT (basicBlocks.find(addr) == basicBlocks.end());
      //if (!(basicBlocks.find(addr) == basicBlocks.end())) return;
      SgAsmBlock* block = new SgAsmBlock();//SgAsmFunctionDeclaration(addr, RoseBin_support::HexToString(addr));//SgAsmBlock();
      block->set_id(addr);
      block->set_address(addr);
      basicBlocks[addr] = block;
    } else {
      fprintf(stderr, "Invalid command '%s'\n", cmd.c_str());
      abort();
    }
  }

// DQ (1/22/2008): Changed inteface for this function
// SgAsmFile* objdumpToRoseBinaryAst(const string& fileName, SgAsmFile* file)
// JJW 1/30/2008 Added project so we have access to path information
void objdumpToRoseBinaryAst(const string& fileName, SgAsmFile* file, SgProject* proj)
{
    ROSE_ASSERT(file != NULL);
    ROSE_ASSERT(proj != NULL);

    map<uint64_t, SgAsmBlock*> basicBlocks;
    map<uint64_t, SgAsmInstruction*> insns;
    map<uint64_t, uint64_t> nextAddress;
 // FILE* f = popen(("./asmToRoseAst.tcl " + fileName).c_str(), "r");
 // FILE* f = popen(( "/home/panas2/development/ROSE-64bit/NEW_ROSE/src/frontend/ObjdumpDisassembler/asmToRoseAst.tcl " + fileName).c_str(), "r");
    string dirOfScript = findRoseSupportPathFromSource("src/frontend/ObjdumpDisassembler", ROSE_AUTOMAKE_LIBEXECDIR);
    string pathToScript = dirOfScript + "/asmToRoseAst.tcl";
    vector<string> args(2);
    args[0] = pathToScript;
    args[1] = fileName;
    FILE* f = popenReadFromVector(args);
    if (!f) {ROSE_ASSERT(!"Could not open file"); abort();}
    stack<StackEntry> st;
    while (true) {
      errno = 0;
      int c = getc(f);
      if (c == EOF) {
        if (errno != 0) {perror("Error reading from pipe"); abort();}
        // There should have been a "done" command
        cerr << "Error in objdumpToRoseBinaryAst -- spawned process failed" << endl;
        abort();
        break;
      } else if (c == '"') {
        string str;
        while (true) {
          if (feof(f)) {ROSE_ASSERT (!"EOF in middle of string"); abort();}
          c = getc(f);
          if (c == '"') break;
          str.push_back((char)c);
        }
        st.push(seString(str));
      } else if (isspace(c)) {
        continue;
      } else if (isdigit(c)) {
        ungetc(c, f);
        unsigned long val;
        int numvals = fscanf(f, "%lu", &val);
        if (numvals != 1) {ROSE_ASSERT (!"Error reading integer"); abort();}
        st.push(seInt(val));
      } else if (isalpha(c)) {
        ungetc(c, f);
        string str;
        while (true) {
          if (feof(f)) break;
          c = getc(f);
          if (!isalnum(c) && c != '_') break;
          str.push_back((char)c);
        }
        if (str == "done") break;
        dispatchCommand(str, st, insns, basicBlocks, nextAddress);
      } else {
        ROSE_ASSERT (!"Invalid character from pipe");
        abort();
      }
    }
    ROSE_ASSERT (st.empty());
    pcloseFromVector(f);

    // Put each instruction into the proper block, and link the blocks together
 // SgAsmFile* file = new SgAsmFile();
    SgAsmBlock* mainBlock = new SgAsmBlock();
    mainBlock->set_parent(file);
    file->set_global_block(mainBlock);

    map<uint64_t, SgAsmBlock*> insnToBlock;

    // fprintf(stderr, "Putting blocks in file\n");
    for (map<uint64_t, SgAsmBlock*>::const_iterator i = basicBlocks.begin(); i != basicBlocks.end(); ++i) {
      mainBlock->get_statementList().push_back(i->second);
      i->second->set_parent(mainBlock);
      // Every basic block start is in its own block
      insnToBlock[i->first] = i->second;
      // fprintf(stderr, "Found basic block start 0x%08X as %p\n", (unsigned int)(i->first), i->second);
    }

    // fprintf(stderr, "Putting instructions in blocks\n");
    // This stuff relies on the maps being sorted -- hash_maps won't work
    for (map<uint64_t, SgAsmInstruction*>::const_iterator i = insns.begin(); i != insns.end(); ++i) {
      uint64_t addr = i->first;
      SgAsmInstruction* insn = i->second;
      map<uint64_t, SgAsmBlock*>::const_iterator theBlockIter = insnToBlock.find(addr);
      ROSE_ASSERT (theBlockIter != insnToBlock.end());
      SgAsmBlock* theBlock = theBlockIter->second;
      insn->set_parent(theBlock);
      theBlock->get_statementList().push_back(insn);
      // fprintf(stderr, "Put insn 0x%08X into block %p\n", addr, theBlock);
      ROSE_ASSERT (nextAddress.find(addr) != nextAddress.end());
      uint64_t next_addr = nextAddress[addr];
      if (next_addr != 0 && insnToBlock.find(next_addr) == insnToBlock.end()) {
        // Set the value for the next instruction if it does not start its own basic block
        insnToBlock[next_addr] = theBlock;
      }
    }

    // fprintf(stderr, "Done getting instructions\n");
 // return file;
  }

}
