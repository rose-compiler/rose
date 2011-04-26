#include <string>
#include "JavaSourceCodePosition.h"

JavaSourceCodePosition::JavaSourceCodePosition (int line_start, int line_end )
   : line_number_start(line_start), line_number_end(line_end), column_number_start(0), column_number_end(0) 
   {
  // Nothing to do here!
   }

JavaSourceCodePosition::JavaSourceCodePosition (int line_start, int line_end, int col_start, int col_end )
   : line_number_start(line_start), line_number_end(line_end), column_number_start(col_start), column_number_end(col_end) 
   {
  // Nothing to do here!
   }

int
JavaSourceCodePosition::getLineStart()
   {
     return line_number_start;
   }

int
JavaSourceCodePosition::getLineEnd()
   {
     return line_number_end;
   }

int
JavaSourceCodePosition::getColStart()
   {
     return column_number_start;
   }

int
JavaSourceCodePosition::getColEnd()
   {
     return column_number_end;
   }



