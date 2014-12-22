/**
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract DE-
 * AC52-06NA25396 for Los Alamos National Laboratory (LANL), which is
 * operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *  
 * Additionally, this program and the accompanying materials are made
 * available under the terms of the Eclipse Public License v1.0 which
 * accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */

/* Based on examples/docs from:
 *      http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/invocation.html#wp9502
 * http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/jniTOC.html
 * http://java.sun.com/docs/books/jni/html/invoke.html 
 */
#include "sage3basic.h"

#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "rose_config.h"
#include "rose_paths.h"
#include "commandline_processing.h"
#include "assert.h"
#include "fortran_error_handler.h"
#include "ofp.h"

/* This is defined if ROSE is configured to use the Java Open Fortran Parser */
#ifndef USE_ROSE_JAVA_SUPPORT
#error "openFortranParser_main.C should not be compiled when Java support is disabled"
#endif


using namespace std;

// DQ (9/6/2010): Allow this to be commented out to simplify debugging using gdb.
// #define ENABLE_FORTRAN_ERROR_HANDLER


/* This is critical to permitting the correct library (the one just built by the user)
// to be used instead of requiring the LD_LIBRARY_PATH to beexplicitly set by the user
// before running any tests.  It is alos important to proper testing using "make distcheck"
// since we want the new library built using that rule to be tested over any other.
*/
#define OVERWRITE_LD_LIBRARY_PATH 1


int openFortranParser_main(int argc, char **argv)
  {
 /* To use different versions of the LD_LIBRARY_PATH, get the value, 
    change it to the path in the build tree, and then reset it to the 
    old value after the call to JVM.  We need to do this because the
    libparser_java_FortranParserActionJNI.so that we need is in the 
    build tree and it is specific to the configuration of ROSE (else 
    V_<class name> enum values will be different as a result of 
    configuration options that might trigger different numbers of IR 
    nodes to be use.
  */

 /* Overwite to a new value. It is not clear when to use the install path and when to use the build path! */
    #ifdef USE_CMAKE
    string new_value = findRoseSupportPathFromBuild("lib", "lib");
    #else
    string new_value = findRoseSupportPathFromBuild("src/frontend/OpenFortranParser_SAGE_Connection/.libs", "lib");
    #endif

 /* Save the old value */
    const char* old_value = getenv(ROSE_SHLIBPATH_VAR); // Might be null

#if OVERWRITE_LD_LIBRARY_PATH
    int overwrite = 1;
    int env_status = setenv(ROSE_SHLIBPATH_VAR,new_value.c_str(),overwrite);
    assert(env_status == 0);
#endif

    if (SgProject::get_verbose() > 0)
       {
         printf ("Call the function that will start a JVM and call the OFP \n\n");
         string JVM_command_line = CommandlineProcessing::generateStringFromArgList(CommandlineProcessing::generateArgListFromArgcArgv(argc, argv));
         printf ("Java JVM commandline = %s \n",JVM_command_line.c_str());
         printf ("ROSE modified %s = %s \n",ROSE_SHLIBPATH_VAR,new_value.c_str());
       }

#ifdef ENABLE_FORTRAN_ERROR_HANDLER
    fortran_error_handler_begin();
#endif

    int status = Rose::Frontend::Fortran::Ofp::jvm_ofp_processing(argc, argv);

#ifdef ENABLE_FORTRAN_ERROR_HANDLER
    fortran_error_handler_end();
#endif

    if (SgProject::get_verbose() > 0)
       {
         printf ("JVM processing done.\n\n");
       }

 /* Reset to the saved value */
#if OVERWRITE_LD_LIBRARY_PATH
 // DQ (9/12/2011): Note that old_value can be NULL and if so then we don't want it to be dereferenced.
 // env_status = setenv(ROSE_SHLIBPATH_VAR,old_value,overwrite);
    if (old_value != NULL)
          env_status = setenv(ROSE_SHLIBPATH_VAR,old_value,overwrite);

    assert(env_status == 0);
#endif

         return status;
  }


#if 0
#if 0
int
experimental_openFortranParser_main(int argc, char **argv)
   {
  // This function does not need to call the JVM.
     int status = 0;

     printf ("ERROR: This function is defined in openFortranParser_main.C of the src/frontend/OpenFortranParser_SAGE_Connection directory (the experimental fortran frontend has not been enabled) \n");
     ROSE_ASSERT(false);

  // Put the call the the new SDF Open Fortran Parser here.

     return status;
   }
#else
int
experimental_openFortranParser_main(int argc, char **argv)
   {
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

     string commandString = "sglri ";

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

#if 0
     printf ("In experimental_openFortranParser_main(): filenameWithPath = %s \n",filenameWithPath.c_str());
#endif

     string filenameWithoutPath = StringUtility::stripPathFromFileName(filenameWithPath);

#if 0
     printf ("In experimental_openFortranParser_main(): filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
     printf ("In experimental_openFortranParser_main(): commandString = %s \n",commandString.c_str());
#endif

     commandString += " -i ";
     commandString += filenameWithPath;
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

  // ******************************************************
  // Put the call the the new SDF Open Fortran Parser here.
  // ******************************************************

     return 0;
   }
#endif
#endif

