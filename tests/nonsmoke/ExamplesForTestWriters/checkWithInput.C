// A stupid example that supposedly reads one input specimen, checks some ROSE functionality, produces some standard output
// which is later checked against a known good answer, and then exits.

#include <iostream>

int
main(int argc, char *argv[]) {

    // Pretend to read the specimen and do some analysis on it. In actuality we just check that we were given some argument.
    if (argc != 2) {
        std::cerr <<"must be called with exactly one argument\n";
        return 1;
    }

    // Pretend to produce some output that will be checked later against an answer file
    std::cout <<"3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068\n";
}
