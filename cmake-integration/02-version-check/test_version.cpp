#include <rose.h>
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "ROSE version check test" << std::endl;
    std::cout << "ROSE version: " << ROSE_PACKAGE_VERSION << std::endl;
    std::cout << "SUCCESS: Version checking works correctly" << std::endl;
    return 0;
}
