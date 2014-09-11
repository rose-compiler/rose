#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "fortran_support.h"

// DQ (9/10/2014): I think this is declared in the other Fortran support (both of which exist).
// SgSourceFile* OpenFortranParser_globalFilePointer = NULL;

using namespace std;

ATbool ofp_traverse_init();


int
experimental_openFortranParser_main(int argc, char **argv)
   {
   OFP::Program Program;
   OFP::UntypedASTBuilder ast;
   OFP::FortranTextUnparser * unparser = NULL;
   std::ofstream * ofs = NULL;
   FILE * file = stdin;

  // Make system call to call the parser and build an ATERM file (put into the build tree).
     printf ("In experimental_openFortranParser_main(): Put the call the the new SDF Open Fortran Parser here... \n");

  // This function does not need to call the JVM.

#if 0
     printf ("In experimental_openFortranParser_main(): Put the call the the new SDF Open Fortran Parser here... argc = %d \n",argc);
#endif

     int i, err;
  // char parse_table[128];
     string parse_table;

  // parse_table[0] = '\0';

     if (argc < 4) 
        {
          printf("usage: fortran_parser --parseTable parse_table_path filename(s)\n");
          return 1;
        }

  // string commandString = "sglri ";
     string commandString = "/home/dquinlan/bin/strategoxt-0.17-install-32bit/bin/sglri ";

#if 0
     printf ("In experimental_openFortranParser_main(): before loop over args: commandString = %s \n",commandString.c_str());
#endif

  // Parse each filename (args not associated with "--parseTable", "--" or "-I")
     for (i = 1; i < argc; i++)
        {
#if 0
          printf ("In experimental_openFortranParser_main(): i = %d argv[%d] = %s \n",i,i,argv[i]);
#endif
          if (strncmp(argv[i], "--parseTable", 12) == 0) 
             {
#if 0
               printf ("In experimental_openFortranParser_main(): argv[i+1 = %d] = %s \n",i+1,argv[i+1]);
#endif
#if 0
               printf ("In experimental_openFortranParser_main(): --parseTable: START: commandString = %s \n",commandString.c_str());
#endif
               commandString += "-p ";
               commandString += argv[i+1];
               commandString += " ";

#if 0
               printf ("In experimental_openFortranParser_main(): --parseTable: before sprintf: commandString = %s \n",commandString.c_str());
#endif
            // sprintf(parse_table, "%s", argv[i+1]);
               parse_table = string(argv[i+1]);
               i += 1;
            // continue;
#if 0
               printf ("In experimental_openFortranParser_main(): --parseTable: END: parse_table   = %s \n",parse_table.c_str());
               printf ("In experimental_openFortranParser_main(): --parseTable: END: commandString = %s \n",commandString.c_str());
#endif
             }
            else
             {
            // This skips over commands line arguments that begin with "--" (this does not appears to be meaningful).
               if (strncmp(argv[i], "--", 2) == 0) 
                  {
                // skip args that are not files
                   i += 1;
                   continue;
                  }
                 else
                  {
                 // This only skips over the options that begin with "-I" but not "-I <path>" (where the "-I" and the path are seperated by a space).
                    if (strncmp(argv[i], "-I", 2) == 0)
                       {
                      // Skip the include dir stuff; it's handled by the lexer.
                      // TODO - not currently true, so skip arg for now? 
                         i += 1;
                         continue;
                       }
                      else
                       {
                      // All other options are ignored.
                      // commandString += argv[i];
#if 0
                         printf ("In experimental_openFortranParser_main(): ignoring -- argv[i = %d] = %s \n",i,argv[i]);
#endif
                       }
                  }
             }
#if 0
          printf ("In experimental_openFortranParser_main(): end of loop over args: commandString = %s \n",commandString.c_str());
#endif
        }

  // string filename = argv[argc-1];
     string filenameWithPath    = argv[argc-1];

#if 1
     printf ("In experimental_openFortranParser_main(): filenameWithPath = %s \n",filenameWithPath.c_str());
#endif

     string filenameWithoutPath = StringUtility::stripPathFromFileName(filenameWithPath);

#if 1
     printf ("In experimental_openFortranParser_main(): filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
     printf ("In experimental_openFortranParser_main(): commandString = %s \n",commandString.c_str());
#endif

     commandString += " -i ";
     commandString += filenameWithPath;

  // Add pipe to transform/simplify OFP's ATerm 
     commandString += " | ";
     commandString += "/home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.4.7-dq-edg49-fortran-rc-aterm/src/3rdPartyLibraries/experimental-fortran-parser/stratego_transformations/";
     commandString += "ofp-simplify";

     commandString += " -o ";
     commandString += filenameWithoutPath;
     commandString += ".aterm";

#if 0
     printf ("filenameWithPath    = %s \n",filenameWithPath.c_str());
     printf ("filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
#endif

  // make sure there is a parse table
  // if (parse_table[0] == '\0')
     if (parse_table.empty() == true)
        {
          fprintf(stderr, "fortran_parser: no parse table provided, use option --parseTable\n");
          return -1;
        }

  // parse the file
  // sprintf(cmd, "sglri -p %s -i %s -o %s.ptree", parse_table, argv[i], argv[i]);

#if 1
     printf ("In experimental_openFortranParser_main(): commandString = %s \n",commandString.c_str());
#endif

  // err = system(cmd);
     err = system(commandString.c_str());

     if (err)
        {
          fprintf(stderr, "fortran_parser: error parsing file %s\n", argv[i]);
          return err;
        }

  // At this point we have a valid aterm file in the working (current) directory.
  // We have to read that aterm file and generate an uninterpreted AST, then iterate
  // on the uninterpreted AST to resolve types, disambiguate function calls and 
  // array references, etc.; until we have a correctly formed AST.  These operations
  // will be seperate passes over the AST which should build a simpler frontend to
  // use as a basis for fortran research and also permit a better design for the
  // frontend to maintain and develop cooperatively with community support.

  // *****************************************************
  // Put the call to the new SDF Open Fortran Parser here.
  // *****************************************************


// Code copied from OFP's traverse.cpp file (takes an ATERM file on the command line).
#if 0
   for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "-i") {
         file = fopen(argv[++i], "r");
         if (file == NULL) {
            fprintf(stderr, "Failed to open file\n");
            exit(-1);
         }
      }
      else if (arg == "-o") {
         ofs = new std::ofstream(argv[++i], std::ios_base::out);
      }
   }
#endif

   printf ("Test 1 \n");

   ofs = NULL;
   if (ofs) unparser = new OFP::FortranTextUnparser(*ofs);
   else     unparser = new OFP::FortranTextUnparser();

   printf ("Test 2 \n");

// This is the initialization of the ATERM library
   ATinitialize(argc, argv);

   printf ("Test 3 \n");

   if (ofp_traverse_init() != ATtrue) exit(1);

   printf ("Test 4 \n");

   filenameWithoutPath += ".aterm";

   printf ("Test 4.2 filenameWithoutPath = %s \n",filenameWithoutPath.c_str());

// This reads the ATERM that was created by the parser.
   file = fopen(filenameWithoutPath.c_str(), "r");

   printf ("Test 4.5 filenameWithoutPath = %s \n",filenameWithoutPath.c_str());

   Program.term = ATreadFromTextFile(file);

#define OFP_DEBUG

   printf ("Test 4.6 \n");

#ifdef OFP_DEBUG
   printf("\n%s\n\n", ATwriteToString(Program.term));
#endif

   printf ("Test 5 \n");

   OFP::setASTBuilder(&ast);
   OFP::setUnparser(unparser);

   printf ("Test 6 \n");

   if (ofp_traverse_Program(Program.term, &Program)) {
      if (ofs == NULL) printf("\nWoot!\n");
   } else return 1;

   if (ofs == NULL) printf("----------------------------\n");
   unparser->unparseNode(Program.getPayload());
   if (ofs == NULL) printf("----------------------------\n\n");

   delete unparser;
   if (ofs) delete ofs;

   return 0;
}

// DQ (9/10/2014): This is the older version of the OFP support (no longer used).
int
OLD_experimental_openFortranParser_main(int argc, char **argv)
   {
  // This function does not need to call the JVM.

  // Put the call the the new SDF Open Fortran Parser here.

     printf ("In experimental_openFortranParser_main(): Put the call the the new SDF Open Fortran Parser here... \n");

     int i, err;
     char cmd[256], parse_table[128];

     parse_table[0] = '\0';

     if (argc < 2) 
        {
          printf("usage: fortran_parser --parseTable parse_table_path filename(s)\n");
          return 1;
        }

  // Parse each filename (args not associated with "--parseTable", "--" or "-I")
     for (i = 1; i < argc; i++)
        {
          printf ("In experimental_openFortranParser_main(): i = %d argv[%d] = %s \n",i,i,argv[i]);

          if (strncmp(argv[i], "--parseTable", 12) == 0) 
             {
               printf ("In experimental_openFortranParser_main(): argv[i+1 = %d] = %s \n",i+1,argv[i+1]);

               sprintf(parse_table, "%s", argv[i+1]);
               i += 1;
               continue;
             }
            else
             {
               if (strncmp(argv[i], "--", 2) == 0) 
                  {
                // skip args that are not files
                   i += 1;
                   continue;
                  }
                 else
                  {
                    if (strncmp(argv[i], "-I", 2) == 0)
                       {
                      // Skip the include dir stuff; it's handled by the lexer.
                      // TODO - not currently true, so skip arg for now? 
                         i += 1;
                         continue;
                       }
                  }
             }

       // make sure there is a parse table
          if (parse_table[0] == '\0')
             {
               fprintf(stderr, "fortran_parser: no parse table provided, use option --parseTable\n");
               return -1;
             }

       // parse the file
          sprintf(cmd, "sglri -p %s -i %s -o %s.ptree", parse_table, argv[i], argv[i]);

          printf ("In experimental_openFortranParser_main(): cmd = %s \n",cmd);

          err = system(cmd);
          if (err)
             {
               fprintf(stderr, "fortran_parser: error parsing file %s\n", argv[i]);
               return err;
             }
        }

     return 0;
   }

