// Normalization of string literals reported by Philippe Charles.

extern int printf(const char *str, ...);

static char string2[] = "bbbb\0bbbb"; // Gets converted to:  "bbbb\000bbbb";

int main(int argc, char **argv) {
    printf("%s !!!\n", string2);
}
