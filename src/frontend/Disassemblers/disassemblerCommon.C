#include "rose.h"

using namespace std;

// DQ (8/21/2008): No longer used (using new IR nodes now)
// using namespace Exec;

#if 0
// Previous version of code
SgAsmGenericSection* DisassemblerCommon::AsmFileWithData::getSectionOfAddress(uint64_t addr) const {
  SgAsmGenericSection *section = ef->get_section_by_va(addr);
  if (!section) {
    vector<SgAsmGenericSection*> possibleSections = ef->get_sections_by_va(addr);
    vector<SgAsmGenericSection*> possibleSections2;
    for (size_t i = 0; i < possibleSections.size(); ++i) {
      if (possibleSections[i]->get_id() != -1) {
        possibleSections2.push_back(possibleSections[i]);
      }
    }
    if (possibleSections2.empty()) {
      // There is no section to satisfy the request.
      return NULL;
    } else if (possibleSections2.size() != 1) {
      // Multiple sections found and they don't map consistently between file and memory.
      cerr << "Trying to disassemble code that is in multiple sections (addr = 0x" << hex << addr << ")" << endl;
      abort();
    } else {
      return possibleSections2[0];
    }
  }
  return section;
}
#endif

SgAsmGenericSection*
DisassemblerCommon::AsmFileWithData::getSectionOfAddress(uint64_t addr) const
   {
  // This version is about the same as the code above, but it calls a new function 
  // "get_best_possible_section_by_va()" implemented in to be more conservative about
  // the selection of segments associated with addresses.
     SgAsmGenericSection *section = NULL;

#if 0
  // Restore original behavior (default is false)
     Disassembler::aggressive_mode = true;
#endif

  // printf ("Disassembler::aggressive_mode = %s \n",Disassembler::aggressive_mode ? "true" : "false");
     ROSE_ASSERT(Disassembler::aggressive_mode == false);
     ROSE_ASSERT(ef != NULL);

  // Step 1: Call the function that Robb wrote
  // section = ef->get_section_by_va(addr);
     section = ef->get_best_possible_section_by_va(addr);

#if 0
  // ROSE_ASSERT(section != NULL);
     if (section == NULL)
        {
          printf ("Address %p generated a UNDEFINED section \n",(void*)addr);
        }
       else
        {
          printf ("Address %p generated a VALID section \n",(void*)addr);
        }
#endif

  // Look in the section table (because not all sections are supposed to contain code to be disassembled).
     if (Disassembler::aggressive_mode == true)
        {
          printf ("Testing: exit if this is called (agressive mode is not yet available)! \n");
          ROSE_ASSERT(false);

       // Look in the segment table (because the OS marks pages of the mapped file based on the segment permissions).
          section = ef->get_section_by_va(addr);
          if (section == NULL)
             {
               vector<SgAsmGenericSection*> possibleSections = ef->get_sections_by_va(addr);
               vector<SgAsmGenericSection*> possibleSections2;
               for (size_t i = 0; i < possibleSections.size(); ++i)
                  {
                    if (possibleSections[i]->get_id() != -1)
                       {
                         possibleSections2.push_back(possibleSections[i]);
                       }
                  }

               if (possibleSections2.empty())
                  {
                  // There is no section to satisfy the request.
                     return NULL;
                  }
                 else
                  {
                     if (possibleSections2.size() != 1)
                        {
                       // Multiple sections found and they don't map consistently between file and memory.
                          cerr << "Trying to disassemble code that is in multiple sections (addr = 0x" << hex << addr << ")" << endl;
                          abort();
                        }
                       else
                        {
                          return possibleSections2[0];
                        }
                  }
             }
        }

     return section;
   }

bool DisassemblerCommon::AsmFileWithData::inCodeSegment(uint64_t addr) const {
  SgAsmGenericSection* sectionOfThisPtr = getSectionOfAddress(addr);
  if (sectionOfThisPtr != NULL &&
      sectionOfThisPtr->is_mapped() &&
      sectionOfThisPtr->get_eperm()) {
    return true;
  }
  return false;
}

size_t DisassemblerCommon::AsmFileWithData::getFileOffsetOfAddress(uint64_t addr) const {
  SgAsmGenericSection* section = getSectionOfAddress(addr);
  if (!section) abort();
  return section->get_va_offset(addr);
}

SgAsmInstruction* DisassemblerCommon::AsmFileWithData::disassembleOneAtAddress(uint64_t addr, set<uint64_t>& knownSuccessors) const {
  SgAsmGenericSection* section = getSectionOfAddress(addr);
  if (!section) return 0;
  if (!section->is_mapped() ||
      !section->get_eperm()) {
    return 0;
  }
  SgAsmGenericHeader* header = section->get_header();
  ROSE_ASSERT (header);
  uint64_t rva = addr - header->get_base_va();
  size_t fileOffset = rva - section->get_mapped_rva() + section->get_offset();
  ROSE_ASSERT (fileOffset < ef->get_size());
  const SgAsmGenericArchitecture* arch = header->get_target();
  SgAsmExecutableFileFormat::InsSetArchitecture isa = arch->get_isa();
  SgAsmInstruction* insn = NULL;
  try {
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family) {
      X86Disassembler::Parameters params(addr, x86_insnsize_32);
      insn = X86Disassembler::disassemble(params, &(ef->content()[0]), ef->get_size(), fileOffset, &knownSuccessors);
    } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family) {
      X86Disassembler::Parameters params(addr, x86_insnsize_64);
      insn = X86Disassembler::disassemble(params, &(ef->content()[0]), ef->get_size(), fileOffset, &knownSuccessors);
    } else if (isa == SgAsmExecutableFileFormat::ISA_ARM_Family) {
      ArmDisassembler::Parameters params(addr, true);
      insn = ArmDisassembler::disassemble(params, &(ef->content()[0]), ef->get_size(), fileOffset, &knownSuccessors);
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

// DQ (8/26/2008): Added initialization for default mode of disassembler
bool Disassembler::aggressive_mode = false;

void Disassembler::disassembleFile(SgAsmFile* f) {

  // DQ (8/26/2008): Set the agressive mode in the disassembler basedon the SgFile (evaluated from the command line).
     SgFile* fileNode = isSgFile(f->get_parent());
     ROSE_ASSERT(fileNode != NULL);
     aggressive_mode = fileNode->get_aggressive();

  SgAsmGenericFile* ef = SgAsmExecutableFileFormat::parse(f->get_name().c_str());
  DisassemblerCommon::AsmFileWithData file(ef);
  map<uint64_t, SgAsmInstruction*> insns;
  map<uint64_t, bool> basicBlockStarts;
  set<uint64_t> functionStarts;

// DQ (8/23/2008): Since headers are also in the section list, isn't this redundant?
  const vector<SgAsmGenericHeader*> & headers = ef->get_headers()->get_headers();
  for (size_t i = 0; i < headers.size(); ++i) {
    uint64_t entryPoint = headers[i]->get_entry_rva() + headers[i]->get_base_va();

 // DQ: This appears to be the "_start" for the program, at least for ELF (unclear what else it can be).
    printf ("Disassembler::disassembleFile(): SgAsmGenericHeader list[%zu] = %p = %s = %s: entryPoint = %p \n",i,headers[i],headers[i]->get_name().c_str(),headers[i]->class_name().c_str(),(void*)entryPoint);

    SgAsmDOSFileHeader* DOS_header = isSgAsmDOSFileHeader(headers[i]);
    if (DOS_header != NULL)
       {
         ROSE_ASSERT( headers[i]->get_name() == "DOS File Header" );
         printf ("Special handling for the DOS File Header in the disassembly \n");

      // There is an additional offset for DOS (and we think is is an offset from the entry point computed using the sections data).
         entryPoint += (DOS_header->get_e_header_paragraphs() * 16L) + DOS_header->get_e_ip();
         if (entryPoint != 0x40)
            {
           // This is the entry point that we are expecting.
              printf ("Non standard DOS entry point selected (or we don't know yet how to compute it for the DOES header) \n");
              ROSE_ASSERT(false);
            }

         printf ("DOS_header->get_e_total_pages()    = %u \n",DOS_header->get_e_total_pages());
         printf ("DOS_header->get_e_last_page_size() = %u \n",DOS_header->get_e_last_page_size());

         unsigned long extra_data_start = DOS_header->get_e_total_pages() * 512L;
         if (DOS_header->get_e_last_page_size())
              extra_data_start -= (512 - DOS_header->get_e_last_page_size());

         printf ("extra_data_start = %lu \n",extra_data_start);

      // entryPoint += 0x40;

         printf ("Using entryPoint = %lu \n",entryPoint);
#if 0
         basicBlockStarts[entryPoint] = true;
         functionStarts.insert(entryPoint);
         file.disassembleRecursively(entryPoint, insns, basicBlockStarts, functionStarts);
#else
         printf ("Skipping the DOS header \n");
#endif
       }
      else
       {
#if 1
         basicBlockStarts[entryPoint] = true;
         functionStarts.insert(entryPoint);
         file.disassembleRecursively(entryPoint, insns, basicBlockStarts, functionStarts);
#else
         printf ("Skipping the PE header \n");
#endif
       }
  }

#if 0
     printf ("Disassembler::disassembleFile(): Looking for pointers that reference executable code (valid sections) \n");

// This is a test that attempts to detect executable code in the sections of the binary
// by looking for pointers to existing executable sections.
  const vector<SgAsmGenericSection*> & sections = ef->get_sections()->get_sections();
  for (size_t i = 0; i < sections.size(); ++i) {
    SgAsmGenericSection* sect = sections[i];
    if (sect->is_mapped()) {
      // Scan for pointers to code
      SgAsmGenericHeader* header = sect->get_header();
      ROSE_ASSERT (header);
      const SgAsmGenericArchitecture * arch = header->get_target();
      SgAsmExecutableFileFormat::InsSetArchitecture isa = arch->get_isa();
      size_t pointerSize = 0;
      if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family) {
        pointerSize = 4;
      } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family) {
        pointerSize = 8;
      } else if (isa == SgAsmExecutableFileFormat::ISA_ARM_Family) {
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

     // This code packs sequences of bytes starting on aligned boundaries together to see 
     // if they generate addresses that then map to an executable section.  This is used as
     // a way to identify hidden parts of the executable that may be instructions.
     // FIXME: assumes file is little endian
        for (size_t k = pointerSize; k > 0; --k) {
          addr <<= 8;

       // This could be a perfomance problem depending upon the implementation of the "content()" function using STL.
          addr |= ef->content()[j + k - 1];
        }

        addr += header->get_base_va();
        if (file.inCodeSegment(addr)) {
          basicBlockStarts[addr] = true;

          printf ("Disassembler::disassembleFile(): SgAsmGenericSection list[%zu]: addr = %p \n",i,(void*)addr);

          file.disassembleRecursively(addr, insns, basicBlockStarts, functionStarts);
        }
      }
    }
  }
#else
     printf ("Warning (conservative disassembly): Skipping search for pointers that reference executable code (valid sections) \n");
#endif

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
    SgAsmGenericSection* section = file.getSectionOfAddress(bb->get_address());
    if (!section) continue;
    SgAsmGenericHeader* header = section->get_header();
    ROSE_ASSERT (header);
    const SgAsmGenericArchitecture* arch = header->get_target();
    SgAsmExecutableFileFormat::InsSetArchitecture isa = arch->get_isa();
    bool isFunctionStart = false;
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family) {
      isFunctionStart = X86Disassembler::doesBBStartFunction(bb, false);
    } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family) {
      isFunctionStart = X86Disassembler::doesBBStartFunction(bb, true);
    } else if (isa == SgAsmExecutableFileFormat::ISA_ARM_Family) {
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
