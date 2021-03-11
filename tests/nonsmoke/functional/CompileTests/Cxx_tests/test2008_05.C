#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>

#if ( defined(__clang__) == 0 && (__GNUC_MINOR__ < 4) && !defined(LIE_ABOUT_GNU_VERSION_TO_EDG) )
#pragma STDC FENV_ACCESS ON
#endif

/* Returns nonzero only iff filename is in user's home directory */
int verify_file(char* const filename) {
  /* Get /etc/passwd entry for current user */
  struct passwd *pwd = getpwuid(getuid());
  if (pwd == NULL) {
    /* handle error */
    return 0;
  }

  const unsigned int len = strlen( pwd->pw_dir);
  if (strncmp( filename, pwd->pw_dir, len) != 0) {
    return 0;
  }
  /* Make sure there is only one '/', immediately after homedir */
  if (strrchr( filename, '/') == filename + len) {
    return 1;
  }
  return 0;
}


int main(int argc, char** argv) {
  if (verify_file( argv[1])) {
    printf("file good\n");
  }
  else {printf("file not good\n");}
  return 0;
}
