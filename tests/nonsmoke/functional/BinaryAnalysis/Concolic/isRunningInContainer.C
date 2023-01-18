// Used to conditionally disable tests that fail in containers
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <rose.h>
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <iostream>

int main() {
    if (Rose::BinaryAnalysis::Concolic::isRunningInContainer()) {
        std::cout <<"this test cannot run in a container\n";
    }
}

#else

#include <iostream>

int main() {
    std::cout <<"concolic testing is disabled in this configuration\n";
}

#endif
