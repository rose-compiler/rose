#include <iostream>
#include <sstream>
#include <string>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

using namespace std;

#if __sun
  // PP (16/05/19) added: https://www.perkin.org.uk/posts/solaris-portability-flock.html
  // This is NOT WORKING, but gets us through compiling ROSE on Solaris
  
  #ifdef LOCK_EX
  #error Solaris extra code may not be required
  #endif

#define   LOCK_SH   1    /* shared lock */
#define   LOCK_EX   2    /* exclusive lock */
#define   LOCK_NB   4    /* don't block when locking */
#define   LOCK_UN   8    /* unlock */


int flock(int fd, int op) 
{
  int rc = 0;
#if 0
#if defined(F_SETLK) && defined(F_SETLKW)
  struct flock fl = {0};

  switch (op & (LOCK_EX|LOCK_SH|LOCK_UN)) {
  case LOCK_EX:
    fl.l_type = F_WRLCK;
    break;

  case LOCK_SH:
    fl.l_type = F_RDLCK;
    break;

  case LOCK_UN:
    fl.l_type = F_UNLCK;
    break;

  default:
    errno = EINVAL;
    return -1;
  }

  fl.l_whence = SEEK_SET;
  rc = fcntl(fd, op & LOCK_NB ? F_SETLK : F_SETLKW, &fl);

  if (rc && (errno == EAGAIN))
    errno = EWOULDBLOCK;
#endif
#endif
  return rc;
}
#endif /* __sun */


namespace Rose {
    // This API takes a rose database file, typically specified via -rose:projectSpecificDatabaseFile commandline
    // and a physical file. If the physical file is already recorded in the database file, the corresponding id
    // is returned, otehrwise, a new entry is made into the database file and the new id is returned.
    // This API is thread safe.
    uint32_t GetProjectWideUniqueIdForPhysicalFile(const string & dbFile, const string & physicalFile) {
        int fd = open(dbFile.c_str(), O_RDWR | O_CREAT , O_SYNC | S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP );
        if (fd == -1) {
            std::cout<<"\n Failed to open the file:" << dbFile;
            exit(-1);
        }
        if(flock(fd,LOCK_EX) == -1) {
            cout<<"\n Failed to acquire lock on the file:" << dbFile;
            close(fd);
            exit(-1);
        }
        
        FILE * fptr = fdopen(fd, "r+");
        if(fptr == NULL){
            std::cout<<"\n Failed to fdopen file id:" << fd;
            flock(fd, LOCK_UN);
            close(fd);
            exit(-1);
        }
        
        char line [PATH_MAX];
        uint32_t index= 0;
        while(fgets(line,PATH_MAX,fptr) != NULL){
            // chop the trailing \n
            line[strlen(line) - 1] = '\0';
            if(physicalFile == string(line)) {
                flock(fd, LOCK_UN);
                fclose(fptr);
                return index;
            }
            index++;
        }
        // Add the new line
        if(fputs((physicalFile + "\n").c_str(),fptr) < 0 ){
            std::cout<<"\n Failed to fputs file:" << dbFile;
            flock(fd, LOCK_UN);
            close(fd);
            exit(-1);
        }
        
        flock(fd, LOCK_UN);
        fclose(fptr);
        return index;
        
    }
    
}

