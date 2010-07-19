#include <stdio.h>

//
// Test switch 5
//
int main(int argc, char *argv[]) {
    for (int i = 0; i < 16; i++) {
        switch(i) {
             default:
                if (i < 8)
                    case 2: case 3: case 5: case 7:
                        printf("< 8 or = 2, 3, 5 or 7\n");
                else
                    case 4: case 6: case 8: case 9: case 10:
                        printf("4, 6, 8, 9, 10 or > 10\n");
        }
    }
}
