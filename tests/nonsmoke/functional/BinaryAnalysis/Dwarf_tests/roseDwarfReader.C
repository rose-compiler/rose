/*
A bit of info abour Dward versions numbers and sections 
available in each version number. Copied from:
http://wiki.dwarfstd.org/index.php?title=Dwarf_Version_Numbers

Dwarf Version Numbers:
   As DWARF4 is not released the list of entries for DWARF4 and 
indeed the existence of DWARF4 must be considered speculative.
The versions applicable by section are, so far: 

                      DWARF2    DWARF3 DWARF4
.debug_info             2         3     4
.debug_abbrev           -         -     -
.debug_frame            1         3     3
.debug_str              -         -     -
.debug_loc              -         -     -
.debug_line             2         3     3
.debug_aranges          2         2     2
.debug_ranges           x         -     -
.debug_pubtypes         x         2     2
.debug_pubnames         2         2     2
.debug_macinfo          -         -     -

Where - means there is no version number in the section, and 
where x means the section did not exist in that version.

*/



#include <rose.h>

// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************


// NOTES: Checkout "man objcopy" and the options for:
//         --add-gnu-debuglink=path-to-file
//           Creates a .gnu_debuglink section which contains a reference to path-to-file and adds it to the output file.
//         --only-keep-debug
//           Strip a file, removing any sections that would be stripped by --strip-debug and leaving the debugging sections.
// These allow for a two part binary.  Can libdwarf read such files?  
// We think that a new section is generated, what is its name?


int
main(int argc, char** argv)
   {
  // ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!
    
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST binary reader (main): time (sec) = ",true);

     try{
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Just set the project, the report will be generated upon calling the destructor for "timer"
     timer.set_project(project);

  // Internal AST consistancy tests.
     printf ("Internal testing of AST ... numberOfASTnodes = %zu\n", numberOfNodes());
     AstTests::runAllTests(project);

#if 1
     int numberOfASTnodes = numberOfNodes();
     printf ("Generate the dot output of the SAGE III AST numberOfASTnodes = %d \n",numberOfASTnodes);
     if (numberOfASTnodes <= 20000)
        {
       // generateDOT ( *project );
          generateDOTforMultipleFile ( *project );
          printf ("DONE: Generate the dot output of the SAGE III AST \n");
        }
       else
        {
          printf ("Skipping generation of dot file (too large to be useful), run a smaller program \n");
        }
#endif

  // Unparse the binary executable (as a binary, as an assembler text file, 
  // and as a dump of the binary executable file format details (sections)).
     return backend(project);

     }catch(SgAsmExecutableFileFormat::FormatError e) 
     {
       std::cerr << e.mesg << std::endl;

       exit(1);
     
     }
   }
