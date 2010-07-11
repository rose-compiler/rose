#include <stdio.h>

//
// Test switch 0
//
int main(int argc, char *argv[]) {
    for (int i = 'a'; i < 'i'; i++) {
        switch(i) {
            case 'a':
                  if (i == 1) // never true
                    printf("*");
            case 'b': printf("**");
            case 'c': case 'd': case 'e': printf("***");
            case 'f': case 'g': printf("****");
            default: printf(" -- Nothing !");
            case 'z': ;
        }
        printf("\n");
    }

    return 0;
}
