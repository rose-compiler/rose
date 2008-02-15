/* unparse_format_fort.h
 *
 * Formatting control for Fortran unparser
 *
 */

#ifndef UNPARSER_FORMAT_FORT_H
#define UNPARSER_FORMAT_FORT_H

// #include "rose.h"
#include "sage3.h"
#include "roseInternal.h"

#include "unparse_format.h"

#define KAI_NONSTD_IOSTREAM 1
#include IOSTREAM_HEADER_FILE


class UnparseFormatFort : public UnparseFormatBase {

private:
  // maximum line length before wrapping/continuation
  static int DefaultMaxLineLength;

  // indentation and maximum indentation value
  static int DefaultTabIndent;
  static int DefaultMaxIndent;

  // line continuation indent
  static int DefaultLineContIndent;

  // Maximum size of unparsed numbers.  No string representing a
  // number should be larger then this value (if it were to be then it
  // is regenerated in exponential notation).
  static int MaxDblDigitsDisplay;

  // Sizes of buffers for unparsing numbers
  static int IntBufferSize;
  static int DblBufferSize;

public:
  UnparseFormatFort(ostream* _os, UnparseFormatHelp *help = 0);
  virtual ~UnparseFormatFort();

  virtual void put(int c) { assert(false); } // FIXME:eraxxon
  
  //! Because we have no look-ahead information, we must format based
  //! on only what we have seen.  Thus, to format a typical executable
  //! statement s1 (which must be followed by a newline and indent), we
  //! cannot add the characters after processing s1, but must wait
  //! until the *following* statement s2 is seen.  In order to
  //! facilitate this, we keep track of the previous statement.
  void format(SgLocatedNode*, SgUnparse_Info& info, 
	      FormatOpt opt = FORMAT_BEFORE_STMT);

  //! inserts 'num' newlines into the unparsed file and adjusts the !
  //! indent level for the next line by 'iamnt'.  Note: 'iamnt' is
  //! ignored if 'num' is less than 1.
  void insert_newline(int num = 1, int iamnt = 0);

  //! increase indent by 'num' (generates indent characters)
  void indent(int num);

  //! inserts num spaces into the unparsed file; does not change indent
  void insert_space(int num);

  //! flush the buffer
  void flush() { os->flush(); }

  //! state
  int current_line() const { return curLine; }
  int current_col() const { return curColumn; }
  bool line_is_empty() const { return (curColumn == 0); }

  //! state: manually set the previously seen node
  void set_prev(SgLocatedNode* x) { prevnode = x; } 

  //! configuration: line length (no wrapping if maxLineLength <= 0)
  void set_maxLineLength(int w) { maxLineLength = w; } 
  int get_maxLineLength() const { return maxLineLength; }

  //! configuration: indent stop
  void set_maxIndent(int s) { maxIndent = s; } 
  int get_maxIndent() const { return maxIndent; } 

  //! routines to output strings that are *already* Fortran values.
  //! (In other words this is not interpreted as a Fortran string
  //! constant.)
  UnparseFormatFort& operator<<(string x);
  UnparseFormatFort& operator<<(const char* const x);

  //! routines to output Fortran values from Sage values
  UnparseFormatFort& operator<<(char x); 
  UnparseFormatFort& operator<<(unsigned char x);
  UnparseFormatFort& operator<<(short x);
  UnparseFormatFort& operator<<(unsigned short x);
  UnparseFormatFort& operator<<(int x);
  UnparseFormatFort& operator<<(unsigned int x);
  UnparseFormatFort& operator<<(long x);
  UnparseFormatFort& operator<<(unsigned long x);
  UnparseFormatFort& operator<<(long long x);
  UnparseFormatFort& operator<<(unsigned long long x);
  UnparseFormatFort& operator<<(float x);
  UnparseFormatFort& operator<<(double x);
  UnparseFormatFort& operator<<(long double x);
  
private:
  ostream* os; //! the directed output for the current file
  UnparseFormatHelp *h;

  int curLine;   //! current line number, initialized to 1
  int curColumn; //! current column, initialized to 0

  int curIndent;     //! indent of the current line
  int curStmtIndent; //! indent of beg. of current stmt (may span many lines)
  
  int maxLineLength; //! maximum line length
  int maxIndent;     //! maximum indent amount

  SgLocatedNode* prevnode;
  
  //! Puts a Fortran string into a file, handling all line wrapping, etc.
  void fortput(const char* const x);

  //! make the output nicer
  void removeTrailingZeros(char* const inputString);
  
  int getUndoLineContinuationIndent();

};

#endif

