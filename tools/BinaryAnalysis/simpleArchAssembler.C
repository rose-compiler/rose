#include <rose.h>
#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

using namespace Rose::BinaryAnalysis;

int main() {
    ROSE_INITIALIZE;
    extern void assemble();
    assemble();
}
