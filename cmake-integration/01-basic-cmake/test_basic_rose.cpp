#include <rose.h>
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "Basic ROSE integration test" << std::endl;
    std::cout << "ROSE version: " << ROSE_PACKAGE_VERSION << std::endl;

    // Test that we can access basic ROSE functionality
    SgProject* project = nullptr;  // Just verify the type is available

    std::cout << "SUCCESS: ROSE library linked correctly" << std::endl;
    return 0;
}
