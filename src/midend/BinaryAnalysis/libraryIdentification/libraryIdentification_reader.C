// Support for reading a binary and comparing its functions to a database.
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"                                 // every librose .C file must start with this

#include <libraryIdentification.h>

// Use the MD5 implementation that is in Linux.
#include <openssl/md5.h>

// Function prototype looks like:
// unsigned char *MD5(const unsigned char *d, unsigned long n, unsigned char *md);

using namespace std;

/**  generateLibraryIdentificationDataBase
 *  
 *  Attempts to match the functions in the database against an
 *  existing binary function identification database with
 *  the functions in the project.  Whether a match is found or not, a
 *  message is simply printed to stdout.
 *  Just calls libraryIdentificationDataBaseSupport internally
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] project      Rose SgProject that has the functions to
 * write or find
 **/
void
LibraryIdentification::matchAgainstLibraryIdentificationDataBase( string databaseName, SgProject* project )
   {
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);

     printf ("Going to process AST of project %p to recognize functions from Library Identification database: %s \n",project,databaseName.c_str());

     libraryIdentificationDataBaseSupport( databaseName, project, /* generate_database */ false);
   }

#endif
