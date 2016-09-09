// A stupid example that supposedly parses one input specimen and produces some output on stdout which is later checked against
// a known good answer, and then exits.

#include <iostream>

int
main(int argc, char *argv[]) {

    // Pretend to parse the specimen and do some analysis on it. In actuality we just check that we were given some argument.
    if (argc != 2) {
        std::cerr <<"must be called with exactly one argument\n";
        return 1;
    }

    // Pretend to produce some output that will be checked later against an answer file
    std::cout <<"3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068\n";
}
