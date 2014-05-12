#include <string>
#include "x10_token.h"

X10_Token_t::X10_Token_t (std::string filename_, X10SourceCodePosition * p) : filename(filename_), pos_info(p) {
    // Nothing to do here!
}

std::string X10_Token_t::getFileName() {
    return filename;
}

X10SourceCodePosition *X10_Token_t::getSourcecodePosition() {
    return pos_info;
}
