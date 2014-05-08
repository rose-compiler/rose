#ifndef __x10_token_h__
#define __x10_token_h__

#include "x10SourceCodePosition.h"
#include "JavaSourceCodePosition.h"

#if 0
class Token_t {
    private:
        std::string filename;
        JavaSourceCodePosition *pos_info;
    public:
        Token_t (std::string filename, JavaSourceCodePosition *pos_info);
        JavaSourceCodePosition *getSourcecodePosition();
        std::string getFileName();
};
#endif

class X10_Token_t {
    private:
        std::string text;
        X10SourceCodePosition * pos_info;
        // I don't think I need the token type.
        // int type;
    public:
        X10_Token_t (std::string s, X10SourceCodePosition * pos_info);
        X10SourceCodePosition * getSourcecodePosition();
        std::string getText();
        std::string getFileName();
};
  
#endif
