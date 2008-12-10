#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdint.h>

using namespace std;


class MapGlobalVariables: public AstSimpleProcessing
   {
     public:
          std::map<SgAsmGenericSymbol::addr_t,SgAsmGenericSymbol*> globalFunctionMap;

       // Store the current statement as we traverse so that we can use it to call nextAsmStatement().
       // We need the address of the next statement to evaluate references to global variables.
          SgAsmStatement*   currentAsmStatement;
          SgAsmInstruction* currentAsmInstruction;
          SgAsmGenericSectionList* asmSectionList;

          MapGlobalVariables();
          void visit(SgNode* n);

   };


MapGlobalVariables::MapGlobalVariables()
// : stringTablePointer(NULL)
   {
     currentAsmStatement   = NULL;
     currentAsmInstruction = NULL;
     asmSectionList        = NULL;
   }

void
MapGlobalVariables::visit(SgNode* n)
   {
#if 0
     printf ("node = %p = %s \n",n,n->class_name().c_str());
#endif

     SgAsmStatement* asmStatement = isSgAsmStatement(n);
     if (asmStatement != NULL)
          currentAsmStatement = asmStatement;

     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
          currentAsmInstruction = asmInstruction;

     SgAsmGenericSectionList* temp_asmSectionList = isSgAsmGenericSectionList(n);
     if (temp_asmSectionList != NULL)
          asmSectionList = temp_asmSectionList;
  // ROSE_ASSERT(asmSectionList == NULL);


  // Build the list of symbols representing global variables and functions.
     SgAsmGenericSymbol* symbol = isSgAsmGenericSymbol(n);
     if (symbol != NULL)
        {
          SgAsmGenericSymbol::addr_t address = symbol->get_value();

          if (symbol->get_type() == SgAsmGenericSymbol::SYM_FUNC)
             {
#if 0
               printf ("Adding entry to globalFunctionMap: address = %p symbol = %p = %s \n",(void*)address,symbol,symbol->get_name()->c_str());
#endif
            // Check and see if this address is already present in the map
               if (globalFunctionMap.find(address) == globalFunctionMap.end())
                  {
                    globalFunctionMap[address] = symbol;
                  }
                 else
                  {
                 // This happens for __libc_start_main and __libc_start_main@@GLIBC_2.2.5 which appear to share the 0x00000000 address.
#if 0
                    printf ("Duplicate function used to represent same address \n");
#endif
                 // ROSE_ASSERT(false);
                  }
             }
        }
   }



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
     SgAsmGenericHeader* header = interp->get_header();
     ROSE_ASSERT (header);

     uint64_t rva = addr - header->get_base_va();

     SgAsmGenericFile* file = header->get_file();
     ROSE_ASSERT (file);

     SgAsmGenericSectionList* sectionList = header->get_sections();
     const SgAsmGenericSectionPtrList& sections = sectionList->get_sections();

     for (size_t i = 0; i < sections.size(); ++i) {
       SgAsmGenericSection* section = sections[i];
       ROSE_ASSERT(section->get_header() == header);
       if (!section->is_mapped() && !isSgAsmDOSFileHeader(header)) continue; // Workaround for bug FIXME
       if (rva < section->get_mapped_rva()) continue;
       if (rva >= section->get_mapped_rva() + section->get_mapped_size())
         continue;
       if (Disassembler::aggressive_mode) {
         // Only allow ELF segments
         ROSE_ASSERT (!"Aggressive mode not supported");
       } else {
         if (!isSgAsmElfSection(section) && !isSgAsmPESection(section) && !isSgAsmDOSFileHeader(header)) continue;

      // printf ("In DisassemblerCommon::AsmFileWithData::getSectionOfAddress(%p): returning section %s \n",(void*)addr,section->get_name().c_str());
         return section;
       }
     }
     return NULL;
   }

bool DisassemblerCommon::AsmFileWithData::inCodeSegment(uint64_t addr) const {
  SgAsmGenericSection* sectionOfThisPtr = getSectionOfAddress(addr);
  if (sectionOfThisPtr != NULL &&
      sectionOfThisPtr->get_mapped_xperm()) {
    return true;
  }
  return false;
}

SgAsmInstruction* DisassemblerCommon::AsmFileWithData::disassembleOneAtAddress(uint64_t addr, set<uint64_t>& knownSuccessors) const {
  SgAsmGenericSection* section = getSectionOfAddress(addr);
  if (!section) return 0;

// Check if this is marked as executable
  if (!section->get_mapped_xperm()) {
    return 0;
  }

// Compute the location of the first instruction in the data saved from the AST binary file format IR.
  ROSE_ASSERT (section->get_header() == interp->get_header());
  SgAsmGenericHeader* header = interp->get_header();
  ROSE_ASSERT (header);
  uint64_t rva = addr - header->get_base_va();
  SgAsmGenericFile* file = isSgAsmGenericFile(header->get_parent()->get_parent());
  ROSE_ASSERT (file);
  size_t fileOffset = rva - section->get_mapped_rva() + section->get_offset();
  ROSE_ASSERT (fileOffset < file->get_orig_size());
  SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
  SgAsmInstruction* insn = NULL;

  try {
    if (isSgAsmDOSFileHeader(header)) { // FIXME
      X86Disassembler::Parameters params(addr, x86_insnsize_16);
      insn = X86Disassembler::disassemble(params, &(file->content()[0]), file->get_orig_size(), fileOffset, &knownSuccessors);
    } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family) {
      X86Disassembler::Parameters params(addr, x86_insnsize_32);
      insn = X86Disassembler::disassemble(params, &(file->content()[0]), file->get_orig_size(), fileOffset, &knownSuccessors);
    } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family) {
      X86Disassembler::Parameters params(addr, x86_insnsize_64);
      insn = X86Disassembler::disassemble(params, &(file->content()[0]), file->get_orig_size(), fileOffset, &knownSuccessors);
    } else if (isa == SgAsmExecutableFileFormat::ISA_ARM_Family) {
      ArmDisassembler::Parameters params(addr, true);
      insn = ArmDisassembler::disassemble(params, &(file->content()[0]), file->get_orig_size(), fileOffset, &knownSuccessors);
    } else if (isa == SgAsmExecutableFileFormat::ISA_PowerPC) {

   // DQ (10/12/2008): Added support for PowerPC.
      PowerpcDisassembler::Parameters params(addr);

   // printf ("Initial file starting address (entry point) = %p  file size = %zu  fileOffset = %zu \n",&(file->content()[0]),file->get_orig_size(),fileOffset);

      insn = PowerpcDisassembler::disassemble(params, &(file->content()[0]), file->get_orig_size(), fileOffset, &knownSuccessors);
#if 0
      printf ("Exit after disassembling the first instruction! \n");
      ROSE_ASSERT(false);
#endif
    } else {

   // DQ (10/12/2008): Output a bit more information when we fail!
      printf ("Error: unsupported instruction set isa = %d = 0x%x \n",isa,isa);

      cerr << "Bad architecture to disassemble (disassembleOneAtAddress)" << endl;
      abort();
    }
    ROSE_ASSERT (insn != NULL);
    return insn;
  } catch (BadInstruction) {
    knownSuccessors.clear();
    return 0;
  } catch (OverflowOfInstructionVector) {
    return 0;
  }
}

void DisassemblerCommon::AsmFileWithData::disassembleRecursively(uint64_t addr, map<uint64_t, SgAsmInstruction*>& insns, map<uint64_t, bool>& basicBlockStarts, 
								 map<uint64_t,std::string>& functionStarts,
								 bool heuristicFunctionDetection) const {
  vector<uint64_t> worklist(1, addr);
  disassembleRecursively(worklist, insns, basicBlockStarts, functionStarts, heuristicFunctionDetection);
}

void DisassemblerCommon::AsmFileWithData::disassembleRecursively(vector<uint64_t>& worklist, map<uint64_t, SgAsmInstruction*>& insns, map<uint64_t, bool>& basicBlockStarts, 
								 map<uint64_t,std::string>& functionStarts, 
								 bool heuristicFunctionDetection) const {
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
	  // tps (6/Oct/2008) : added that this is a heuristic and not from the symbol table
	  if (heuristicFunctionDetection == true) {
	    functionStarts.insert(make_pair(constant,""));
	  }
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
bool Disassembler::heuristicFunctionDetection = false;

void Disassembler::disassembleFile(SgAsmFile* f)
   {
  // This is the entry point into the disassembler code.  This function is the single call made from 
  // file: sageSupport.C, function: SgBinaryFile::buildAST()
  // which generate the AST specific to the instructions.  Before calling this function (disassembleFile())
  // the binary file format has been evaluated and the AST IR nodes for this have been built.

  // Because a binary file can have different sections that have different interpretations 
  // (target different processors, processors modes (32-bit vs. 64-bit), or operating systems),
  // more than one interpretation must be used as context to do the disassembly of the binary 
  // instructions.  For example, PE files have a header at the start that contains code and
  // which assumes the DOS operating system and a 16-bit processor mode; the rest of the 
  // instructions are interpreted with the context of at least 32-bit mode and under the 
  // Windows OS.
     const SgAsmInterpretationPtrList& interps = f->get_interpretations();
     for (size_t i = 0; i < interps.size(); ++i)
        {
          disassembleInterpretation(interps[i]);
        }
   }

void Disassembler::disassembleInterpretation(SgAsmInterpretation* interp) {

  // DQ (8/26/2008): Set the agressive mode in the disassembler based on the SgFile (evaluated from the command line).
     SgAsmFile* asmFile = isSgAsmFile(interp->get_parent());
     ROSE_ASSERT (asmFile);
     SgBinaryFile* fileNode = isSgBinaryFile(asmFile->get_parent());
     ROSE_ASSERT(fileNode != NULL);
     aggressive_mode = fileNode->get_aggressive();

     DisassemblerCommon::AsmFileWithData file(interp);
     map<uint64_t, SgAsmInstruction*> insns;
     map<uint64_t, bool> basicBlockStarts;
     map<uint64_t, std::string> functionStarts;


     SgAsmGenericHeader* header = interp->get_header();
     ROSE_ASSERT (header);

  // Compute the location of the first instruction in the file.
     uint64_t entryPoint = header->get_entry_rva() + header->get_base_va();

     SgAsmDOSFileHeader* DOS_header = isSgAsmDOSFileHeader(header);
     if (DOS_header != NULL)
        {
          SgAsmGenericFile* gf = isSgAsmGenericFile(header->get_parent()->get_parent());
          ROSE_ASSERT (gf);
          const SgAsmGenericSectionPtrList& sections = gf->get_sections();
          for (size_t i = 0; i < sections.size(); ++i)
             {
               if (sections[i]->get_header() == DOS_header)
                  {
                    printf ("DOS section DOS_header->is_mapped()           = %s (will be reset to true) \n",DOS_header->is_mapped() ? "true" : "false");
                    printf ("DOS section DOS_header->get_mapped_size()      = %p (will be reset) \n",(void*)DOS_header->get_mapped_size());

                 // DQ (8/31/2008): I think this is temporary code: Don't we want the disassemble to avoid having side-effects on the binary file format?
                 // RPM (9/8/2008): is_mapped() is read-only, depending on mapped address and size being non-zero */
                 // sections[i]->is_mapped(true);

                 // This details of this formulation appear to be different from different sources 
                 // (web pages), the one that special cases last_page_size == 0 is the best.
                 // sections[i]->set_mapped_size(DOS_header->get_e_total_pages() * 512 + DOS_header->get_e_last_page_size());
                 // sections[i]->set_mapped_size(DOS_header->get_e_total_pages() * 512 - (512 - DOS_header->get_e_last_page_size()));
                    sections[i]->set_mapped_size(DOS_header->get_e_total_pages() * 512 - 512 + (DOS_header->get_e_last_page_size() != 0 ? DOS_header->get_e_last_page_size() : 512));

                 // If this is a DOS section that is part of a PE then the extended DOS section in the PE SgAsmInterpretation 
                 // will have the position of the PE section, this limits the size of the mapped DOS section and prevents 
                 // disassembly of the PE headers as part of the DOS SgAsmInterpretation.  This value is usually 80h, but it 
                 // does not have to be, so we look it up explicitly by finding the SgAsmDOSExtendedHeader.  If the 
                 // SgAsmDOSExtendedHeader does not exist then this is likely a DOS executable and we compute the mapped 
                 // size using the usual DOS formula (using the total_pages and the last_page_size entries in the DOS header).
                    rose_addr_t DOS_section_mapped_size = 0x0;
                    rose_addr_t DOS_header_mapped_size  = 0x0;

                 // Search for the SgAsmDOSExtendedHeader section in the section list (the generic file section 
                 // list may disappear soon, if so we have to find it in the section list in the headers in the 
                 // different interpretations. Note that this is likely the same for the NE and LE formats. Not yet tested 
                 // with NE or LE executable format files.
                    for (size_t j = 0; j < sections.size(); ++j)
                       {
                         SgAsmDOSExtendedHeader* asmDOSExtendedHeader = isSgAsmDOSExtendedHeader(sections[j]);
                         if (asmDOSExtendedHeader != NULL)
                            {
                           // Size of the DOS header and the DOS Extended Header (should be 0x40 is size)
                              DOS_header_mapped_size = DOS_header->get_size() + asmDOSExtendedHeader->get_size();

                           // DQ: I think this is always true.
                              ROSE_ASSERT(DOS_header_mapped_size == 0x40);

                           // Get the location of the PE/NE/LE/LX header (pointed to by the Extended DOS header)
                           // This is actually the file address of the new header but since it is mapped 
                           // with the DOS text segment it is the position in the mapped address space (I think).
                              DOS_section_mapped_size = asmDOSExtendedHeader->get_e_lfanew();

                              printf ("Resetting the size of the DOS section to be %p - %p (minus %p for the size of the DOS and Extended DOS headers) \n",
                                   (void*)DOS_section_mapped_size,(void*)DOS_header_mapped_size,(void*)DOS_header_mapped_size);

                           // Set the header file to be explicitly 40h bytes long (the availiable memory mapped from disk before the nest setsion)
                              sections[i]->set_mapped_size(DOS_section_mapped_size - DOS_header_mapped_size);
                            }
                       }

                    printf ("DOS section DOS_header->get_e_total_pages()    = %d \n",DOS_header->get_e_total_pages());
                    printf ("DOS section DOS_header->get_e_last_page_size() = %d \n",DOS_header->get_e_last_page_size());
                    printf ("DOS section mapped size                        = %p (hex) = %"PRIu64" (decimal) \n",(void*)sections[i]->get_mapped_size(),sections[i]->get_mapped_size());
                  }
             }

          ROSE_ASSERT (DOS_header->get_e_cs() == 0); // Don't support executables >64k
          entryPoint = DOS_header->get_e_ip();
        }

  // printf ("In Disassembler::disassembleInterpretation(): entryPoint = %p \n",(void*)entryPoint);

     basicBlockStarts[entryPoint] = true;

  // tps (6/Oct/08): commented out since it affects the function detection
     if (heuristicFunctionDetection == true)
        {
       // DQ (10/12/2008): The entry point for the program is not a function, so it has no function name?
       // However, if we treat it as a function then perhaps we should give it a name, e.g. "program_entry_point".
          functionStarts.insert(make_pair(entryPoint,""));
        }

  // Build up the binary AST with instruction IR nodes generated by disassembly.
     file.disassembleRecursively(entryPoint, insns, basicBlockStarts, functionStarts, heuristicFunctionDetection);

#if 0
     printf ("Disassembler::disassembleFile(): Looking for pointers that reference executable code (valid sections) \n");

// This is a test that attempts to detect executable code in the sections of the binary
// by looking for pointers to existing executable sections.
  const vector<SgAsmGenericSection*> & sections = interp->get_header()->get_sections()->get_sections();
  for (size_t i = 0; i < sections.size(); ++i) {
    SgAsmGenericSection* sect = sections[i];
    if (sect->is_mapped()) {
      // Scan for pointers to code
      SgAsmGenericHeader* header = sect->get_header();
      ROSE_ASSERT (header);
      SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
      size_t pointerSize = 0;
      if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family) {
        pointerSize = 4;
      } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family) {
        pointerSize = 8;
      } else if (isa == SgAsmExecutableFileFormat::ISA_ARM_Family) {
        pointerSize = 4;
      } else {
        cerr << "Bad architecture to disassemble (aggressive)" << endl;
        abort();
      }
      ROSE_ASSERT (pointerSize != 0);
      uint64_t endOffset = sect->get_size(); // Size within section

      for (uint64_t j = 0;
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
          addr |= *sect->content(j + k - 1, 1);
        }

        addr += header->get_base_va();
        if (file.inCodeSegment(addr)) {
          basicBlockStarts[addr] = true;

          printf ("Disassembler::disassembleFile(): SgAsmGenericSection list[%zu]: addr = %p \n",i,(void*)addr);

          file.disassembleRecursively(addr, insns, basicBlockStarts, functionStarts,  heuristicFunctionDetection);
        }
      }
    }
  }
#else
  // printf ("Warning (conservative disassembly): Skipping search for pointers that reference executable code (valid sections) \n");
#endif

  // DQ (10/12/2008): I think that the code below interprets the instructions just generated to distinguish (and build) functions from blocks.
     map<uint64_t, SgAsmBlock*> basicBlocks;
     for (map<uint64_t, bool>::const_iterator i = basicBlockStarts.begin(); i != basicBlockStarts.end(); ++i)
        {
          uint64_t addr = i->first;
          SgAsmBlock* b = new SgAsmBlock();
          b->set_address(addr);
          b->set_id(addr);
          b->set_externallyVisible(i->second);
          basicBlocks[addr] = b;
        }

     MapGlobalVariables t;
     t.traverse(interp, preorder);
  // printf ("Number of global functions = %zu \n",t.globalFunctionMap.size());
     std::string funcName="none";

     SgAsmBlock* blk = PutInstructionsIntoBasicBlocks::putInstructionsIntoBasicBlocks(basicBlocks, insns);
  // Find the signatures that indicate the beginnings of functions -- they
  // are only considered if they are at the start of a basic block
     const SgAsmStatementPtrList& computedBasicBlocks = blk->get_statementList();

  // printf ("computedBasicBlocks.size() = %zu \n",computedBasicBlocks.size());
     for (size_t i = 0; i < computedBasicBlocks.size(); ++i)
        {
       // DQ (10/14/2008): Is this the sort of coding style that we want to have in a for loop?

          SgAsmBlock* bb = isSgAsmBlock(computedBasicBlocks[i]);
          if (!bb) continue;

          SgAsmGenericSection* section = file.getSectionOfAddress(bb->get_address());
          if (!section) continue;

       // These conditionals are all loop invariant
          ROSE_ASSERT (section->get_header() == header);
          SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
          bool isFunctionStart = false;

          if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family)
             {
            // if (!heuristicFunctionDetection)
               if (heuristicFunctionDetection == false)
                  {
                    if (t.globalFunctionMap.find(bb->get_address()) != t.globalFunctionMap.end())
                       {
                         isFunctionStart = true;
                         funcName = t.globalFunctionMap[bb->get_address()]->get_name()->get_string();
                       }
                      else
                         isFunctionStart = false;
                  }
                 else
                    isFunctionStart = X86Disassembler::doesBBStartFunction(bb, false);
             }
            else
             {
               if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family)
                  {
                 // if (!heuristicFunctionDetection)
                    if (heuristicFunctionDetection == false)
                       {
                         if (t.globalFunctionMap.find(bb->get_address()) != t.globalFunctionMap.end())
                            {
                              isFunctionStart = true;
                              funcName = t.globalFunctionMap[bb->get_address()]->get_name()->get_string();
                            }
                           else
                            {
                              isFunctionStart = false;
                            }
                       }
                      else
                       {
                         isFunctionStart = X86Disassembler::doesBBStartFunction(bb, true);
                       }
                  }
                 else
                  {
                   if (isa == SgAsmExecutableFileFormat::ISA_ARM_Family)
                      {
                        isFunctionStart = false; // FIXME
                      }
                     else
                      {
                        if (isSgAsmDOSFileHeader(header))
                           {
                             isFunctionStart = false; // FIXME
                           }
                          else
                           {
                             if (isa == SgAsmExecutableFileFormat::ISA_PowerPC)
                                {
                               // DQ (10/14/2008): Added support for PowerPC in location after addition of function symbols.

                                  if (heuristicFunctionDetection == false)
                                     {
                                       if (t.globalFunctionMap.find(bb->get_address()) != t.globalFunctionMap.end())
                                          {
                                            isFunctionStart = true; 
                                            funcName = t.globalFunctionMap[bb->get_address()]->get_name()->get_string();
                                          }
                                         else
                                          {
                                            isFunctionStart = false;
                                          }
                                     }
                                    else
                                     {
                                       isFunctionStart = PowerpcDisassembler::doesBBStartFunction(bb, true);
                                     }

#if 0
                                  printf ("Exit after disassembling the first instruction after reading function symbols! \n");
                                  ROSE_ASSERT(false);
#endif
                                }
                               else
                                {
                                  cerr << "Bad architecture to disassemble (interpreting functions)" << endl;
                                  abort();
                                }
                           }
                      }
                  }
             }

          if (isFunctionStart)
             {
               functionStarts.insert(make_pair(bb->get_address(),funcName));
             }

       // printf ("Processed a block! computedBasicBlocks[%zu] = %p \n",i,computedBasicBlocks[i]);
        }

  // DQ (10/16/2008): This outputs a hexidecimal number at times!
  // (tps - 2Jun08) : commented out for now until we investigate this further... breaking the current function analysis
  // cerr << ">> Number of FunctionStarts: " << functionStarts.size() << endl;

  // printf ("Number of FunctionStarts: %zu \n",functionStarts.size());

     blk = PutInstructionsIntoBasicBlocks::putInstructionsIntoFunctions(blk, functionStarts);
     interp->set_global_block(blk);
     blk->set_parent(interp);
     blk->set_externallyVisible(true);
   }
