#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "fortran_support.h"

SgSourceFile* OpenFortranParser_globalFilePointer = NULL;

using namespace std;

int
experimental_openFortranParser_main(int argc, char **argv)
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

