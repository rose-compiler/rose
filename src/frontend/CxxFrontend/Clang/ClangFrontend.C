#include <iostream>

#include "ClangFrontend.h"

int clang_main(int argc, char* argv[], SgSourceFile& sageFile) {
    std::cout << "Called the clang frontend!" << std::endl;
    return 1;
}
