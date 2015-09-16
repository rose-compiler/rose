#include "sage3basic.h"
#include "SourceCodeFormat.h"

SourceCodeFormat::SourceCodeFormat():defaultLineLength(255),defaultIndentation(2) {
}

SourceCodeFormat::~SourceCodeFormat() {
}

// return: > 0: start new lines; == 0: use same line; < 0:default
int
SourceCodeFormat::getLine( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt)   {
  // Use default mechanism to select the line where to output generated code
  return -1;
}

// return starting column. if < 0, use default
int
SourceCodeFormat::getCol( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt) {
  // Use default mechanism to select the column where to output generated code
  return -1;
}

int
SourceCodeFormat::tabIndent() {
  return defaultIndentation;
}

int
SourceCodeFormat::maxLineLength() {
  return defaultLineLength;
}
