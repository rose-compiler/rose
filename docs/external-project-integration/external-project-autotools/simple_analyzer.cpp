// Simple ROSE-based tool.
//
// This tool assumes that ROSE was compiled without any analysis support (neither C/C++, binary, nor anything else).

#include <rose.h>
#include <iostream>

int main(int argc, char* argv[]) {
    ROSE_INITIALIZE;
    std::cout << "Simple ROSE Analyzer" << std::endl;
    std::cout << "ROSE Version: " << ROSE_PACKAGE_VERSION << std::endl;
    std::cout << "====================" << std::endl;
}
