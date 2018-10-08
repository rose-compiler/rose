#include <iostream>
#include <cstdlib>

int main() {
    unsigned int *ptr = new unsigned int[10];
    std::cout << "Test case " << *ptr << std::endl;
    free(ptr);
    return 0;
}
