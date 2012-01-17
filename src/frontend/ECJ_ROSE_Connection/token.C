#include <string>
#include "token.h"

Token_t::Token_t (std::string s, JavaSourceCodePosition * p)
   : text(s), pos_info(p)
   {
  // Nothing to do here!
   }

std::string
Token_t::getText()
   {
     return text;
   }

JavaSourceCodePosition *
Token_t::getSourcecodePosition()
   {
         return pos_info;
   }
