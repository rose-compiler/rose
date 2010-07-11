#include <stdio.h>

//
// Test switch 3
//
enum { fatal, error };
int main(int argc, char *argv[]) {
    int x = fatal,
        error_count = 0;

     switch(x) {
         case fatal: printf("Fatal ");
                     /* Drops Through */
         case error: printf("Error");
                     ++error_count;
                     break;
     }
}
