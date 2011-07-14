#include <iostream>

#include "ClangFrontend.h"

#include "DotConsumer.h"

int main(int argc, char ** argv) {

    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: \'ClangToDot file-in [file-out]\'" << std::endl;
        std::cerr << "\tIf none given, the output file will be \'file-in\'.dot" << std::endl;
        exit(1);
    }

    std::string in_file(argv[1]);

    // TODO check if 'in_file' exists

    Language language = unknown;

    size_t last_period = in_file.find_last_of(".");
    std::string extention(in_file.substr(last_period + 1));

    std::cerr << "extention: \"" << extention << "\"" << std::endl;

    if (extention == "c") {
        language = C;
    }
    else if (extention == "C" || extention == "cxx" || extention == "cpp") {
        language = CPLUSPLUS;
    }
    else if (extention == "objc") {
        language = OBJC;
    }
    else if (extention == "cu") {
        language = CUDA;
    }
    else if (extention == "ocl" || extention == "cl") {
        language = OPENCL;
    }

    std::string out_file;
    if (argc == 3)
        out_file = std::string(argv[2]);
    else
        out_file = in_file + ".dot";

    DotConsumer::generateDot(in_file, out_file, language);

    return 0;
}

