#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;

// This program demonstrates the required mechanism for processing the input ASCII file
// within preprocessors built by ROSE.  If it detects illeagal characters in the input
// file then it generates a new file as a backup and then overwrites the original file.
// This could be dangerous, so we might want to attempt an alternative approach, later.

// The default behaviour within ROSE preprocessors could be to just quit if illegal
// characters are found in the input code. But this is not a great way to handle
// the expansion of tabs (tabs are not illegal characters, just annoying ones).

// NOTE: We don't want to have the default behaviour be to overwrite the user's 
//       input file!!!

#include "stdio.h"
#include "stdlib.h"

#include <string>
#include <iostream>
#include <fstream>

#define NEW_LINE '\n'
#define HORIZONTAL_TAB '\t'
#define VERTICAL_TAB '\v'
#define BACK_SPACE '\b'
#define CARRIAGE_RETURN '\r'
#define FORM_FEED '\f'

// Bell (C++ calls this "alert")
#define ALERT '\a'

// Convert all tabs into spaces (make this a user specified value within ROSE)
#define HORIZONTAL_TAB_LENGTH 3

bool
fileContainsProblemCharaters ( string filename, bool ignoreTabs )
   {
  // This function looks for characters that can appear in ASCII files, but which are
  // a problem to the EDG parser (e.g. Carriage Returns) or tabs which would be a 
  // problem to the accurate unparsing of the final code.

  // printf ("Input filename = %s \n",filename.c_str());

     std::ifstream from(filename.c_str());
     if (!from)
        {
          printf ("Could not open %s \n",filename.c_str());
          abort();
        }

     bool foundNewLine        = false;
     bool foundHorizontalTab  = false;
     bool foundVerticalTab    = false;
     bool foundBackSpace      = false;
     bool foundCarriageReturn = false;
     bool foundFormFeed       = false;
     bool foundBell           = false;

     char ch;
     while (from.get(ch))
        {
          switch (ch)
             {
               case NEW_LINE:
                 // printf ("found a NEW_LINE \n");
                    foundNewLine = true;
                    break;
               case HORIZONTAL_TAB:
                 // printf ("found a HORIZONTAL_TAB \n");
                    foundHorizontalTab = true;
                    break;
               case VERTICAL_TAB:
                    printf ("found a VERTICAL_TAB \n");
                    foundVerticalTab = true;
                    break;
               case BACK_SPACE:
                    printf ("found a BACK_SPACE \n");
                    foundBackSpace = true;
                    break;
               case CARRIAGE_RETURN:
                    printf ("found a CARRIAGE_RETURN \n");
                    foundCarriageReturn = true;
                    break;
               case FORM_FEED:
                    printf ("found a FORM_FEED \n");
                    foundFormFeed = true;
                    break;
               case ALERT:
                    printf ("found a ALERT (bell) \n");
                    foundBell = true;
                    break;
             }
        }

  // Inputparameter permits ignoring horizontal tabs
     if (ignoreTabs)
        foundHorizontalTab = false;

     return foundCarriageReturn || foundHorizontalTab || foundVerticalTab || foundBackSpace || foundFormFeed || foundBell;
   }

void
filterInputFile ( string filename, string outputFilename )
   {
  // This function filters the input file to remove ^M characters and expand tabs etc.
  // Any possible processing of the input file, before being compiled, should be done
  // by this function.

  // printf ("Input filename = %s \n",filename.c_str());

     std::ifstream from(filename.c_str());
     if (!from)
        {
          printf ("Could not open %s \n",filename.c_str());
          abort();
        }

     std::ofstream to (outputFilename.c_str());
     if (!to)
        {
          printf ("Could not open %s \n",outputFilename.c_str());
          abort();
        }

     char ch;
     while (from.get(ch))
        {
          switch (ch)
             {
               case NEW_LINE:
                 // printf ("found a NEW_LINE \n");
                    to.put(ch);
                    break;
               case HORIZONTAL_TAB:
                 // printf ("found a HORIZONTAL_TAB \n");
                    for (int i=0; i < HORIZONTAL_TAB_LENGTH; i++)
                         to.put(' ');
                    break;
               case VERTICAL_TAB:
                 // printf ("found a VERTICAL_TAB \n");
                    break;
               case BACK_SPACE:
                 // printf ("found a BACK_SPACE \n");
                    break;
               case CARRIAGE_RETURN:
                 // printf ("found a CARRIAGE_RETURN \n");
                    break;
               case FORM_FEED:
                 // printf ("found a FORM_FEED \n");
                    break;

               default:
                   to.put(ch);
             }
        }
   }

int main ()
   {
     string inputFile  = "testFileFilter.C";
     string outputFile = "filtered_testFileFilter.C";

     bool ignoreTabs = true;
     if (fileContainsProblemCharaters(inputFile,ignoreTabs) == true)
     {
        printf ("Problems detected in input file (rebuilding input file) \n");
     // abort();

     // filter the problem file and rebuild it as a new file
     // filename policy:
     //   1) copy the input file to a new file,
     //   2) save the new file as a backup file,
     //   3) once finished with step 2, overwrite the 
     //      existing file (to exclude problem characters).

        filterInputFile (inputFile,outputFile);
     }     

     return 0;
   }

