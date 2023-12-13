#include <rose.h>
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

using namespace Rose::BinaryAnalysis;

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    extern void assemble();
    assemble();
}
