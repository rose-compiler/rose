// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=4:tabstop=4:textwidth=80

//Your test file code goes here.

#include <stdlib.h>

int main(int argc, char *argv[]) {
    char* avar        = "HOME=/usr/home";
    int ret;

    /* This is the incorrect method of calling putenv() where it is passed an
     * automatic variable */
    ret = putenv(avar);

    return ret;
}
