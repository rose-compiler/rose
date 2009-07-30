/* Code addition in the file containing main() */

#if BLCR_CHECKPOINTING
// checkpointing code
#include "libcr.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libcr.h"
#ifndef O_LARGEFILE
  #define O_LARGEFILE 0
#endif

// client handle used across source files
cr_client_id_t cr;

#endif

int
main( int   argc,
      char *argv[] )
{
#if BLCR_CHECKPOINTING
  //initialize the blcr environment
  cr= cr_init();
  cri_info_init(); 
#endif

//.....
}
