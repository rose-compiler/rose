#include <string>
#include "token.h"

Token_t::Token_t (std::string filename_, JavaSourceCodePosition * p) : filename(filename_), pos_info(p) {
    // Nothing to do here!
}

std::string Token_t::getFileName() {
    return filename;
}

JavaSourceCodePosition *Token_t::getSourcecodePosition() {
    return pos_info;
}
