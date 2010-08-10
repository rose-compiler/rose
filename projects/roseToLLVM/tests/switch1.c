#include <stdio.h>

//
// Test switch 1
//
int main(int argc, char *argv[]) {
     int x = 1;

     switch(x) {
         case 1: printf("*");
         case 2: printf("**");
         case 3: printf("***");
         case 4: printf("****");
     }
}
