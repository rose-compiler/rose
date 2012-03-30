//#include <iostream>
//#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[]) {
    //ROSE_ASSERT(argc == 2);
    char* argu = argv[1];
   
    int t;
    int y = 2;
    //int i;
    int i = atoi(argu);
    if (i > 2) {
        i = 3;
        //y = 3;
    }
    else {
        i = 2;
    }
    if (y == 2) {
        t = 1;
    }
    else {
    t = 0;
    }
    return t;
}
