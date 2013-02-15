#include <string>
#include "x10SourceCodePosition.h"

X10SourceCodePosition::X10SourceCodePosition(int line_start, int line_end ) : line_number_start(line_start),
                                                                                line_number_end(line_end),
                                                                                column_number_start(0),
                                                                                column_number_end(0) 
{
    // Nothing to do here!
}

X10SourceCodePosition::X10SourceCodePosition(int line_start, int line_end, int col_start, int col_end) : line_number_start(line_start),
                                                                                                           line_number_end(line_end),
                                                                                                           column_number_start(col_start),
                                                                                                           column_number_end(col_end) 
{
    // Nothing to do here!
}

int X10SourceCodePosition::getLineStart() {
    return line_number_start;
}

int X10SourceCodePosition::getLineEnd() {
    return line_number_end;
}

int X10SourceCodePosition::getColStart() {
    return column_number_start;
}

int X10SourceCodePosition::getColEnd() {
    return column_number_end;
}



