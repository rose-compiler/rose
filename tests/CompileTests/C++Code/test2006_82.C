// glob.h depends on dirent.h so once this is handled correctly the 
// problem in ROSE disappears.  However it should not be required.
// We likely need to have policy for how to handle class declarations that 
// never have a matching defining declaration.  Even if it would not link
// it should still compile!

// #include <dirent.h>
#include <glob.h>
