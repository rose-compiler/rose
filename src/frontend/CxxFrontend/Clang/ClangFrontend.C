#include <iostream>

#include "ClangFrontend.h"

#include "DotConsumer.h"
#include "DotVisitor.h"

int clang_main(int argc, char* argv[], SgSourceFile& sageFile) {

    std::cout << "Enter clang_main..." << std::endl;

//    for (int i = 0; i < argc; i++)
//	std::cout << "\targv[" << i << "] = " << argv[i] << std::endl;

    std::string in_file(argv[argc-1]);

    generateDot(in_file, "output.dot");

    std::cout << "Leave clang_main." << std::endl;
    return 1;
}
