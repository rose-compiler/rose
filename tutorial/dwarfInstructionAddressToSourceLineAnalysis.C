#include "rose.h"

int
main(int argc, char** argv)
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

// This is controled by using the --with-dwarf configure command line option.
#if USE_ROSE_DWARF_SUPPORT

  // The input file is the binary file...
     int binary_file_id = project->get_fileList()[0]->get_file_info()->get_file_id();

  // Increment to get the next file id (for the source file, instead of the binary file)
     int source_file_id = binary_file_id + 1;

     std::string binaryFilename = Sg_File_Info::getFilenameFromID(binary_file_id);
     printf ("file_id = %d binaryFilename = %s \n",binary_file_id,binaryFilename.c_str());

     std::string sourceFilename = Sg_File_Info::getFilenameFromID(source_file_id);
     printf ("file_id = %d sourceFilename = %s \n",source_file_id,sourceFilename.c_str());

  // Compute the source line range from the instructions in the binary executable
     std::pair<LineColumnFilePosition,LineColumnFilePosition> sourceFileRange;
     sourceFileRange = SgAsmDwarfLineList::sourceCodeRange( source_file_id );

     printf ("\nSource file line number range for:\n -- file = %s (id = %d)\n -- [(line=%d, col=%d), (line=%d, col=%d)] \n",
          sourceFilename.c_str(),source_file_id,
          sourceFileRange.first.first, sourceFileRange.first.second, 
          sourceFileRange.second.first, sourceFileRange.second.second);

  // Compute the binary executable instruction address range
     std::pair<uint64_t,uint64_t> addressRange = SgAsmDwarfLineList::instructionRange();
     printf ("\nBinary instruction address range = (0x%lx, 0x%lx) \n",addressRange.first,addressRange.second);

     int minLine = sourceFileRange.first.first;
     int maxLine = sourceFileRange.second.first;
     int columnNumber = -1;

     printf ("\nInstruction addresses computed from source positions: \n");
  // Iterate over line numbers to map back to instruction addresses
     for (int lineNumber = minLine - 2; lineNumber <= maxLine + 2; lineNumber++)
        {
       // Out of range values generate the next address or NULL.
          FileIdLineColumnFilePosition s(source_file_id,std::pair<int,int>(lineNumber,columnNumber));
          uint64_t instructionAddress = SgAsmDwarfLineList::sourceCodeToAddress(s);
          printf ("   sourceCodeToAddress(%d,%d,%d) = 0x%lx \n",s.first,s.second.first,s.second.second,instructionAddress);
        }

     uint64_t minInstructionAddress = addressRange.first;

  // Iterate over the addresses of the binary and compute the source code line numbers (limit the range so the output will be short).
     printf ("\nSource lines computed from address range (truncated to keep output short): \n");
     for (uint64_t address = minInstructionAddress - 1; address < minInstructionAddress + 25; address++)
        {
          FileIdLineColumnFilePosition s_map = SgAsmDwarfLineList::addressToSourceCode(address);
          printf ("   addressToSourceCode: address 0x%lx = (%d,%d,%d) \n",address,s_map.first,s_map.second.first,s_map.second.second);
        }
#else
     printf ("\n\nROSE must be configured with --with-dwarf=<path to libdwarf> to use Dwarf support. \n\n");
#endif

     printf ("\nProgram Terminated Normally! \n\n");

  // Skip call to backend since this is just an analysis.
     return backend(project);
   }
