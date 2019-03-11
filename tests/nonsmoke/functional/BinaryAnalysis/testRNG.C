#include "sage3basic.h"

#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "LinearCongruentialGenerator.h"
#include "integerOps.h"

static uint64_t
next_random()
{
#if 1
    static LinearCongruentialGenerator rng;
    return rng();
#else
    return rand();
#endif
}

int
main(int argc, char *argv[])
{
    size_t niters = 1000;
    size_t right_shift = 0;
    uint64_t mask = 0xffff;

    int argno=1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strncmp(argv[argno], "--total=", 8)) {
            niters = strtoul(argv[argno]+8, NULL, 0);
        } else if (!strncmp(argv[argno], "--shift=", 8)) {
            right_shift = strtoul(argv[argno]+8, NULL, 0);
        } else if (!strncmp(argv[argno], "--mask=", 7)) {
            mask = strtoul(argv[argno]+7, NULL, 0);
        } else {
            std::cerr <<argv[0] <<": unrecognized switch: " <<argv[argno] <<"\n";
            exit(1);
        }
    }

    uint64_t prev = IntegerOps::shiftRightLogical2(next_random(), right_shift) & mask;
    for (size_t i=0; i<niters; ++i) {
        uint64_t next = IntegerOps::shiftRightLogical2(next_random(), right_shift) & mask;
        std::cout <<prev <<"\t" <<next <<"\n";
        prev = next;
    }
    return 0;
}

#endif
