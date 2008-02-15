/* unparse_format.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_FORMAT_H
#define UNPARSER_FORMAT_H

// #include "rose.h"
#include "sage3.h"
#include "roseInternal.h"

#define KAI_NONSTD_IOSTREAM 1
#include IOSTREAM_HEADER_FILE

#define MAXCHARSONLINE 1000
#define MAXINDENT  60
#define TABINDENT 2

// Used in unparser.C in functions Unparser::count_digits(<type>)
// default was 10, but that is too small for float, double, and long double
// (use of 10 also generated a purify error for case of double)
// Size of buffer used to generation of strings from number values,
// it is larger than what we need to display because the values can 
// be arbitrarily large (up to the size of MAX_DOUBLE)
// #define MAX_DIGITS 128
#define MAX_DIGITS 512

// Size of what we want to have be displayed in the unparsed (generated) code.
// No string representing a number should be larger then this value
// (if it were to be then it is regenerated in exponential notation).
#define MAX_DIGITS_DISPLAY 32

enum FormatOpt {
  FORMAT_BEFORE_DIRECTIVE, FORMAT_AFTER_DIRECTIVE,
  FORMAT_BEFORE_STMT, FORMAT_AFTER_STMT, 
  FORMAT_BEFORE_BASIC_BLOCK1, FORMAT_AFTER_BASIC_BLOCK1, 
  FORMAT_BEFORE_BASIC_BLOCK2, FORMAT_AFTER_BASIC_BLOCK2
};

class UnparseFormatHelp
{
 public:
  virtual ~UnparseFormatHelp() {}
   virtual int getLine( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt)  
                { return -1; } 
            // return: > 0: start new lines; == 0: use same line; < 0:default
   virtual int getCol( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt) 
                   { return -1; }
           // return starting column. if < 0, use default
};


class UnparseFormatBase {
public:
  UnparseFormatBase() { }
  virtual ~UnparseFormatBase() { }

  virtual void put(int c) = 0; // FIXME:eraxxon: make non-virtual

  // FIXME:eraxxon: a lot of stuff can become common
};


class UnparseFormat : public UnparseFormatBase
   {
    int currentLine; //! stores current line number being unparsed
    int currentIndent; //! indent of the current line
    int chars_on_line; //! the number of characters printed on the line
    int stmtIndent; // the current indent for statement
    int linewrap; // the characters allowed perline before wraping the line
    int indentstop; // the number of spaces allowed for indenting
    SgLocatedNode* prevnode;
    ostream* os; //! the directed output for the current file
    UnparseFormatHelp *h;

 // void insert_newline(int i = 1, int indent = -1);
    void insert_space(int);

//! make the output nicer
    void removeTrailingZeros ( char* inputString );

   bool formatHelp(SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt = FORMAT_BEFORE_STMT); 
  public:
   
    virtual void put(int c); 
    UnparseFormat& operator << (string out);
    UnparseFormat& operator << (int num);
    UnparseFormat& operator << (short num);
    UnparseFormat& operator << (unsigned short num);
    UnparseFormat& operator << (unsigned int num);
    UnparseFormat& operator << (long num);
    UnparseFormat& operator << (unsigned long num);
    UnparseFormat& operator << (long long num);
    UnparseFormat& operator << (unsigned long long num);
    UnparseFormat& operator << (float num);
    UnparseFormat& operator << (double num);
    UnparseFormat& operator << (long double num);

    int current_line() const { return currentLine; }
    int current_col() const { return chars_on_line; }
    bool line_is_empty() const { return currentIndent == chars_on_line; }

 // DQ (2/16/2004): Make this part of the public interface (to control old-style K&R C function definitions)
    void insert_newline(int i = 1, int indent = -1);

   UnparseFormat( ostream* _os, UnparseFormatHelp *help = 0 );
   virtual ~UnparseFormat();

  //! the ultimate formatting functions
  void format(SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt = FORMAT_BEFORE_STMT);
  void flush() { os->flush(); }
  void set_linewrap( int w) { linewrap = w; } // no wrapping if linewrap <= 0
  int get_linewrap() const { return linewrap; }
  void set_indentstop( int s) { indentstop = s; } 
  int get_indentstop() const { return indentstop; } 
};

class UnparseOrigFormat : public UnparseFormatHelp
{
  //! returns the length that we should subtract when printing out keywords in printSpecifier
  int cases_of_printSpecifier(SgLocatedNode* node, SgUnparse_Info& info);

  int get_type_len(SgType* type);  //! returns the length of the given type
  int special_cases(SgLocatedNode*); //! returns the length to subtract when indenting
public:
   virtual int getLine( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);
   virtual int getCol( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);
};

#endif






