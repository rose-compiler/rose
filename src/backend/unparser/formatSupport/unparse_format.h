/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_FORMAT_H
#define UNPARSER_FORMAT_H

//#include "sage3.h"
//#include "roseInternal.h"
#include "unparser.h"
class SgScopeStatement;

class SgLocatedNode;
class Unparser;

#define KAI_NONSTD_IOSTREAM 1
// #include IOSTREAM_HEADER_FILE
#include <iostream>

// DQ (1/26/2009): a value of 1000 is too small for Fortran code (see test2009_09.f; from Bill Henshaw)
// This value is now increased to 1,000,000.  If this is too small then likely we want to
// know about it anyway!
// #define MAXCHARSONLINE 1000
#define MAXCHARSONLINE 1000000

#define MAXINDENT  60

// DQ: Try out a larger setting
#define TABINDENT 2
// #define TABINDENT 5

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

// DQ (3/16/2006): Added comments.  
// These control how indentation and newlines are added in the
// pre and post processing of each statement.
typedef enum Format_Opt
   {
     FORMAT_BEFORE_DIRECTIVE, 
     FORMAT_AFTER_DIRECTIVE,
     FORMAT_BEFORE_STMT, 
     FORMAT_AFTER_STMT, 
     FORMAT_BEFORE_BASIC_BLOCK1,
     FORMAT_AFTER_BASIC_BLOCK1, 
     FORMAT_BEFORE_BASIC_BLOCK2,
     FORMAT_AFTER_BASIC_BLOCK2,
     FORMAT_BEFORE_NESTED_STATEMENT,
     FORMAT_AFTER_NESTED_STATEMENT,
   } FormatOpt;

#include "unparseFormatHelp.h"
class UnparseFormat 
   {
     int currentLine;   //! stores current line number being unparsed
     int currentIndent; //! indent of the current line
     int chars_on_line; //! the number of characters printed on the line
     int stmtIndent;    //! the current indent for statement
     int linewrap;      //! the characters allowed perline before wraping the line
     int indentstop;    //! the number of spaces allowed for indenting
     SgLocatedNode* prevnode; //! The previous SgLocatedNode unparsed
     std::ostream* os;  //! the directed output for the current file
     UnparseFormatHelp *formatHelpInfo;

  // void insert_newline(int i = 1, int indent = -1);
     void insert_space(int);

 //! make the output nicer
     void removeTrailingZeros ( char* inputString );

     bool formatHelp(SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt = FORMAT_BEFORE_STMT); 

     public:

          UnparseFormat& operator << (std::string out);
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

       // DQ (10/13/2006): Added to support debugging!
       // UnparseFormat& operator << (void* pointerValue);

          int current_line() const { return currentLine; }
          int current_col() const { return chars_on_line; }
          bool line_is_empty() const { return currentIndent == chars_on_line; }

       // DQ (2/16/2004): Make this part of the public interface (to control old-style K&R C function definitions)
          void insert_newline(int i = 1, int indent = -1);

	  //   private:
	  //Unparser* unp;
     public:
       // UnparserFormat(Unparser* unp,std::ostream* _os, UnparseFormatHelp *help = NULL):unp(unp){};
          UnparseFormat( std::ostream* _os = NULL, UnparseFormatHelp *help = NULL );
         ~UnparseFormat();

      //! the ultimate formatting functions
          void format(SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt = FORMAT_BEFORE_STMT);

       // void set_linewrap( int w) { linewrap = w; } // no wrapping if linewrap <= 0
       // int get_linewrap() const { return linewrap; }

          void flush() { os->flush(); }

          void set_linewrap( int w);// { linewrap = w; } // no wrapping if linewrap <= 0
          int get_linewrap() const;// { return linewrap; }

          void set_indentstop( int s) { indentstop = s; } 
          int get_indentstop() const { return indentstop; } 

       // DQ (3/18/2006): Added to support presentation and debugging of formatting
          std::string formatOptionToString(FormatOpt opt);
       // DQ (6/6/2007): Debugging support for hidden list data held in scopes
          void outputHiddenListData ( Unparser* unp,SgScopeStatement* inputScope );

   };

#endif






