#ifndef __token_h__
#define __token_h__

#include "JavaSourceCodePosition.h"

#include <string>

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
