#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdint.h>

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
     ROSE_ASSERT (interp != NULL);
     SgAsmGenericHeader* header = interp->get_header();
     ROSE_ASSERT (header != NULL);

     uint64_t rva = addr - header->get_base_va();

  // printf ("In getSectionOfAddress(): addr = 0x%"PRIx64" rva = 0x%"PRIx64"\n",addr,rva);

     SgAsmGenericFile* file = header->get_file();
     ROSE_ASSERT (file);

     SgAsmGenericSectionList* sectionList        = header->get_sections();
     const SgAsmGenericSectionPtrList & sections = sectionList->get_sections();

     for (size_t i = 0; i < sections.size(); ++i)
        {
          SgAsmGenericSection* section = sections[i];
          ROSE_ASSERT(section->get_header() == header);

       // DQ (2/5/2009): I think this would be an error for a section we explicitly marked as contining code to disassemble.
       // if (!section->is_mapped() && !isSgAsmDOSFileHeader(header))
          if (section->get_contains_code() == false && section->is_mapped() == false && !isSgAsmDOSFileHeader(header))
             {
            // printf ("note: section->is_mapped() = %s (continue) \n",section->is_mapped() ? "true" : "false");
               continue; // Workaround for bug FIXME
             }

       // if (rva < section->get_mapped_rva()) continue;
          if (section->get_contains_code() == true)
             {
               if (rva < section->get_rose_mapped_rva())
                    continue;
             }
            else
             {
               if (rva < section->get_mapped_rva())
                    continue;
             }

       // if (rva >= section->get_mapped_rva() + section->get_mapped_size()) continue;
          if (section->get_contains_code() == true)
             {
            // Error checking (mapped_size == 0 if the section is not mapped).
            // if (section->get_mapped_size() == 0)
            //      printf ("In DisassemblerCommon::AsmFileWithData::getSectionOfAddress(%p): section->get_mapped_size() == 0 section name %s \n",(void*)addr,section->get_name()->c_str());
            // ROSE_ASSERT(section->get_mapped_size() > 0);
               ROSE_ASSERT(section->get_size() > 0);

               if (rva >= section->get_rose_mapped_rva() + section->get_size())
                    continue;
             }
            else
             {
               if (rva >= section->get_mapped_rva() + section->get_mapped_size())
                    continue;
             }

       // We want to support two or more modes, but this is unfinished.
          if (Disassembler::aggressive_mode)
             {
            // Only allow ELF segments
               ROSE_ASSERT (!"Aggressive mode not supported");
             }
            else
             {
               if (!isSgAsmElfSection(section) && !isSgAsmPESection(section) && !isSgAsmDOSFileHeader(header)) continue;

            // printf ("In DisassemblerCommon::AsmFileWithData::getSectionOfAddress(%p): returning section %s \n",(void*)addr,section->get_name()->c_str());
               return section;
             }
        }

     return NULL;
   }

bool
DisassemblerCommon::AsmFileWithData::inCodeSegment(uint64_t addr) const
   {
     SgAsmGenericSection* sectionOfThisPtr = getSectionOfAddress(addr);

  // DQ (2/4/2009): Change this to check the get_contains_code() boolean function (flag)
  // to see if this is a section that should be disassembled even if it was not marked 
  // as executable (such as in object files).
  // if (sectionOfThisPtr != NULL && sectionOfThisPtr->get_mapped_xperm())
     if (sectionOfThisPtr != NULL && (sectionOfThisPtr->get_mapped_xperm() == true || sectionOfThisPtr->get_contains_code() == true) )
        {
          return true;
        }

     return false;
   }

SgAsmInstruction*
DisassemblerCommon::AsmFileWithData::disassembleOneAtAddress(uint64_t addr, set<uint64_t>& knownSuccessors) const
   {
     SgAsmGenericSection* section = getSectionOfAddress(addr);

  // Trap first possible case of error (addr was not in any section).
     if (section == NULL)
        {
#if 1
          printf ("getSectionOfAddress(addr = 0x%"PRIx64") returned NULL \n",addr);
#endif
          return NULL;
        }

  // DQ (2/4/2009): Change this to check the get_contains_code() boolean function (flag)
  // to see if this is a section that should be disassembled even if it was not marked 
  // as executable (such as in object files).

  // Check if this is marked as executable, if not then was it marked to have code anyway (e.g. object file, if so then it will be disassembled)
  // if (!section->get_mapped_xperm())
     if (section->get_mapped_xperm() == false && section->get_contains_code() == false)
        {
          printf ("In DisassemblerCommon::AsmFileWithData::disassembleOneAtAddress(), but section->get_mapped_xperm() == false && section->get_contains_code() == false \n");
          return NULL;
        }

  // Compute the location of the first instruction in the data saved from the AST binary file format IR.
     ROSE_ASSERT (section->get_header() == interp->get_header());
     SgAsmGenericHeader* header = interp->get_header();
     ROSE_ASSERT (header);
     uint64_t rva = addr - header->get_base_va();
     SgAsmGenericFile* file = isSgAsmGenericFile(header->get_parent()->get_parent());
     ROSE_ASSERT (file);
#if 0
  // Original code.
     size_t fileOffset = rva - section->get_mapped_rva() + section->get_offset();
     ROSE_ASSERT (fileOffset < file->get_orig_size());
#else
  // DQ (2/5/2009): This is the modified version to handle sections from object file that are not marked 
  // executable and for which we have introduced a mechansim to explicitly mark them as containing code 
  // (marking is done via a separate analysis usingteh symbols in the object file).
     size_t fileOffset = 0;
     if (section->get_contains_code() == true)
        {
       // This is a section in an object file that is explicitly marked as containing code.
       // printf ("In getSectionOfAddress() addr = 0x%"PRIx64" section->get_rose_mapped_rva() = 0x%"PRIx64" section->get_offset() = %zu \n",addr,section->get_rose_mapped_rva(),section->get_offset());
          fileOffset = rva - section->get_rose_mapped_rva() + section->get_offset();
          ROSE_ASSERT (fileOffset < file->get_orig_size());
        }
       else
        {
          fileOffset = rva - section->get_mapped_rva() + section->get_offset();
          ROSE_ASSERT (fileOffset < file->get_orig_size());
        }
#endif

     SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
     SgAsmInstruction* insn = NULL;

     try{
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
          } else {

            // DQ (10/12/2008): Output a bit more information when we fail!
               printf ("Error: unsupported instruction set isa = %d = 0x%x \n",isa,isa);

               cerr << "Bad architecture to disassemble (disassembleOneAtAddress)" << endl;
               abort();
          }
          ROSE_ASSERT (insn != NULL);
          return insn;
        }

     catch (BadInstruction)
        {
          knownSuccessors.clear();
          return NULL;
        } 
     catch (OverflowOfInstructionVector)
        {
          return NULL;
        }
   }

void
DisassemblerCommon::AsmFileWithData::disassembleRecursively(uint64_t addr,
                                                            map<uint64_t, SgAsmInstruction*>& insns,
                                                            BasicBlockStarts &basicBlockStarts
                                                            ) const
{
  vector<uint64_t> worklist(1, addr);
  disassembleRecursively(worklist, insns, basicBlockStarts);
}

/** Disassemble instructions reachable from the addresses in @p worklist, adding them to @p insns mapped by virtual address.
 *  The @p basicBlockStarts will indicate whether each instruction address is the beginning of a basic block. */
void
DisassemblerCommon::AsmFileWithData::disassembleRecursively(vector<uint64_t>& worklist,
                                                            map<uint64_t, SgAsmInstruction*>& insns,
                                                            BasicBlockStarts &basicBlockStarts
                                                            ) const
{
    while (!worklist.empty()) {
        uint64_t addr = worklist.back();
        worklist.pop_back();

        // Check if this instruction has already been decoded.
        if (insns.find(addr) != insns.end())
            continue;
        
        ++instructionsDisassembled;

        if (instructionsDisassembled % 10000 == 0) {
            cerr << instructionsDisassembled << " disassembling " << addr
                 << " worklist size = " << worklist.size()
                 << ", done = " << insns.size() << endl;
        }

        /* Disassemble an instruction, returning all known successor addresses of this instruction. */
        set<uint64_t> knownSuccessors;
        SgAsmInstruction* insn = disassembleOneAtAddress(addr, knownSuccessors);

        // Check if there was an error, NULL return value means that the instruction was not decoded.
        // This is likely because it was an illegal instruction (data instead of code).
        if (insn == NULL) {
            cerr << "Bad instruction at 0x" << hex << addr << endl; 
            continue;
        }

        insns.insert(make_pair(addr, insn));

        /* Build branching-graph based on the known successor addresses. Non-branching instructions will return at most one
         * successor which we do not add to the graph because we want edges between the basic blocks, not within a basic
         * block. Unconditional branches will also return at most one successor, but the successor will probably not be the
         * next address but rather some distant address which we store as an edge in the graph.  Instructions that have more
         * than one successor are such things as conditional branches or CALL-like instructions where one of the successors is
         * probably the next address--we store all of these successor edges (the distant addresses obviously need to be stored,
         * but the next address is also stored because it's an edge to another basic block. */
        for (set<uint64_t>::const_iterator i = knownSuccessors.begin(); i != knownSuccessors.end(); ++i) {
            rose_addr_t work_addr = *i;
            if (!inCodeSegment(work_addr))
                continue;
            if (knownSuccessors.size()>1 || work_addr != addr + insn->get_raw_bytes().size())
                basicBlockStarts[work_addr].insert(addr);
            if (insns.find(work_addr) == insns.end())
                worklist.push_back(work_addr);
        }

        /* Scan for constant operands that are code pointers. Such operands are often used in a closely following instruction
         * as a jump target. E.g., "move 0x400600, reg1; ...; jump reg1". We don't know when (or even if) the execution branch
         * occurs, but for the sake of inter basic block branching we'll just say it happes at this instruction. (In practice,
         * it almost always happens at the end of this instruction's basic block.) */
        SgAsmOperandList* ol = insn->get_operandList();
        const vector<SgAsmExpression*>& operands = ol->get_operands();
        for (size_t i = 0; i < operands.size(); ++i) {
            uint64_t constant = 0;
            switch (operands[i]->variantT()) {
              case V_SgAsmWordValueExpression:
                constant = isSgAsmWordValueExpression(operands[i])->get_value();
                break;
              case V_SgAsmDoubleWordValueExpression:
                constant = isSgAsmDoubleWordValueExpression(operands[i])->get_value();
                break;
              case V_SgAsmQuadWordValueExpression:
                constant = isSgAsmQuadWordValueExpression(operands[i])->get_value();
                break;
              default:
                 continue; // Not an appropriately-sized constant
            }
            if (inCodeSegment(constant)) {
                basicBlockStarts[constant].insert(addr);
                if (insns.find(constant) == insns.end()) {
                    worklist.push_back(constant);
                }
            }
        }
    }
}

// DQ (8/26/2008): Added initialization for default mode of disassembler
bool Disassembler::aggressive_mode            = false;
bool Disassembler::heuristicFunctionDetection = false;

void
Disassembler::disassembleFile(SgAsmFile* f)
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
       // printf ("In Calling Disassembler::disassembleFile(): disassembler for SgAsmInterpretation = %p \n",interps[i]);
          disassembleInterpretation(interps[i]);
        }
   }

void
Disassembler::disassembleInterpretation(SgAsmInterpretation* interp)
   {
  // DQ (8/26/2008): Set the agressive mode in the disassembler based on the SgFile (evaluated from the command line).
     SgAsmFile* asmFile = isSgAsmFile(interp->get_parent());
     ROSE_ASSERT (asmFile);
     SgBinaryFile* fileNode = isSgBinaryFile(asmFile->get_parent());
     ROSE_ASSERT(fileNode != NULL);

  // Evaluate what mode of analysis is requested from the command line.
     aggressive_mode = fileNode->get_aggressive();

     DisassemblerCommon::AsmFileWithData  file(interp);
     map<uint64_t, SgAsmInstruction*>     insns;
     DisassemblerCommon::BasicBlockStarts basicBlockStarts;
     DisassemblerCommon::FunctionStarts   functionStarts;

     SgAsmGenericHeader* header = interp->get_header();
     ROSE_ASSERT (header != NULL);

  /* Seed the disassembly with the entry point(s) stored in the file header. */
     SgRVAList entry_rvalist = header->get_entry_rvas();

  // printf ("entry_rvalist.size() = %zu \n",entry_rvalist.size());
     for (size_t i = 0; i < entry_rvalist.size(); i++)
        {
       // DQ (2/5/2009): ELF files only have a single entry in the RVA list.  For object files we have added 
       // the start of each section that contains code (as an address) to the header->get_entry_rvas(). So 
       // that we can traverse all the sections that have code and disassemble those sections.
          uint64_t entryPoint = entry_rvalist[i].get_rva() + header->get_base_va();
#if 0
          printf ("entry_rvalist[i].get_rva()             = 0x%"PRIx64" \n",entry_rvalist[i].get_rva());
          printf ("header->get_base_va()                  = 0x%"PRIx64" \n",header->get_base_va());
          printf ("Computed entryPoint                    = 0x%"PRIx64" \n",entryPoint);
#endif
       // DQ (2/5/2009): Added comment: If the entryPoint is not in the list of block statrts then add it as a mapped_type...
          if (basicBlockStarts.find(entryPoint) == basicBlockStarts.end())
             {
            // printf ("Adding this entryPoint = %zu to the basicBlockStarts list \n",entryPoint);
               basicBlockStarts[entryPoint] = DisassemblerCommon::BasicBlockStarts::mapped_type();
             }

       // printf ("In Disassembler::disassembleInterpretation(): calling disassembleRecursively() for entry_rvalist[%zu] \n",i);
          file.disassembleRecursively(entryPoint, insns, basicBlockStarts);
       // printf ("DONE: In Disassembler::disassembleInterpretation(): calling disassembleRecursively() for entry_rvalist[%zu] \n",i);
        }

#if 0
    // This is a test that attempts to detect executable code in the sections of the binary
    // by looking for pointers to existing executable sections.
    printf ("Disassembler::disassembleFile(): Looking for pointers that reference executable code (valid sections) \n");
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
                // This could be a perfomance problem depending upon the implementation of the "content()" function using STL.
                for (size_t k = pointerSize; k > 0; --k) {
                    addr <<= 8;
                    addr |= *sect->content(j + k - 1, 1);
                }

                addr += header->get_base_va();
                if (file.inCodeSegment(addr)) {
                    basicBlockStarts[addr] = true;
                    printf ("Disassembler::disassembleFile(): SgAsmGenericSection list[%zu]: addr = %p \n",i,(void*)addr);
                    file.disassembleRecursively(addr, insns, basicBlockStarts);
                }
            }
        }
    }
#endif

    /* Adjust basicBlockStarts and functionStarts to indicate the starting (lowest) address of all known functions. This must
     * be done before we assign instructions to basic blocks since any newly detected function starts must necessarily also be
     * the beginning of a basic block. */
     detectFunctionStarts(interp, insns, basicBlockStarts, functionStarts);

  /* Assign instructions to basic blocks based on the addresses in the basicBlockStarts map. */
     map<uint64_t, SgAsmBlock*> basicBlocks;
     for (DisassemblerCommon::BasicBlockStarts::const_iterator i = basicBlockStarts.begin(); i != basicBlockStarts.end(); ++i) {
          uint64_t addr = i->first;
          SgAsmBlock* b = new SgAsmBlock();
          b->set_address(addr);
          b->set_id(addr);
          basicBlocks[addr] = b;
        }

     SgAsmBlock *blk = PutInstructionsIntoBasicBlocks::putInstructionsIntoBasicBlocks(basicBlocks, insns);

    /* Look for basic blocks that have instruction patterns that indicate they could be the start of a function. */
    if (heuristicFunctionDetection) {
        const SgAsmStatementPtrList& computedBasicBlocks = blk->get_statementList();
        SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
        for (size_t i = 0; i < computedBasicBlocks.size(); ++i) {
            SgAsmBlock* bb = isSgAsmBlock(computedBasicBlocks[i]);
            if (!bb) continue;

            SgAsmGenericSection* section = file.getSectionOfAddress(bb->get_address());
            if (!section) continue;
            ROSE_ASSERT (section->get_header() == header);

            bool isFunctionStart = false;

            if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family) {
                isFunctionStart = X86Disassembler::doesBBStartFunction(bb, false);
            } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family) {
                isFunctionStart = X86Disassembler::doesBBStartFunction(bb, true);
            } else if (isa == SgAsmExecutableFileFormat::ISA_PowerPC) {
                isFunctionStart = PowerpcDisassembler::doesBBStartFunction(bb, true);
            } else {
                fprintf(stderr, "Bad architecture to disassemble (isa=0x%04x)\n", isa);
                abort();
            }
            if (isFunctionStart)
                functionStarts[bb->get_address()].reason |= SgAsmFunctionDeclaration::FUNC_PATTERN;
        }
    }

    /* Put basic blocks (SgAsmBlock) into functions (SgAsmFunctionDeclaration) */
    blk = PutInstructionsIntoBasicBlocks::putInstructionsIntoFunctions(blk, functionStarts);
    interp->set_global_block(blk);
    blk->set_parent(interp);
}
