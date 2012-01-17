
// Support for writing a FLIRT data base from an existing AST.

#include <rose.h>

#include <libraryIdentification.h>

// Use the MD5 implementation that is in Linux.
// I don't need this byt Andreas will...
#include <openssl/md5.h>

// Function prototype looks like:
// unsigned char *MD5(const unsigned char *d, unsigned long n, unsigned char *md);

using namespace std;

#if 0
// Example of now to use the SQL DataBase
  FunctionIdentification ident("test.db");


  std::string testString = "test instr seq" ;
  {
    std::cerr << "Testing insert " << std::endl;
    library_handle handle;

    handle.filename= "foo";
    handle.function_name="bar";
    handle.begin=1;
    handle.end=10;
    ident.set_function_match(handle,testString);

  }

  {
    std::cerr << "Testing get " << std::endl;

    library_handle handle;

    if( !ident.get_function_match(handle,testString) )
    {
      std::cerr << "Function not found. Exiting." << std::endl;
      exit(1);
    }
      
      ;
    std::cout << "Found function " << handle.filename << " " << handle.function_name 
      << " " << handle.begin << " " << handle.end << std::endl;
  }
#endif

void
outputOpCodeVector(SgUnsignedCharList s, int index)
   {
  // Debugging support to output the OpCode vector.
     printf ("Function (printable)   %d: ",index);
     for (size_t i=0; i < s.size(); i++)
        {
          if (isprint(s[i]))
               printf("%c",s[i]);
            else
               printf(".");
        }
     printf("\n");
     printf ("Function (hexidecimal) %d: ",index);
     for (size_t i=0; i < s.size(); i++)
        {
          printf(" %x",s[i]);
        }
     printf("\n");
   }

void
LibraryIdentification::write_database ( FunctionIdentification & ident, const string & fileName, const string & functionName, size_t startOffset, size_t endOffset, const SgUnsignedCharList & s )
   {
     library_handle handle;

     handle.filename      = fileName;     // SageInterface::generateProjectName(project); // "foo";
     handle.function_name = functionName; // "function-" + StringUtility::numberToString(counter);

  // Need to compute these from the adress...
  // Use: size_t fileOffset = rva - section->get_mapped_rva() + section->get_offset();
     handle.begin = startOffset;
     handle.end   = endOffset;

  // set_function_match( const library_handle & handle, const std::string & data );
     ident.set_function_match(handle,s);
   }

bool
LibraryIdentification::match_database ( const FunctionIdentification & ident, string & fileName, string & functionName, size_t & startOffset, size_t & endOffset, const SgUnsignedCharList & s )
   {
     library_handle handle;

  // get_function_match( const library_handle & handle, const std::string & data );
     bool result = ident.get_function_match(handle,s);

     fileName     = handle.filename;
     functionName = handle.function_name;
     startOffset  = handle.begin;
     endOffset    = handle.end;

     return result;
   }

void
LibraryIdentification::testForDuplicateEntries( const vector<SgUnsignedCharList> & functionOpcodeList )
   {
  // Test for duplicate entries...
     int index_i = 0;
     for (vector<SgUnsignedCharList>::const_iterator i = functionOpcodeList.begin(); i != functionOpcodeList.end(); i++)
        {
          int index_j = 0;
          for (vector<SgUnsignedCharList>::const_iterator j = functionOpcodeList.begin(); j != functionOpcodeList.end(); j++)
             {
               if (i != j)
                  {
                    if (*i == *j)
                       {
                         printf ("Error, detected duplicate entry in functionOpcodeList index_i = %d index_j = %d \n",index_i,index_j);
#if 0
                         outputOpCodeVector(*i,index_i);
                         outputOpCodeVector(*j,index_j);
#endif
                      // ROSE_ASSERT(false);
                       }
                  }

               index_j++;
             }

          index_i++;
        }
   }


void
LibraryIdentification::libraryIdentificationDataBaseSupport( string databaseName, SgProject* project, bool generate_database )
   {
  // This is a factored low level support for generateLibraryIdentificationDataBase() and matchAgainstLibraryIdentificationDataBase()
  // This code if factored because most of the function is the loop support to access all the function in the different SgAsmInterpretation 
  // objects (Elf has 1, while PE have 2 SgAsmInterpretation objects).

  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);

     printf ("Building LibraryIdentification database: %s from AST of project: %p \n",databaseName.c_str(),project);

  // Example of build the SQL DataBase
     FunctionIdentification ident(databaseName);

     Rose_STL_Container<SgNode*> binaryInterpretationList = NodeQuery::querySubTree (project,V_SgAsmInterpretation);

  // This is something we can assert on Linux (Elf binary file format), but not for a library archive.
  // ROSE_ASSERT(binaryInterpretationList.size() == 1);

     vector<SgUnsignedCharList> functionOpcodeList;

     printf ("*********************************** \n");
     printf ("Traverse the AST to find functions: \n");
     printf ("*********************************** \n");

  // int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator j = binaryInterpretationList.begin(); j != binaryInterpretationList.end(); j++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgAsmInterpretation* asmInterpretation = isSgAsmInterpretation(*j);
          ROSE_ASSERT(asmInterpretation != NULL);

          printf ("Calling the NodeQuery::querySubTree() on SgAsmFunction\n");
          Rose_STL_Container<SgNode*> binaryFunctionList = NodeQuery::querySubTree (asmInterpretation,V_SgAsmFunction);
          printf ("DONE: Calling the NodeQuery::querySubTree() on SgAsmFunction\n");

          for (Rose_STL_Container<SgNode*>::iterator i = binaryFunctionList.begin(); i != binaryFunctionList.end(); i++)
             {
            // Build a pointer to the current type so that we can call the get_name() member function.
               SgAsmFunction* binaryFunction = isSgAsmFunction(*i);
               ROSE_ASSERT(binaryFunction != NULL);

               string mangledFunctionName   = binaryFunction->get_name();
               printf ("mangledFunctionName = %s \n",mangledFunctionName.c_str());
               string demangledFunctionName = StringUtility::demangledName(mangledFunctionName);
               printf ("demangledFunctionName = %s \n",demangledFunctionName.c_str());
#if 0
            // For debugging ... skip the unnamed functions where are not really present in the object file.
               if (binaryFunction->get_name().empty() == true)
                    continue;
#endif
               printf ("\n\n*********************************************************** \n");
               printf ("    Binary Function %p = %s demangled = %s \n",binaryFunction,mangledFunctionName.c_str(),demangledFunctionName.c_str());
               printf ("******************************************************************* \n");

            // Note that we need a SgAsmInterpretation object in generateOpCodeVector() to compute the 
            // section.  This might not make sense.
               size_t startOffset = 0, endOffset = 0;
               SgUnsignedCharList s = generateOpCodeVector(asmInterpretation,binaryFunction, startOffset, endOffset);
               functionOpcodeList.push_back(s);
#if 0
            // There seem to be a lot of dumplicate entries in object files.
               testForDuplicateEntries(functionOpcodeList);
#endif
               if (s.empty() == true)
                  {
                    printf ("Warning: zero length function \n");
                    continue;
                  }

               if (generate_database == true)
                  {
                 // Generate the database using all the functions ... (in this file).
                    string fileName = SageInterface::generateProjectName(project); // "foo";
                 // string functionName = "function-" + StringUtility::numberToString(counter);
                    string functionName = binaryFunction->get_name();

                    write_database (ident,fileName,functionName,startOffset,endOffset,s);
                  }
                 else
                  {
                 // Read data base and look for a match
                    string fileName;
                    string functionName;

                    bool found_match = match_database (ident,fileName,functionName,startOffset,endOffset,s);

                    printf ("found_match test: fileName = %s functionName = %s found_match = %s \n",fileName.c_str(),functionName.c_str(),found_match ? "true" : "false");
                  }
#if 0
            // Debugging output
               outputOpCodeVector(s,counter);
#endif
            // Increment the counter used to name the functions
            // counter++;
             }
        }
     printf ("DONE: Traverse the AST to file functions \n");
   }

void
LibraryIdentification::generateLibraryIdentificationDataBase( string databaseName, SgProject* project )
   {
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);

     printf ("Building LibraryIdentification database: %s from AST of project: %p \n",databaseName.c_str(),project);

     libraryIdentificationDataBaseSupport(databaseName,project,/* generate_database */ true);
   }

void
LibraryIdentification::FlattenAST::visit(SgNode* n)
   {
  // This is the older function which just generated the opcodes for each instruction (un-normalized).

     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
       // printf ("asmInstruction = %p \n",asmInstruction);

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

          printf ("asmInstruction->get_mnemonic() = %s size = %zu \n",asmInstruction->get_mnemonic().c_str(),opCodeString.size());
        }

     SgAsmValueExpression* asmExpression = isSgAsmValueExpression(n);
     if (asmExpression != NULL)
        {
          printf ("   asmExpression->get_bit_size() = %u asmExpression->get_bit_offset() = %u \n",asmExpression->get_bit_size(),asmExpression->get_bit_offset());
        }

  // ROSE_ASSERT(endAddress != startAddress);
   }

LibraryIdentification::FlattenAST_SynthesizedAttribute
// FlattenAST_AndResetImmediateValues::evaluateSynthesizedAttribute ( SgNode* n, FlattenAST_AndResetImmediateValues::SynthesizedAttributesList childAttributes )
LibraryIdentification::FlattenAST_AndResetImmediateValues::evaluateSynthesizedAttribute ( SgNode* n, SynthesizedAttributesList childAttributes )
   {
  // Build the return value
     LibraryIdentification::FlattenAST_SynthesizedAttribute localResult;

  // Collect all the bit offset ranges and bit range sizes and accumulate them into localResult
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
          printf ("localResult.rangeList.size() = %zu \n",localResult.rangeList.size());
#endif

     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
       // printf ("asmInstruction = %p \n",asmInstruction);

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

          printf ("asmInstruction->get_mnemonic() = %s size = %zu \n",asmInstruction->get_mnemonic().c_str(),opCodeString.size());

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
#if 0
               printf ("   asmExpression->get_bit_size() = %u asmExpression->get_bit_offset() = %u \n",size,offset);
#endif
               localResult.rangeList.push_back(std::pair<unsigned char,unsigned char> (size,offset) );
             }
        }

     return localResult;
   }


SgUnsignedCharList
LibraryIdentification::generateOpCodeVector(SgAsmInterpretation* asmInterpretation, SgNode* node, size_t & startOffset, size_t & endOffset)
   {
     ROSE_ASSERT(asmInterpretation != NULL);

     SgUnsignedCharList s;
#if 0
  // This generates the opcode byte array (non-normalized)
     FlattenAST t(s);

     printf ("Traverse the AST for this function to generate byte stream, node = %p \n",node);
     t.traverse(node,preorder);
#else
  // This generates the opcode byte array in a normalized for (zeroed bit locations where imediates are coded).
     FlattenAST_AndResetImmediateValues t(s);

     printf ("Synthesized attribute traverse the AST for this function to generate byte stream, node = %p \n",node);
     t.traverse(node);
#endif
     printf ("DONE: Traverse the AST for this function to generate byte stream s.size() = %zu \n",s.size());

     size_t startAddress = t.startAddress;
     size_t endAddress   = t.endAddress;

     printf ("startAddress = %p endAddress = %p \n",(void*)startAddress,(void*)endAddress);

     if (s.empty() == false)
        {
       // Compute the offset from the address...
       // Need to compute these from the adress...
       // Use: size_t fileOffset = rva - section->get_mapped_rva() + section->get_offset();

#if 0
       /* There are lots of other disassembler methods now. I'm not sure why we want a new AsmFileWithData, unless it was
        * just for finding the section that goes with an address.  The SgAsmGenericHeader::get_best_section_by_va() and related
        * methods are the correct ones to call for that info. [RPM 2009-06-23] */
       // Build a AsmFileWithData object with the relevant SgAsmInterpretation
       // Later we can record a list of them.
          DisassemblerCommon::AsmFileWithData asmFileInformation (asmInterpretation);
#endif


#if 0
       /* The code assumes that a SgAsmInterpretation points to a single binary file. This isn't the case with library
        * archives or dynamic linking. Therefore returning startOffset and endOffset doesn't make much sense--they might be
        * offsets in two different files (or an offset in anonymous memory).  The replacement code returns the offsets for the
        * instructions with the lowest and highest virtual addresses anway.   See Disassembler::mark_referenced_instructions()
        * for some ideas since that function also maps virtual addresses to file offsets. [RPM 2009-09-23] */

       // We need a DisassemblerCommon::AsmFileWithData object to call getSectionOfAddress()
       // SgAsmGenericSection* section = DisassemblerCommon::AsmFileWithData::getSectionOfAddress(t.startAddress);
          SgAsmGenericHeader* fhdr = asmInterpretation->get_header();
          ROSE_ASSERT(fhdr != NULL);
          SgAsmGenericSection* section = fhdr->get_best_section_by_va(fhdr->get_base_va()+t.startAddress);
          ROSE_ASSERT(section != NULL);

       /* This code assumes that the entire sequence of instructions is present in a single section, or a group of sections that
        * are mapped in such a way that that file layout mirrors virtual memory layout. This isn't always the case. In fact, the
        * disassembler is now able to disassemble instructions that even span two sections such that the first bytes of the
        * instruction are at one file offset and the last few bytes are at a wildly different offset. [RPM 2009-06-23]
        *
        * This code also assumes that the virtual address specified in the binary file is the same address that ROSE
        * ultimately uses when it simulates the mapping and relocation fixups performed by the loader. [RPM 2009-09-09] */
          startOffset = startAddress - section->get_mapped_preferred_rva() + section->get_offset();
          endOffset   = endAddress - section->get_mapped_preferred_rva() + section->get_offset();
#else
          MemoryMap *map = asmInterpretation->get_map(); /*map that was used durring disassembly*/
          ROSE_ASSERT(map!=NULL);
          const MemoryMap::MapElement *me = map->find(startAddress);
          ROSE_ASSERT(me!=NULL);
          startOffset = me->get_offset();
          me = map->find(endAddress);
          ROSE_ASSERT(me!=NULL);
          endOffset = me->get_offset();
#endif

          printf ("---- function %p addresses: (start = %p, end = %p) file offsets: (start = %zu, end = %zu) \n",node,(void*)startAddress,(void*)endAddress,startOffset,endOffset);

          size_t lengthOfOpcodeVectorByAddress = endAddress - startAddress;
          size_t lengthOfOpcodeVectorBySize    = s.size();

          printf ("---- lengthOfOpcodeVectorByAddress = %zu lengthOfOpcodeVectorBySize = %zu \n",lengthOfOpcodeVectorByAddress,lengthOfOpcodeVectorBySize);

       // DQ (7/11/2009): See the email from Robb (7/10/2009) for an explaination of why this is an inapropriate 
       // thing to assert (I think it used to be fine under the previous implementation)
       // ROSE_ASSERT(lengthOfOpcodeVectorByAddress == lengthOfOpcodeVectorBySize);
        }
       else
        {
          printf ("Warning: found a zero length function node = %p \n",node);
        }

     return s;
   }

