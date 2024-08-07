// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparseFormatHelp.h"

UnparseFormatHelp::~UnparseFormatHelp()
   {}

// return: > 0: start new lines; == 0: use same line; < 0:default
int
UnparseFormatHelp::getLine( SgLocatedNode*, SgUnparse_Info&, FormatOpt)
   {
     return -1;
   }

// return starting column. if < 0, use default
int
UnparseFormatHelp::getCol( SgLocatedNode*, SgUnparse_Info&, FormatOpt)
   {
     return -1;
   }

int
UnparseFormatHelp::tabIndent()
   {
     return TABINDENT;
   }

int
UnparseFormatHelp::maxLineLength()
   {
     return MAXINDENT;
   }

