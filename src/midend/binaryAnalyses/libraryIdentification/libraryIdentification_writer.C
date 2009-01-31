
// Support for writing a FLIRT data base from an existing AST.

#include <rose.h>

#include <libraryIdentification.h>

// Use the MD5 implementation that is in Linux.
#include <openssl/md5.h>

// Function prototype looks like:
// unsigned char *MD5(const unsigned char *d, unsigned long n, unsigned char *md);

using namespace std;

void
LibraryIdentification::generateLibraryIdentificationDataBase( string databaseName, SgProject* project )
   {
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);

     printf ("Building LibraryIdentification database: %s from AST of project: %p \n",project,databaseName.c_str());
   }
