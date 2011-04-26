#include <string>
#include "token.h"

Token_t::Token_t (std::string s, int line, int col )
   : text(s), line_number(line), column_number(col)
   {
  // Nothing to do here!
   }

std::string
Token_t::getText()
   {
     return text;
   }

int
Token_t::getLine()
   {
     return line_number;
   }

int
Token_t::getCol()
   {
     return column_number;
   }



