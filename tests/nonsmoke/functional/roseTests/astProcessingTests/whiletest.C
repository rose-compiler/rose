
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    //int i = atoi(;
    int i  = atoi((const char*) argv[2]);
    int j = atoi((const char*) argv[3]);
    int k = atoi((const char*) argv[4]);
    int kmk = 2-k;
    int kpk = 2+k;
    int ipj = i + j;
    int imj = i - j;
    if (kmk == ipj) {
       j = j + 1;
    }
    else if (imj == kpk) {
       j = j - 1;
    }
    else {
      j = j + 10;
    }
    k = k + 1;
    int x = 0;

    while (j < 5) {
        i = i + 2;
	j = j + 1;
        
       // i = i i+ 1;
    }
    int ik = i + k;
    int imk = i - k;
    if (i > 15) {
        i = i + 1;;
    }
    if (i < 15) {
        if (true) {
            i = i + 1;;
        }
        else {
            i = i + 2;
        }
    }
    else {
        i = i - 1;
    }

    return 0;
}
