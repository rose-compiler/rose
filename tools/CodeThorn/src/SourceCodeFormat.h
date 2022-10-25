#include "unparseFormatHelp.h"

class SourceCodeFormat : public UnparseFormatHelp {
 public:
  SourceCodeFormat();
  ~SourceCodeFormat();
  
  virtual int getLine( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);
  virtual int getCol ( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);
  
  // return the value for indentation of code
  virtual int tabIndent ();
  
  // return the value for where line wrapping starts
  virtual int maxLineLength ();
  
 private:
  int defaultLineLength;
  int defaultIndentation;
};
