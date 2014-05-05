#include "rose.h"

int main(int argc, char *argv[]) {
    return frontend(argc, argv) == NULL ? 1 : 0;
}
