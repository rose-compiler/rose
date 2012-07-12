#ifndef __token_h__
#define __token_h__

#include "JavaSourceCodePosition.h"

class Token_t {
    private:
        std::string text;
        JavaSourceCodePosition * pos_info;
        // I don't think I need the token type.
        // int type;
    public:
        Token_t (std::string s, JavaSourceCodePosition * pos_info);
        JavaSourceCodePosition * getSourcecodePosition();
        std::string getText();
};
  
#endif
