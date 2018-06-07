
// Support for writing a FLIRT data base from an existing AST.

#include "sage3basic.h"                                 // every librose .C file must start with this

#include <libraryIdentification.h>

using namespace std;
using namespace Rose;



/**  generateLibraryIdentificationDataBase
 *  
 *  Creates or updates a binary function identification database with
 *  the functions in the project.  
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] libraryName  Library names cannot be discovered from all
 *                         library types, so pass in name.
 * @param[in] libraryVersion  Library version, same problem
 * @param[in] project      Rose SgProject that has the functions to
 * write or find
 **/
void
LibraryIdentification::generateLibraryIdentificationDataBase( const std::string& databaseName, 
                                                              const std::string& libraryName, 
                                                              const std::string& libraryVersion, 
                                                              SgProject* project )
{
    // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
    TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);
    
    printf ("Building LibraryIdentification database: %s from AST of library: %s : %s \n", databaseName.c_str(),libraryName.c_str(), libraryVersion.c_str());
    
    FunctionIdDatabaseInterface ident(databaseName);
    
    Rose_STL_Container<SgAsmGenericFile*> fileList = SageInterface::querySubTree<SgAsmGenericFile>(project);
    if(fileList.size() > 1) {
        std::cout << "libraryIdentification ERROR, only one library at a time" << std::endl;
        ASSERT_require(false);
    }
    LibraryInfo libraryInfo( libraryName, libraryVersion, fileList[0]);
    ident.addLibraryToDB(libraryInfo);
    
    Rose_STL_Container<SgNode*> binaryInterpretationList = NodeQuery::querySubTree (project,V_SgAsmInterpretation);
    
    for (Rose_STL_Container<SgNode*>::iterator j = binaryInterpretationList.begin(); j != binaryInterpretationList.end(); j++)
        {
            SgAsmInterpretation* asmInterpretation = isSgAsmInterpretation(*j);
            ASSERT_require(asmInterpretation != NULL);
            
            //Now get all the function for the interpretation
            Rose_STL_Container<SgNode*> binaryFunctionList = NodeQuery::querySubTree (asmInterpretation,V_SgAsmFunction);
            
            for (Rose_STL_Container<SgNode*>::iterator i = binaryFunctionList.begin(); i != binaryFunctionList.end(); i++)
                {
                    // Build a pointer to the current type so that we can call the get_name() member function.
                    SgAsmFunction* binaryFunction = isSgAsmFunction(*i);
                    ROSE_ASSERT(binaryFunction != NULL);
                    
                    FunctionInfo functionInfo(binaryFunction, libraryInfo);
                    
                    
                    string mangledFunctionName   = binaryFunction->get_name();
                    string demangledFunctionName = StringUtility::demangledName(mangledFunctionName);
                    printf ("Function %s demangled = %s going into database\n", mangledFunctionName.c_str(), demangledFunctionName.c_str());
                    
                    ident.addFunctionToDB(functionInfo);
                    
                }
        }
    
}



/**  FlattenAST::visit
 *  
 * Called by traverse.  Gets the opcode for each instruction and puts
 * it in the data opcodeVector.  This function isn't used anymore, in
 * defrence to FlattenAST_AndResetImmediateValues, which does the same
 * thing, but also "normalizes" the data. (zeroed bit locations where
 * immediates are coded) 
 *
 * @param[in] n node to get opcode of (if it's an asmInstruction)
 **/
void
LibraryIdentification::FlattenAST::visit(SgNode* n)
   {
  // This is the older function which just generated the opcodes for each instruction (un-normalized).

     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {

          size_t instructionAddress = asmInstruction->get_address();
          if (startAddress == 0)
             startAddress = instructionAddress;

       // Get the op-code for each instruction and append them to the STL data vector.
          SgUnsignedCharList opCodeString = asmInstruction->get_raw_bytes();

       // DQ (7/11/2009): Adding code to zero out the offsets that support immediates.
       // if SgAsmValueExpression::p_bit_size is nonzero { zero out corresponding bits of checksum buffer }

          for (size_t i=0; i < opCodeString.size(); i++)
             {
               data.push_back(opCodeString[i]);
             }

       // Always update the endAddress (and add the length of the last instruction)
          endAddress = instructionAddress + opCodeString.size();

          printf ("asmInstruction->get_mnemonic() = %s size = %" PRIuPTR " \n",asmInstruction->get_mnemonic().c_str(),opCodeString.size());
        }

     SgAsmValueExpression* asmExpression = isSgAsmValueExpression(n);
     if (asmExpression != NULL)
        {
          printf ("   asmExpression->get_bit_size() = %u asmExpression->get_bit_offset() = %u \n",asmExpression->get_bit_size(),asmExpression->get_bit_offset());
        }
   }

/**  LibraryIdentification::FlattenAST_RangeListAttribute
 *  
 * This function is called by a bottom-up traverse.  It's intended to
 * make an opcode list similar to FlattenAST, except with any
 * immediates in the instructions set to 0.  
 * The idea there is to avoid having static linking change pointers,
 * and therefore having an identical function in two different
 * binaries appear as different functions.  (Note, this could
 * misidentify functions that only differ by an integer for example,
 * Add+2 and Add+3.
 * 
 * This works by saving the bit_offset and bit_size of any
 * SgAsmValueExpression to the synthesized attribute, then the parent
 * will try to zero out the immediates.
 *
 * @param[in] n node to get opcode of (if it's an asmInstruction)
 * @param[in] childAttributes The rangeLists of all the children
 **/
LibraryIdentification::FlattenAST_RangeListAttribute
// FlattenAST_AndResetImmediateValues::evaluateRangeListAttribute ( SgNode* n, FlattenAST_AndResetImmediateValues::RangeListAttributesList childAttributes )
LibraryIdentification::FlattenAST_AndResetImmediateValues::evaluateSynthesizedAttribute ( SgNode* n, SynthesizedAttributesList childAttributes )
   {
  // Build the return value
     LibraryIdentification::FlattenAST_RangeListAttribute localResult;

  // Collect all the bit offset ranges and bit range sizes and
  // accumulate them into localResult
     //foreach child { for each child.item { localResult.append(child.item)}}
     SynthesizedAttributesList::iterator i = childAttributes.begin();
     while (i != childAttributes.end())
        {
          std::vector<std::pair<unsigned char,unsigned char> >::iterator j = i->rangeList.begin();
          while (j != i->rangeList.end())
             {
               localResult.rangeList.push_back(*j);

               j++;
             }
          i++;
        }

#if 0
  // Debugging code
     if (localResult.rangeList.size() > 0)
          printf ("localResult.rangeList.size() = %" PRIuPTR " \n",localResult.rangeList.size());
#endif

     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
          size_t instructionAddress = asmInstruction->get_address();
          if (startAddress == 0)
               startAddress = instructionAddress;

       // Get the op-code for each instruction and append them to the STL data vector.
          SgUnsignedCharList opCodeString = asmInstruction->get_raw_bytes();

       // DQ (7/11/2009): Adding code to zero out the offsets that support immediates.
       // if SgAsmValueExpression::p_bit_size is nonzero { zero out corresponding bits of checksum buffer }

          for (size_t i=0; i < opCodeString.size(); i++)
             {
               data.push_back(opCodeString[i]);
             }

       // Always update the endAddress (and add the length of the last instruction)
          endAddress = instructionAddress + opCodeString.size();

          printf ("asmInstruction->get_mnemonic() = %s size = %" PRIuPTR " \n",asmInstruction->get_mnemonic().c_str(),opCodeString.size());

          std::vector<std::pair<unsigned char,unsigned char> >::iterator k = localResult.rangeList.begin();
          while (k != localResult.rangeList.end())
             {
            // localResult.rangeList.push_back(*k);
               unsigned char bit_size   = k->first;
               unsigned char bit_offset = k->second;

#if 0
            // Debugging code
               printf ("   bit_size = %u bit_offset = %u \n",bit_size,bit_offset);
#endif
               //Change to byte sized chunks, only works because x86
               //only has byte and word sized immediates
               unsigned char offset_div_8 = bit_offset / 8;
               unsigned char size_div_8   = bit_size   / 8;

            // unsigned char offset_mod_8 = bit_offset % 8;
            // unsigned char size_mod_8   = bit_size   % 8;

               for (int i = 0; i < size_div_8; i++)
                  {
                    printf ("Setting byte #%u of instruction op-code to zero \n",offset_div_8+i);

                 // For now just reset the relevant bytes (this is sufficent for x86, but we really want the more general solution).
                 // Enforce this using an assert.
                    ROSE_ASSERT(bit_size % 8 == 0);

                    opCodeString[offset_div_8+i] = 0;
#if 0
                 // What we really want to do is set the relevant bits to zero, but debug this later.
                    for (int i = start; i < end; i++)
                       {
                         opCodeString[index] = opCodeString[index] & 1 << i;
                       }
#endif
                  }

               k++;
             }
        }

     SgAsmValueExpression* asmExpression = isSgAsmValueExpression(n);
     if (asmExpression != NULL)
        {
          unsigned char size   = asmExpression->get_bit_size();
          unsigned char offset = asmExpression->get_bit_offset();

          if (size > 0)
             {
               localResult.rangeList.push_back(std::pair<unsigned char,unsigned char> (size,offset) );
             }
        }

     return localResult;
   }


// /**  generateOpCodeVector
//  *  
//  *  Generates an opcode vector from root (should always be a
//  *  function?) which is returned.  startOffset and endOffset are also
//  *  filled in.
//  *  Uses FunctionIdentification::FlattenAST internally
//  *
//  * @param[in] asmInterpretation The assembler interepretation to use
//  * @param[in] root The root of the AST to generate the vector from. Function?  
//  * @param[out] startOffset  The file pointer where the function begins
//  * @param[out] endOffset  The file pointer where the function ends
//  * @return The completed opcode vector
//  **/
// SgUnsignedCharList
// LibraryIdentification::generateOpCodeVector(SgAsmInterpretation* asmInterpretation, SgNode* root, size_t & startOffset, size_t & endOffset)
//    {
//      ROSE_ASSERT(asmInterpretation != NULL);

//      SgUnsignedCharList functionBytes;

// #if 0
//   // This generates the opcode byte array (non-normalized) CURRENTLY UNUSED
//      FlattenAST flatAST(functionBytes);
//      flatAST.traverse(root,preorder);
// #else
//   // This generates the opcode byte array in a normalized for (zeroed bit locations where immediates are coded).
//      FlattenAST_AndResetImmediateValues flatAST(functionBytes);

//      printf ("Synthesized attribute traverse the AST for this function to generate byte stream, node = %p \n",root);
//      flatAST.traverse(root);
// #endif
//      printf ("DONE: Traverse the AST for this function to generate byte stream functionBytes.size() = %" PRIuPTR " \n",functionBytes.size());

//      size_t startAddress = flatAST.startAddress;
//      size_t endAddress   = flatAST.endAddress;

//      printf ("startAddress = %p endAddress = %p \n",(void*)startAddress,(void*)endAddress);

//      if (functionBytes.empty() == false)
//         {
//        // Compute the offset from the address...
//        // Need to compute these from the adress...
//        // Use: size_t fileOffset = rva - section->get_mapped_rva() + section->get_offset();

// #if 0
//        /* There are lots of other disassembler methods now. I'm not sure why we want a new AsmFileWithData, unless it was
//         * just for finding the section that goes with an address.  The SgAsmGenericHeader::get_best_section_by_va() and related
//         * methods are the correct ones to call for that info. [RPM 2009-06-23] */
//        // Build a AsmFileWithData object with the relevant SgAsmInterpretation
//        // Later we can record a list of them.
//           DisassemblerCommon::AsmFileWithData asmFileInformation (asmInterpretation);
// #endif


// #if 0
//        /* The code assumes that a SgAsmInterpretation points to a single binary file. This isn't the case with library
//         * archives or dynamic linking. Therefore returning startOffset and endOffset doesn't make much sense--they might be
//         * offsets in two different files (or an offset in anonymous memory).  The replacement code returns the offsets for the
//         * instructions with the lowest and highest virtual addresses anway.   See Disassembler::mark_referenced_instructions()
//         * for some ideas since that function also maps virtual addresses to file offsets. [RPM 2009-09-23] */

//        // We need a DisassemblerCommon::AsmFileWithData object to call getSectionOfAddress()
//        // SgAsmGenericSection* section = DisassemblerCommon::AsmFileWithData::getSectionOfAddress(flatAST.startAddress);
//           SgAsmGenericHeader* fhdr = asmInterpretation->get_header();
//           ROSE_ASSERT(fhdr != NULL);
//           SgAsmGenericSection* section = fhdr->get_best_section_by_va(fhdr->get_base_va()+flatAST.startAddress);
//           ROSE_ASSERT(section != NULL);

//        /* This code assumes that the entire sequence of instructions is present in a single section, or a group of sections that
//         * are mapped in such a way that that file layout mirrors virtual memory layout. This isn't always the case. In fact, the
//         * disassembler is now able to disassemble instructions that even span two sections such that the first bytes of the
//         * instruction are at one file offset and the last few bytes are at a wildly different offset. [RPM 2009-06-23]
//         *
//         * This code also assumes that the virtual address specified in the binary file is the same address that ROSE
//         * ultimately uses when it simulates the mapping and relocation fixups performed by the loader. [RPM 2009-09-09] */
//           startOffset = startAddress - section->get_mapped_preferred_rva() + section->get_offset();
//           endOffset   = endAddress - section->get_mapped_preferred_rva() + section->get_offset();
// #else
//           BinaryAnalysis::MemoryMap::Ptr map = asmInterpretation->get_map(); /*map that was used durring disassembly*/
//           ROSE_ASSERT(map!=NULL);
//           ROSE_ASSERT(map->at(startAddress).exists());
//           const BinaryAnalysis::MemoryMap::Node &me1 = *(map->at(startAddress).findNode());
//           startOffset = me1.value().offset() + startAddress - me1.key().least();
//           ROSE_ASSERT(map->at(endAddress).exists());
//           const BinaryAnalysis::MemoryMap::Node &me2 = *(map->at(endAddress).findNode());
//           endOffset = me2.value().offset() + endAddress - me2.key().least();
// #endif

//           printf ("---- function %p addresses: (start = %p, end = %p) file offsets: (start = %" PRIuPTR ", end = %" PRIuPTR ") \n",root,(void*)startAddress,(void*)endAddress,startOffset,endOffset);

//           size_t lengthOfOpcodeVectorByAddress = endAddress - startAddress;
//           size_t lengthOfOpcodeVectorBySize    = functionBytes.size();

//           printf ("---- lengthOfOpcodeVectorByAddress = %" PRIuPTR " lengthOfOpcodeVectorBySize = %" PRIuPTR " \n",lengthOfOpcodeVectorByAddress,lengthOfOpcodeVectorBySize);

//         }
//        else
//         {
//           printf ("Warning: found a zero length function node = %p \n",root);
//         }

//      return functionBytes;
//    }


// /**
//  *  outputOpCodeVector 
//  *
//  *  Debugging support.  Prints an OpCode vector to stdout.  First as
//  *  characters, then as hex.
//  *
//  *  TODO: If we want to keep this, we should have it use Robb's logging and
//  *  disassember features.
//  *
//  * @param[in] s The OpCode vector 
//  * @param[in] index            The index at which the opcode vector starts. (Assumes its a function)
//  **/
// void
// outputOpCodeVector(SgUnsignedCharList s, int index)
//    {
//      printf ("Function (printable)   %d: ",index);
//      for (size_t i=0; i < s.size(); i++)
//         {
//           if (isprint(s[i]))
//                printf("%c",s[i]);
//             else
//                printf(".");
//         }
//      printf("\n");
//      printf ("Function (hexidecimal) %d: ",index);
//      for (size_t i=0; i < s.size(); i++)
//         {
//           printf(" %x",s[i]);
//         }
//      printf("\n");
//    }


