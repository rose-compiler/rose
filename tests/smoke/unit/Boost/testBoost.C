#include "rose_getline.h"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <boost/version.hpp>
#include <iostream>

int
main(int argc, char *argv[])
{
    size_t nchecked=0, nerrors=0;
    
    // What version of boost headers was I compiled with?
    char hdr_version[64];
    sprintf(hdr_version, "%d.%d.%d", BOOST_VERSION/100000, (BOOST_VERSION/100)%1000, BOOST_VERSION%100);

    // What version of boost libraries was I compiled with?  We want to check every boost library individually, so we look at
    // our memory map rather than calling a particular boost library function.
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f)
        return 0;
    char *line = NULL;
    size_t linesz = 0;
    ssize_t n;
    while ((n=rose_getline(&line, &linesz, f))>0) {
        while (n>0 && isspace(line[n-1]))
            line[--n]='\0';
        char *basename = strrchr(line, '/');
        basename = basename ? basename+1 : line;
        if (strncmp(basename, "libboost", 8))
            continue; 
        char *so = strstr(basename, ".so.");
        if (!so)
            continue;
        char *lib_ver = so+4;

        ++nchecked;
        if (strcmp(hdr_version, lib_ver)) {
            std::cerr <<argv[0] <<": error: compiled with boost " <<hdr_version <<" but linked with " <<basename <<"\n"
                      <<"    " <<line <<"\n";
            ++nerrors;
        }
    }
    fclose(f);
    if (line)
        free(line);

    std::cout <<"checked " <<nchecked <<" boost " <<(1==nchecked?"library":"libraries")
              <<" and found " <<nerrors <<(1==nerrors?" error":" errors") <<"\n";
    return nerrors ? 1 : 0;
}

