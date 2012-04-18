#include "yicesParserLib.h"
//#include <rose.h>
//#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "yices check" << std::endl;
    int i = yicesCheck(argc, argv);
    std::cout << "end yices check" << std::endl;
}
