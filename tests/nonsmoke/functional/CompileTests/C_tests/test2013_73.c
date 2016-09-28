// We need to test that a file compiled with a reference to another directory (e.g. "path/foo.c")
// will be place back into that directory as "path/rose_foo.c" and then that the "-I" option
// is not modified to include the current directory.

// There is an example problem demonstated by the compilation of file.c in ffmpeg-1.2.
// But I don't know how to reproduce the problem as a simple test.


// #include "config.h"
// #include "file.h"
// #include "log.h"
// #include "mem.h"
/* DQ (9/14/2013): Added include for Linux */
// #include <time.h>
// struct timespec {};

// #define _XOPEN_SOURCE 600
// #define __USE_MISC
#include <fcntl.h>
#include <sys/stat.h>
