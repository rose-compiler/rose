#include "rose.h"

using namespace std;
using namespace Exec;

ExecSection* DisassemblerCommon::AsmFileWithData::getSectionOfAddress(uint64_t addr) const {
  ExecSection *section = ef->get_section_by_va(addr);
  if (!section) {
    vector<ExecSection*> possibleSections = ef->get_sections_by_va(addr);
    if (possibleSections.empty()) {
      // There is no section to satisfy the request.
      return NULL;
    } else if (possibleSections.size() != 1) {
      // Multiple sections found and they don't map consistently between file and memory.
      cerr << "Trying to disassemble code that is in multiple sections (addr = 0x" << hex << addr << ")" << endl;
      abort();
    }
  }
  return section;
}

bool DisassemblerCommon::AsmFileWithData::inCodeSegment(uint64_t addr) const {
  ExecSection* sectionOfThisPtr = getSectionOfAddress(addr);
  if (sectionOfThisPtr != NULL &&
      sectionOfThisPtr->is_mapped() &&
      sectionOfThisPtr->get_eperm()) {
    return true;
  }
  return false;
}

size_t DisassemblerCommon::AsmFileWithData::getFileOffsetOfAddress(uint64_t addr) const {
  ExecSection* section = getSectionOfAddress(addr);
  if (!section) abort();
  return section->get_va_offset(addr);
}

SgAsmInstruction* DisassemblerCommon::AsmFileWithData::disassembleOneAtAddress(uint64_t addr, set<uint64_t>& knownSuccessors) const {
  ExecSection* section = getSectionOfAddress(addr);
  if (!section) return 0;
  if (!section->is_mapped() ||
      !section->get_eperm()) {
    return 0;
  }
  ExecHeader* header = section->get_header();
  ROSE_ASSERT (header);
  uint64_t rva = addr - header->get_base_va();
  size_t fileOffset = rva - section->get_mapped_rva() + section->get_offset();
  ROSE_ASSERT (fileOffset < ef->get_size());
  const Architecture& arch = header->get_target();
  InsSetArchitecture isa = arch.get_isa();
  SgAsmInstruction* insn = NULL;
  try {
    if ((isa & ISA_FAMILY_MASK) == ISA_IA32_Family) {
      X86Disassembler::Parameters params(addr, x86_insnsize_32);
      insn = X86Disassembler::disassemble(params, ef->content(), ef->get_size(), fileOffset, &knownSuccessors);
    } else if ((isa & ISA_FAMILY_MASK) == ISA_X8664_Family) {
      X86Disassembler::Parameters params(addr, x86_insnsize_64);
      insn = X86Disassembler::disassemble(params, ef->content(), ef->get_size(), fileOffset, &knownSuccessors);
    } else if (isa == ISA_ARM_Family) {
      ArmDisassembler::Parameters params(addr, true);
      insn = ArmDisassembler::disassemble(params, ef->content(), ef->get_size(), fileOffset, &knownSuccessors);
    } else {
      cerr << "Bad architecture to disassemble" << endl;
      abort();
    }
    ROSE_ASSERT (insn);
    return insn;
  } catch (BadInstruction) {
    knownSuccessors.clear();
    return 0;
  } catch (OverflowOfInstructionVector) {
    return 0;
  }
}

void DisassemblerCommon::AsmFileWithData::disassembleRecursively(uint64_t addr, map<uint64_t, SgAsmInstruction*>& insns, map<uint64_t, bool>& basicBlockStarts, set<uint64_t>& functionStarts) const {
  vector<uint64_t> worklist(1, addr);
  disassembleRecursively(worklist, insns, basicBlockStarts, functionStarts);
}

void DisassemblerCommon::AsmFileWithData::disassembleRecursively(vector<uint64_t>& worklist, map<uint64_t, SgAsmInstruction*>& insns, map<uint64_t, bool>& basicBlockStarts, set<uint64_t>& functionStarts) const {
  while (!worklist.empty()) {
    uint64_t addr = worklist.back();
    worklist.pop_back();
    if (insns.find(addr) != insns.end()) continue;
    ++instructionsDisassembled;
    if (instructionsDisassembled % 10000 == 0) {
      cerr << instructionsDisassembled << " disassembling " << addr << " worklist size = " << worklist.size() << ", done = " << insns.size() << endl;
    }
    set<uint64_t> knownSuccessors;
    SgAsmInstruction* insn = disassembleOneAtAddress(addr, knownSuccessors);
    if (!insn) {cerr << "Bad instruction at 0x" << hex << addr << endl; continue;}
    insns.insert(make_pair(addr, insn));
    for (set<uint64_t>::const_iterator i = knownSuccessors.begin(); i != knownSuccessors.end(); ++i) {
      if (!inCodeSegment(*i)) { /* cerr << "Found succ outside code segment at 0x" << hex << *i << endl; */ continue;} // Assume no jumps to data segments
      if (knownSuccessors.size() != 1 || *i != addr + insn->get_raw_bytes().size()) {
        basicBlockStarts[*i] |= false; // Ensure it exists, but don't change its value if it was already true
        // basicBlockStarts[*i] = true; // Be more conservative
      }
      if (insns.find(*i) == insns.end()) {
        worklist.push_back(*i);
      }
    }
    // The return location for a call needs to be externally visible, which
    // it wouldn't be by the other rules
    SgAsmx86Instruction* x86insn = isSgAsmx86Instruction(insn);
    SgAsmArmInstruction* arminsn = isSgAsmArmInstruction(insn);
    if ((x86insn && (x86insn->get_kind() == x86_call || x86insn->get_kind() == x86_farcall || x86InstructionIsUnconditionalBranch(x86insn))) ||
        (arminsn && (arminsn->get_kind() == arm_b || arminsn->get_kind() == arm_bl || arminsn->get_kind() == arm_blx || arminsn->get_kind() == arm_bx || arminsn->get_kind() == arm_bxj))) {
      basicBlockStarts[addr + insn->get_raw_bytes().size()] = true;
    }
    // Scan for constant operands that are code pointers
    SgAsmOperandList* ol = insn->get_operandList();
    const vector<SgAsmExpression*>& operands = ol->get_operands();
    for (size_t i = 0; i < operands.size(); ++i) {
      uint64_t constant = 0;
      switch (operands[i]->variantT()) {
        case V_SgAsmWordValueExpression: constant = isSgAsmWordValueExpression(operands[i])->get_value(); break;
        case V_SgAsmDoubleWordValueExpression: constant = isSgAsmDoubleWordValueExpression(operands[i])->get_value(); break;
        case V_SgAsmQuadWordValueExpression: constant = isSgAsmQuadWordValueExpression(operands[i])->get_value(); break;
        default: continue; // Not an appropriately-sized constant
      }
      if (inCodeSegment(constant)) {
        // The second part of the condition is trying to handle if
        // something pushes the address of the next instruction
        if ((x86insn && (x86insn->get_kind() == x86_call || x86insn->get_kind() == x86_farcall)) ||
            (arminsn && (arminsn->get_kind() == arm_bl || arminsn->get_kind() == arm_blx))) {
          functionStarts.insert(constant);
        }
        basicBlockStarts[constant] = true;
        if (insns.find(constant) == insns.end()) {
          worklist.push_back(constant);
        }
      }
    }
  }
}

void Disassembler::disassembleFile(SgAsmFile* f) {
  ExecFile* ef = Exec::parse(f->get_name().c_str());
  DisassemblerCommon::AsmFileWithData file(ef);
  map<uint64_t, SgAsmInstruction*> insns;
  map<uint64_t, bool> basicBlockStarts;
  set<uint64_t> functionStarts;
  const vector<ExecHeader*>& headers = ef->get_headers();
  for (size_t i = 0; i < headers.size(); ++i) {
    uint64_t entryPoint = headers[i]->get_entry_rva() + headers[i]->get_base_va();
    basicBlockStarts[entryPoint] = true;
    functionStarts.insert(entryPoint);
    file.disassembleRecursively(entryPoint, insns, basicBlockStarts, functionStarts);
  }

  const vector<ExecSection*>& sections = ef->get_sections();
  for (size_t i = 0; i < sections.size(); ++i) {
    ExecSection* sect = sections[i];
    if (sect->is_mapped()) {
      // Scan for pointers to code
      ExecHeader* header = sect->get_header();
      ROSE_ASSERT (header);
      const Architecture& arch = header->get_target();
      InsSetArchitecture isa = arch.get_isa();
      size_t pointerSize = 0;
      if ((isa & ISA_FAMILY_MASK) == ISA_IA32_Family) {
        pointerSize = 4;
      } else if ((isa & ISA_FAMILY_MASK) == ISA_X8664_Family) {
        pointerSize = 8;
      } else if (isa == ISA_ARM_Family) {
        pointerSize = 4;
      } else {
        cerr << "Bad architecture to disassemble" << endl;
        abort();
      }
      ROSE_ASSERT (pointerSize != 0);
      uint64_t endOffset = sect->get_offset() + sect->get_size(); // Size within file
      ROSE_ASSERT (endOffset <= ef->get_size());
      for (uint64_t j = sect->get_offset();
           j + pointerSize <= endOffset;
           j += pointerSize) {
        uint64_t addr = 0;
        // FIXME: assumes file is little endian
        for (size_t k = pointerSize; k > 0; --k) {
          addr <<= 8;
          addr |= ef->content()[j + k - 1];
        }
        addr += header->get_base_va();
        if (file.inCodeSegment(addr)) {
          basicBlockStarts[addr] = true;
          file.disassembleRecursively(addr, insns, basicBlockStarts, functionStarts);
        }
      }
    }
  }
  map<uint64_t, SgAsmBlock*> basicBlocks;
  for (map<uint64_t, bool>::const_iterator i = basicBlockStarts.begin(); i != basicBlockStarts.end(); ++i) {
    uint64_t addr = i->first;
    SgAsmBlock* b = new SgAsmBlock();
    b->set_address(addr);
    b->set_id(addr);
    b->set_externallyVisible(i->second);
    basicBlocks[addr] = b;
  }
  SgAsmBlock* blk = PutInstructionsIntoBasicBlocks::putInstructionsIntoBasicBlocks(basicBlocks, insns);
  // Find the signatures that indicate the beginnings of functions -- they
  // are only considered if they are at the start of a basic block
  const SgAsmStatementPtrList& computedBasicBlocks = blk->get_statementList();
  for (size_t i = 0; i < computedBasicBlocks.size(); ++i) {
    SgAsmBlock* bb = isSgAsmBlock(computedBasicBlocks[i]);
    if (!bb) continue;
    ExecSection* section = file.getSectionOfAddress(bb->get_address());
    if (!section) continue;
    ExecHeader* header = section->get_header();
    ROSE_ASSERT (header);
    const Architecture& arch = header->get_target();
    InsSetArchitecture isa = arch.get_isa();
    bool isFunctionStart = false;
    if ((isa & ISA_FAMILY_MASK) == ISA_IA32_Family) {
      isFunctionStart = X86Disassembler::doesBBStartFunction(bb, false);
    } else if ((isa & ISA_FAMILY_MASK) == ISA_X8664_Family) {
      isFunctionStart = X86Disassembler::doesBBStartFunction(bb, true);
    } else if (isa == ISA_ARM_Family) {
      isFunctionStart = false; // FIXME
    } else {
      cerr << "Bad architecture to disassemble" << endl;
      abort();
    }
    if (isFunctionStart) {
      functionStarts.insert(bb->get_address());
    }
  }
  // (tps - 2Jun08) : commented out for now until we investigate this further... breaking the current function analysis
  // blk = PutInstructionsIntoBasicBlocks::putInstructionsIntoFunctions(blk, functionStarts);
  f->set_global_block(blk);
  blk->set_parent(f);
  blk->set_externallyVisible(true);
}
