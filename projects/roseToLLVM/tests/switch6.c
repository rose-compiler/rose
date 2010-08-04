#include <stdio.h>

//
// Test switch 6
//
int prime(int x) {
    if (x == 2)
        return 1;
    if (x == 3)
        return 1;
    if (x == 5)
        return 1;
    if (x == 7)
        return 1;
    return 0;
}

void process_prime(int x) {
    printf("%i is a prime\n", x);
}

void process_composite(int x) {
    printf("%i is a composite\n", x);
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < 10; i++) {
        int x = i + 1;
        switch(x) {
             default:
                if (prime(x))
                    case 2: case 3: case 5: case 7:
                        process_prime(x);
                else
                    case 4: case 6: case 8: case 9: case 10:
                        process_composite(x);
        }
    }
}
