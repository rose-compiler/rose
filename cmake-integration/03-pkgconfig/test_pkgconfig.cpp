#include <rose.h>
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "pkg-config ROSE integration test" << std::endl;
    std::cout << "ROSE version: " << ROSE_PACKAGE_VERSION << std::endl;
    std::cout << "SUCCESS: ROSE linked via pkg-config" << std::endl;
    return 0;
}
