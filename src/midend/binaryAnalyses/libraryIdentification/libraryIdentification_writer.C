
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
     int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator j = binaryInterpretationList.begin(); j != binaryInterpretationList.end(); j++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgAsmInterpretation* asmInterpretation = isSgAsmInterpretation(*j);
          ROSE_ASSERT(asmInterpretation != NULL);

          printf ("Calling the NodeQuery::querySubTree() on SgAsmFunctionDeclaration \n");
          Rose_STL_Container<SgNode*> binaryFunctionList = NodeQuery::querySubTree (asmInterpretation,V_SgAsmFunctionDeclaration);
          printf ("DONE: Calling the NodeQuery::querySubTree() on SgAsmFunctionDeclaration \n");

          for (Rose_STL_Container<SgNode*>::iterator i = binaryFunctionList.begin(); i != binaryFunctionList.end(); i++)
             {
            // Build a pointer to the current type so that we can call the get_name() member function.
               SgAsmFunctionDeclaration* binaryFunction = isSgAsmFunctionDeclaration(*i);
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
     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
       // printf ("asmInstruction = %p \n",asmInstruction);

          size_t instructionAddress = asmInstruction->get_address();
          if (startAddress == 0)
             startAddress = instructionAddress;

       // Get the op-code for each instruction and append them to the STL data vector.
          SgUnsignedCharList opCodeString = asmInstruction->get_raw_bytes();
          for (size_t i=0; i < opCodeString.size(); i++)
             {
               data.push_back(opCodeString[i]);
             }

       // Always update the endAddress (and add the length of the last instruction)
          endAddress = instructionAddress + opCodeString.size();
        }

  // ROSE_ASSERT(endAddress != startAddress);
   }


SgUnsignedCharList
LibraryIdentification::generateOpCodeVector(SgAsmInterpretation* asmInterpretation, SgNode* node, size_t & startOffset, size_t & endOffset)
   {
     ROSE_ASSERT(asmInterpretation != NULL);

     SgUnsignedCharList s;
     FlattenAST t(s);

     printf ("Traverse the AST for this function to generate byte stream, node = %p \n",node);
     t.traverse(node,preorder);
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

  // We need a DisassemblerCommon::AsmFileWithData object to call getSectionOfAddress()
  // SgAsmGenericSection* section = DisassemblerCommon::AsmFileWithData::getSectionOfAddress(t.startAddress);
     SgAsmGenericHeader* fhdr = asmInterpretation->get_header();
     ROSE_ASSERT(fhdr != NULL);
     SgAsmGenericSection* section = fhdr->get_best_section_by_va(fhdr->get_base_va()+t.startAddress);
     ROSE_ASSERT(section != NULL);

     /* This code assumes that the entire sequence of instructions is present in a single section, or a group of sections that
      * are mapped in such a way that that file layout mirrors virtual memory layout. This isn't always the case. In fact, the
      * disassembler is now able to disassemble instructions that even span two sections such that the first bytes of the
      * instruction are at one file offset and the last few bytes are at a wildly different offset. [RPM 2009-06-23] */
     startOffset = startAddress - section->get_mapped_rva() + section->get_offset();
     endOffset   = endAddress - section->get_mapped_rva() + section->get_offset();

     printf ("---- function %p addresses: (start = %p, end = %p) file offsets: (start = %zu, end = %zu) \n",node,(void*)startAddress,(void*)endAddress,startOffset,endOffset);

     size_t lengthOfOpcodeVectorByAddress = endAddress - startAddress;
     size_t lengthOfOpcodeVectorBySize    = s.size();

     printf ("---- lengthOfOpcodeVectorByAddress = %zu lengthOfOpcodeVectorBySize = %zu \n",lengthOfOpcodeVectorByAddress,lengthOfOpcodeVectorBySize);
     ROSE_ASSERT(lengthOfOpcodeVectorByAddress == lengthOfOpcodeVectorBySize);
        }
       else
        {
          printf ("Warning: found a zero length function node = %p \n",node);
        }

     return s;
   }

