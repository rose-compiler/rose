
// Support for reading a binary and comparing its functions to a database.

#include <rose.h>

#include <libraryIdentification.h>

// Use the MD5 implementation that is in Linux.
#include <openssl/md5.h>

// Function prototype looks like:
// unsigned char *MD5(const unsigned char *d, unsigned long n, unsigned char *md);

using namespace std;

void
LibraryIdentification::matchAgainstLibraryIdentificationDataBase( string databaseName, SgProject* project )
   {
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);

     printf ("Going to process AST of project %p to recognize functions from Library Identification database: %s \n",project,databaseName.c_str());

     libraryIdentificationDataBaseSupport( databaseName, project, /* generate_database */ false);
   }
