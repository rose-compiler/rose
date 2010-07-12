#include <stdio.h>

//
// Test switch 4
//
int main(int argc, char *argv[]) {
    for (int i = 0; i < 16; i++) {
        switch(i) {
            case 1:
                 printf("The number is 1\n");
                 break;
            case 2:
                 printf("The number is 2\n");
                 break;
            case 3:
                 printf("The number is 3\n");
                 break;
            case 4:
                 printf("The number is 4\n");
                 break;
            case 5:
                 printf("The number is 5\n");
                 break;
            case 6:
                 printf("The number is 6\n");
                 break;
            case 7:
                 printf("The number is 7\n");
                 break;
            case 8:
                 printf("The number is 8\n");
                 break;
            case 9:
                 printf("The number is 9\n");
                 break;
            case 10:
                 printf("The number is 10\n");
                 break;
            default:
                 printf("Out of range\n");
        }
    }
}
