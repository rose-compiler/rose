/* unparser.C
 * Contains the implementation of the constructors, destructor, formatting functions,
 * and fucntions that unparse directives.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"
// #include "unparse_format.h"
#include "unparseFormatHelp.h"
#include <iomanip>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


UnparseFormat::UnparseFormat( ostream* nos, UnparseFormatHelp *inputFormatHelp)
   {
  // Set the output stream (C++ ostream mechanism)
     ROSE_ASSERT(nos);
     os            = nos;
     os->precision(16);

  // Set the helper class used to control unparsing
     formatHelpInfo = inputFormatHelp;

  // Set other variables that store state about the formatting as the code is unparsed
     chars_on_line = 0;
     stmtIndent    = 0;
     currentIndent = 0;
     currentLine   = 1;
     linewrap      = MAXCHARSONLINE;

  // indentstop    = MAXINDENT;
     indentstop    = (formatHelpInfo != NULL) ? formatHelpInfo->maxLineLength() : MAXINDENT;

     prevnode      = NULL;
   }

UnparseFormat::~UnparseFormat()
   {
  // DQ (3/18/2006): I think we can assert this
     ROSE_ASSERT(os != NULL);
     if (os != NULL)
        {
       // Add a new line to avoid warnings from many compilers about lack of a final CR in the generated code
          insert_newline();

       // Call the flush function to force out the final output to the target file
          (*os).flush();
        }

  // Delete the UnparseFormatHelp object if one was used (C++ does not need this conditional test)
     if (formatHelpInfo != NULL)
          delete formatHelpInfo;
   }

//-----------------------------------------------------------------------------------
//  void Unparser::insert_newline
//
//  inserts num newlines into the unparsed file
//
// Liao, 5/16/2009: some comments:
//   the resetting of chars_on_line can help remove redundant insertion of two consecutive empty lines.
//   In most cases, this is a desired behavior. 
//    But sometimes , an extra empty line (line 2)
//    must be preserved after an empty line (line 1) ending with '\' preceeding it.
//    e.g.
// #define BZ_ITER(nn) 
//    int nn;
//
//    BZ_ITER(i);
//    For the example above, caller to this function has to pass num>1 to ensure an insertion 
//    always happen for the second '\n' character.
//-----------------------------------------------------------------------------------
void
UnparseFormat::insert_newline(int num, int indent)
   {
     if (chars_on_line == 0)
        {
          --num;
        }

     for (int i = 0 ; i < num; i++)
        {
#if 1
          (*os) << endl;
#else
       // DQ (5/7/2010): Test the line number value as a prelude to an option that would rest 
       // the Sg_File_Info objects in AST to match that of the unparsed code.
          (*os) << "// (line=" << currentLine << ")" << endl;
#endif
        }

     if (num > 0)
        {
          currentIndent = 0;
          chars_on_line = 0;
          currentLine+=num;
        }

     if (indent > currentIndent)
         indent -= currentIndent;
     else
         indent = 0;

     if (indent > 0)
        insert_space( (indent > indentstop)? indentstop : indent );
   }

//-----------------------------------------------------------------------------------
//  void Unparser::insert_space
//
//  inserts num spaces into the unparsed file
//-----------------------------------------------------------------------------------
void
UnparseFormat::insert_space(int num)
   {
  // insert blank space
     for (int i = 0; i < num; i++)
          (*os) << " ";
     if (num > 0)
        {
          if (currentIndent == chars_on_line) 
               currentIndent += num;
          chars_on_line += num;
        }
   }


UnparseFormat& UnparseFormat::operator << ( string out)
   {
     const char* p  = out.c_str();
     const char* const head= out.c_str();

  // DQ (7/20/2008): Better to fix it here then use the code "++p2;" (below)
  // const char* p2 = p + strlen(p)-1;
     const char* p2 = p + strlen(p);

  // DQ (3/18/2006): The default is TABINDENT, but we get a value from formatHelp if available
     int tabIndentSize = TABINDENT;
     if (formatHelpInfo != NULL)
          tabIndentSize = formatHelpInfo->tabIndent();

#if 0
  // DQ (7/20/2008): I have always wanted to turn this off...I can't figure 
  // out why it is a great idea to eat explicit trailing CRs.

  // DQ (3/18/2006): I think that this is the cause of the famous "\n" eating
  // problem for strings output using "cur" in the code generation.  I makes
  // since that this would be handled this way, but I always wondered why the
  // "\n" at the end of a string was ignored in the implementation of the code
  // generation.
     for ( ; (*p2 == '\n'); --p2)
        {
        }
          ++p2;
#endif

  // DQ: Better code might use "strlen(p)" instead of "(p2 - p)"
     if (linewrap > 0 && chars_on_line + (p2 - p) >= linewrap) 
        {
          insert_newline(1, stmtIndent + 2 * tabIndentSize);
        }

  // printf ("p = %p p2 = %p \n",p,p2);

  // DQ (12/3/2006): This is related to a 64 bit bug where p starts as p2+1 and this for loop ends in a seg fault!
  // for ( ; p != p2; p++)
     for ( ; p < p2; p++)
        {
          ROSE_ASSERT(p != NULL);
       // printf ("p = %p p2 = %p *p = %c \n",p,p2,*p);

     // Liao, 5/16/2009
     // insert_newline() has a semantic to skip the second and after new line for a sequence of 
     // '\n'. It is very useful to remove excessive newlines in the unparased file.
     //
     // BUT:	  
     // two consecutive '\n' might be essential for the correctness of a program
     // e.g. 
     //       # define BZ_ITER(nn) 
     //         int nn; 
     //
     //      BZ_ITER(I);
     // In the example above, the extra new line after "int nn; \" must be preserved!
     // Otherwise, the following statement will be treated as a continuation line of "int nn;\"
     // 
     // So the code below is changed to lookback two characters to decide if the line continuation
     // case is encountered and call a special version of insert_newline() to always insert a line. 	  
          if ( *p == '\n') 
             {
               bool mustInsert=false;
               if ((p-head)>1)
                  {
                    char ahead1 = *(p-2);
                    char ahead2 = *(p-1);
                    if ((ahead1=='\\') && (ahead2=='\n'))
                    mustInsert = true;
                  }
               if (mustInsert)
                    insert_newline(2,-1);
                 else
                    insert_newline();
             }
            else
             {
               (*os) << *p;
               chars_on_line ++;
             }
        }

     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (int num)
   {
     char buffer[MAX_DIGITS];
     sprintf(buffer, "%d", num);
     assert (strlen(buffer) < MAX_DIGITS);
  // (*os) << buffer;
     (*this) << buffer;
     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (short num)
   {
     char buffer[MAX_DIGITS];

  // [DTdbug] 3/13/2000 -- Changing the format specifier.  I don't think
  //          that %su is conventional, and I think that's what's making
  //          the code crash in this case (using SUN's CC compiler).
  //
  // sprintf(buffer, "%sd", num);

     sprintf(buffer, "%hd", num);
     assert (strlen(buffer) < MAX_DIGITS);
     (*this) << buffer;
     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (unsigned short num)
   {
     char buffer[MAX_DIGITS];

  // [DTdbug] 3/13/2000 -- Changing the format specifier.  I don't think
  //          that %su is conventional, and I think that's what's making
  //          the code crash in this case (using SUN's CC compiler).
  //
  // sprintf(buffer, "%su", num);

     sprintf(buffer, "%hu", num); 
     assert (strlen(buffer) < MAX_DIGITS);
     (*this) << buffer;
     return *this;
   }

UnparseFormat& UnparseFormat:: operator <<(unsigned int num)
   {
     char buffer[MAX_DIGITS];
     sprintf(buffer, "%u", num);
     assert (strlen(buffer) < MAX_DIGITS);
     (*this) << buffer;
     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (long num)
   {
     char buffer[MAX_DIGITS];
     sprintf(buffer, "%ld", num);
     assert (strlen(buffer) < MAX_DIGITS);
     (*this) << buffer;
     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (unsigned long num)
   {
     char buffer[MAX_DIGITS];
     sprintf(buffer, "%lu", num);
     assert (strlen(buffer) < MAX_DIGITS);
     (*this) << buffer;
     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (long long num)
   {
     char buffer[MAX_DIGITS];
     sprintf(buffer, "%ld", (long)num);
     assert (strlen(buffer) < MAX_DIGITS);
     (*this) << buffer; 
     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (unsigned long long num)
   {
     char buffer[MAX_DIGITS];
     sprintf(buffer, "%lu", (long)num);
     assert (strlen(buffer) < MAX_DIGITS);
     (*this) << buffer;
     return *this;
   }

void
UnparseFormat::removeTrailingZeros ( char* inputString )
   {
  // Supporting function for formating floating point numbers
     int i = strlen(inputString)-1;
  // replace trailing zero with a null character (string terminator)
     while ( (i > 0) && (inputString[i] == '0') )
        {
       // Leave the trailing zero after the '.' (generate "2.0" rather than "2.")
       // this makes the output easier to read and more clear that it is a floating 
       // point number.
          if (inputString[i-1] != '.')
               inputString [i] = '\0';
          i--;
        }
   }

UnparseFormat& UnparseFormat:: operator << (float num)
   {
  // DQ (4/21/2005): Modified to use ostream instead of sprintf
#if 0
     ROSE_ASSERT (MAX_DIGITS < 1024);
     ROSE_ASSERT (MAX_DIGITS_DISPLAY < MAX_DIGITS);
     char buffer[1024];
  // sprintf(buffer, "%e", num);
     sprintf(buffer, "%0.8f", num);

  // If it is a REALLY large number then regenerate the string in exponential form.
     if (strlen(buffer) > MAX_DIGITS_DISPLAY)
          sprintf(buffer, "%8e", num);

     assert (strlen(buffer) < MAX_DIGITS_DISPLAY);
     removeTrailingZeros(buffer);
     (*os) << buffer;
#else
  // DQ (4/21/2005): Set the precision higher than required and let the ostream operators remove trailing zeros etc.
  // (*os) << setiosflags(ios::showpoint) << setprecision(8) << num;
  // (*this) << setiosflags(ios::showpoint) << setprecision(12) << num;
     stringstream  out;
     out << setiosflags(ios::showpoint) << setprecision(12) << num;
     (*this) << out.str();
#endif
     return *this;
   }

UnparseFormat& UnparseFormat:: operator << (double num)
   {
  // DQ (4/21/2005): Modified to use ostream instead of sprintf
#if 0
     ROSE_ASSERT (MAX_DIGITS < 1024);
     char buffer[1024];
  // sprintf(buffer, "%f", num);
     sprintf(buffer, "%0.15f", num);

  // printf ("strlen(buffer) = %d \n",strlen(buffer));

  // If it is a REALLY large number then regenerate the string in exponential form.
     if (strlen(buffer) > MAX_DIGITS_DISPLAY)
          sprintf(buffer, "%16e", num);

     assert (strlen(buffer) < MAX_DIGITS);
  // printf ("Before removeTrailingZeros(): buffer = %s \n",buffer);
     removeTrailingZeros(buffer);
  // printf ("After removeTrailingZeros(): buffer = %s \n",buffer);
     (*os) << buffer;
#else
  // Don't set the precision since the default is 16 and anything that we unparse
  // after that will be incorrect since the precision os a double is about 16 digits.
  // (*os) << setiosflags(ios::showpoint) << num;

  // DQ (4/21/2005): Set the precision higher than required and let the ostream operators remove trailing zeros etc.
  // (*os) << setiosflags(ios::showpoint) << setprecision(24) << num;
     stringstream  out;
     out << setiosflags(ios::showpoint) << setprecision(24) << num;
     (*this) << out.str();
#endif

     return *this;
   }

// DQ (4/21/2005): It makes not difference to make the function parameter const ref!
// UnparseFormat& UnparseFormat:: operator << (const long double & num)
UnparseFormat& UnparseFormat:: operator << (long double num)
   {
  // DQ (4/21/2005): Modified to use ostream instead of sprintf
#if 0
     char buffer[MAX_DIGITS];
  // sprintf(buffer, "%Lf", num); // this generates an assert error below
     sprintf(buffer, "%1.32lf", num); // g++ warns of passing long double to double
     assert (strlen(buffer) < MAX_DIGITS);
     removeTrailingZeros(buffer);
     (*os) << buffer;
#else
  // This does not work (some sort of bug in the stadard library, I think)!
  // (*os) << setiosflags(ios::showpoint) << setprecision(32) << num;
  // (*os) << setiosflags(ios::showpoint) << setprecision(16) << num;
  // (*os) << num;

  // DQ (4/21/2005): Set the precision higher than required and let the ostream operators remove trailing zeros etc.
  // (*os) << setiosflags(ios::showpoint) << setprecision(48) << num;
     stringstream  out;
     out << setiosflags(ios::showpoint) << setprecision(48) << num;
     (*this) << out.str();
#endif
     return *this;
   }

#if 0
UnparseFormat& UnparseFormat:: operator << (void* pointer)
   {
#if 0
     char buffer[MAX_DIGITS];
     sprintf(buffer, "%p", pointer);
     assert (strlen(buffer) < MAX_DIGITS);
#else
     string buffer = StringUtility::numberToString(pointer);
#endif
     (*this) << buffer;
     return *this;
   }
#endif


void UnparseFormat::set_linewrap( int w) { linewrap = w; } // no wrapping if linewrap <= 0
int UnparseFormat::get_linewrap() const { return linewrap; }

void
UnparseFormat::outputHiddenListData(Unparser* unp, SgScopeStatement* inputScope )
   {
  // debugging support
     unp-> cur << "\n /* Hidden declaration list in " << inputScope->class_name() << ": size      = " << inputScope->get_hidden_declaration_list().size() << " */ ";
     unp-> cur << "\n /* Hidden type list in " << inputScope->class_name() << ": size             = " << inputScope->get_hidden_type_list().size() << " */ ";
     unp-> cur << "\n /* Hidden type elaboration list in " << inputScope->class_name() << ": size = " << inputScope->get_type_elaboration_list().size() << " */ ";

     for (set<SgSymbol*>::iterator i = inputScope->get_hidden_declaration_list().begin(); i != inputScope->get_hidden_declaration_list().end(); i++)
        {
          printf ("In hidden_declaration_list: i = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
          unp-> cur << "\n /* Hidden declaration list: i = " <<  StringUtility::numberToString(*i) << " = " << (*i)->class_name().c_str() << " = " << SageInterface::get_name(*i).c_str() << " */ ";
        }
     for (set<SgSymbol*>::iterator i = inputScope->get_hidden_type_list().begin(); i != inputScope->get_hidden_type_list().end(); i++)
        {
          printf ("In hidden_type_list:        i = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
          unp-> cur << "\n /* Hidden declaration list: i = " <<  StringUtility::numberToString(*i) << " = " << (*i)->class_name().c_str() << " = " << SageInterface::get_name(*i).c_str() << " */ ";
        }
     for (set<SgSymbol*>::iterator i = inputScope->get_type_elaboration_list().begin(); i != inputScope->get_type_elaboration_list().end(); i++)
        {
          printf ("In hidden_elaboration_list: i = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
          unp-> cur << "\n /* Hidden declaration list: i = " <<  StringUtility::numberToString(*i) << " = " << (*i)->class_name().c_str() << " = " << SageInterface::get_name(*i).c_str() << " */ ";
        }
     unp-> cur << "\n ";
   }


bool UnparseFormat::formatHelp(SgLocatedNode* node, SgUnparse_Info& info, FormatOpt opt)
   {
  // Note that since the default implementations of getLine and getCol return the value -1,
  // these must be overridden if this function is to return true.  The default implementation
  // also is that help (h) is NULL so for that reason the function typically returns false as well.

     assert(node != NULL);
     if (formatHelpInfo != NULL)
        {
          int line = formatHelpInfo->getLine(node, info, opt) - currentLine;
          int col  = formatHelpInfo->getCol(node, info, opt)  - chars_on_line;

          if (line >= 0 && col >= 0)
             {
               insert_newline(line, 0);
               insert_space( col );
             }
            else
               if (col >= 0)
                  {
                    insert_space(col);
                  }
                 else
                    return false;

          return true;
        }

     return false;
   }


string
UnparseFormat::formatOptionToString(FormatOpt opt)
   {
     string s;
     switch(opt)
        {
          case FORMAT_AFTER_STMT:
               s = "FORMAT_AFTER_STMT";
               break;
          case FORMAT_BEFORE_STMT:
               s = "FORMAT_BEFORE_STMT";
               break;
          case FORMAT_BEFORE_DIRECTIVE:
               s = "FORMAT_BEFORE_DIRECTIVE";
               break;
          case FORMAT_AFTER_DIRECTIVE:
               s = "FORMAT_AFTER_DIRECTIVE";
               break;
          case FORMAT_BEFORE_BASIC_BLOCK1:
               s = "FORMAT_BEFORE_BASIC_BLOCK1";
               break;
          case FORMAT_AFTER_BASIC_BLOCK1:
               s = "FORMAT_AFTER_BASIC_BLOCK1";
               break;
          case FORMAT_BEFORE_BASIC_BLOCK2:
               s = "FORMAT_BEFORE_BASIC_BLOCK2";
               break;
          case FORMAT_AFTER_BASIC_BLOCK2:
               s = "FORMAT_AFTER_BASIC_BLOCK2";
               break;
          case FORMAT_BEFORE_NESTED_STATEMENT:
               s = "FORMAT_BEFORE_NESTED_STATEMENT";
               break;
          case FORMAT_AFTER_NESTED_STATEMENT:
               s = "FORMAT_AFTER_NESTED_STATEMENT";
               break;

          default:
             {
               printf ("Error: default reached in switch for UnparseFormat::formatOptionToString ... \n");
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

void
UnparseFormat::format(SgLocatedNode* node, SgUnparse_Info& info, FormatOpt opt)
   {
  // DQ (added comments): this function addes new line formatting to the unparse statements
  // depending on the type of statement and the options with which it is called.

     if (info.get_outputCodeGenerationFormatDelimiters() == true)
        {
       // printf ("In UnparseFormat::format(%s,opt=%d) \n",node->class_name().c_str(),opt);
       // (*this) << formatOptionToString(opt) << ":" << node->class_name() << "[";
          (*this) << formatOptionToString(opt) << ":" << node->class_name() << "[";
        }

  // DQ (3/18/2006): The default is TABINDENT but we get a value from formatHelp if available
     int tabIndentSize = TABINDENT;
     if (formatHelpInfo != NULL)
          tabIndentSize = formatHelpInfo->tabIndent();

  // This provides a default implementation when the user has not specificed any help to control the unparsing
     if ( formatHelp(node, info, opt) == false )
        {
          int v = node->variantT();
          int v1 = (prevnode == 0) ? 0 : prevnode->variantT();
          switch (opt)
             {
               case FORMAT_AFTER_STMT:
                    if (v == V_SgFunctionDefinition || v == V_SgClassDefinition)
                         insert_newline(2);
                    break;
               case FORMAT_BEFORE_STMT:
                  {
                    switch(v)
                       {
                         case V_SgBasicBlock:
                      // DQ (3/18/2006): Added SgNullStatement as something that should not generate formatting in this case
                         case V_SgNullStatement:
                              break;
                         default:
                            {
                              if (!info.inConditional())
                                 {
                                   linewrap = MAXCHARSONLINE;
                                   prevnode = node;
                                   if  (v == V_SgFunctionDefinition || v == V_SgClassDefinition)
                                        insert_newline(2,stmtIndent);
                                     else
                                        insert_newline(1,stmtIndent);
                                 }
                            }
                       }
                    break;
                  }
               case FORMAT_BEFORE_DIRECTIVE:
                    linewrap = -1;
                    insert_newline(1,0); 
                    break;
               case FORMAT_AFTER_DIRECTIVE:
                    linewrap = MAXCHARSONLINE;
                    insert_newline();
                    break;
               case FORMAT_BEFORE_BASIC_BLOCK1:
                    if ( v1 != V_SgCatchOptionStmt && v1 != V_SgDoWhileStmt  && 
                         v1 != V_SgForStatement && v1 != V_SgIfStmt && 
                         v1 != V_SgSwitchStatement && v1 != V_SgWhileStmt ) 
                       insert_newline(); 
                    break;
               case FORMAT_AFTER_BASIC_BLOCK1:
                    stmtIndent += tabIndentSize;
                    break;
               case FORMAT_BEFORE_BASIC_BLOCK2:
                    stmtIndent -= tabIndentSize;
                    insert_newline(1,stmtIndent);
                    break;
               case FORMAT_AFTER_BASIC_BLOCK2:
                    break;
               case FORMAT_BEFORE_NESTED_STATEMENT:
                    if (v != V_SgBasicBlock) {
                      stmtIndent += tabIndentSize;
                    }
                    break;
               case FORMAT_AFTER_NESTED_STATEMENT:
                    if (v != V_SgBasicBlock) {
                      stmtIndent -= tabIndentSize;
                    }
                    break;
               default:
                  {
                    printf ("Error: default reached in switch for formatting within unparsing ... \n");
                    ROSE_ASSERT(false);
                  }
             }
        }

     if (info.get_outputCodeGenerationFormatDelimiters() == true)
        {
       // printf ("Leaving UnparseFormat::format(%s,opt=%d) \n",node->class_name().c_str(),opt);
          (*this) << "]" << node->class_name();
        }
   }





#if 0
// DQ (3/18/2006): This appears to be old code, I think we can remove it at some point.

//-----------------------------------------------------------------------------------
//  int UnparseOrigFormat::get_type_len
//  
//  Auxiliary function used by special_cases to determine the length of
//  the given type. This length is then subtracted from the amount to indent.
//-----------------------------------------------------------------------------------
int
UnparseOrigFormat::get_type_len(SgType* type)
   {
     assert(type != NULL);
  
     switch(type->variant())
        {
          case T_UNKNOWN: return 0;
          case T_CHAR: return 4;	
          case T_SIGNED_CHAR: return 11;	
          case T_UNSIGNED_CHAR: return 13;	
          case T_SHORT: return 5;	
          case T_SIGNED_SHORT: return 12; 	
          case T_UNSIGNED_SHORT: return 14;	
          case T_INT: return 3;	
          case T_SIGNED_INT: return 10;	
          case T_UNSIGNED_INT: return 12;	
          case T_LONG: return 4;	
          case T_SIGNED_LONG: return 11;	
          case T_UNSIGNED_LONG: return 13;	
          case T_VOID: return 4;	
          case T_GLOBAL_VOID: return 11;	
          case T_WCHAR: return 5;	
          case T_FLOAT: return 5;	
          case T_DOUBLE: return 6;	
          case T_LONG_LONG: return 9;
          case T_UNSIGNED_LONG_LONG: return 18;	
          case T_LONG_DOUBLE: return 11;	
          case T_STRING: return 6;
          case T_BOOL: return 4;	
          case T_COMPLEX: return 7;

          case T_DEFAULT:
             {
               // (*os) << "T_DEFAULT not implemented" << endl;
               //  printf ("In Unparser::get_type_len(): T_DEFAULT not implemented (returning 0) \n");
               // ROSE_ABORT();
               return 0;
               break;
             }

          case T_POINTER:
             {
               SgPointerType* ptr_type = isSgPointerType(type);
               assert(ptr_type != NULL);
               return 1 + get_type_len(ptr_type->get_base_type());
             }

          case T_MEMBER_POINTER:
             {
               // (*os) << "T_MEMBER_POINTER not implemented" << endl;
                cerr << "In Unparser::get_type_len(): T_MEMBER_POINTER not implemented (returning 0)" << endl;
               // ROSE_ABORT();
               return 0;
               break;
             }

  case T_REFERENCE: {  // not sure
    SgReferenceType* ref_type = isSgReferenceType(type);
    assert(ref_type != NULL);
    return 1 + get_type_len(ref_type->get_base_type());
  }
  
  case T_NAME: {
    cerr << "T_NAME not implemented" << endl;
    break;
  }	
  
  case T_CLASS: {
    int length = 0;
    SgClassType* class_type = isSgClassType(type);
    assert (class_type != NULL);
    
    SgName qn = class_type->get_name();

 // We can't force all SgName objects to have a valid string!
 // assert (qn.str() != NULL);

    if (qn.str() != NULL)
         length += strlen(qn.str());
    return length;
  }

  case T_ENUM: {
    SgEnumType* enum_type = isSgEnumType(type);
    assert (enum_type != NULL);
    SgName qn = enum_type->get_name();
    return strlen(qn.str());
  }

  case T_TYPEDEF: {
    SgTypedefType* typedef_type = isSgTypedefType(type);
    SgName nm = typedef_type->get_name();
    return strlen(nm.str());
  }

  case T_MODIFIER: {
    SgModifierType* mod_type = isSgModifierType(type);
    assert(mod_type != NULL);
    int length = 0;
    if (mod_type->get_typeModifier().get_constVolatileModifier().isConst()) length += 5;
    if (mod_type->get_typeModifier().get_constVolatileModifier().isVolatile()) length += 8;
 // DQ (4/22/2004): Removed CC++ support
 // if (mod_type->isSync()) length += 0;
 // if (mod_type->isGlobal()) length += 0;
    if (mod_type->get_typeModifier().isRestrict()) length += 8;
    
    if (mod_type->get_typeModifier().get_constVolatileModifier().isConst() && 
        mod_type->get_typeModifier().get_constVolatileModifier().isVolatile())
         length += 1;

    return length + get_type_len(mod_type->get_base_type()) + 1;
  }
  
  case T_FUNCTION: {
    SgFunctionType* func_type = isSgFunctionType(type);
    assert(func_type != NULL);
    SgType* ret_type = func_type->get_return_type();
    assert(ret_type != NULL);
    
    return get_type_len(ret_type);

  }
  
  case T_MEMBERFUNCTION: {
    SgMemberFunctionType* mfunc_type = isSgMemberFunctionType(type);
    assert(mfunc_type != NULL);

    int length = 0;
    SgClassDefinition* class_stmt = mfunc_type->get_struct_name();
    assert(class_stmt != NULL);
    length += get_type_len(class_stmt->get_declaration()->get_type());

    SgType* ret_type = mfunc_type->get_return_type();
    assert(ret_type != NULL);

    length += 2;                  //space for "::"
    
    return length + get_type_len(ret_type);

  }

  case T_PARTIAL_FUNCTION: {
    cerr <<  "T_PARTIAL_FUNCTION not implemented" << endl;
    break;
  }
  case T_ARRAY:	{
    SgArrayType* array_type = isSgArrayType(type);
    assert(array_type != NULL);
    
    SgType* base_type = array_type->get_base_type();

    return get_type_len(base_type);
  }
  case T_ELLIPSE:{
    cerr << "T_ELLIPSE not implemented" << endl;
    break;
  } 

  default: return 0;
  
  }
  return 0;
}
#endif

#if 0
// DQ (3/18/2006): This appears to be old code, I think we can remove it at some point.

//-----------------------------------------------------------------------------------
//  int UnparseOrigFormat::cases_of_printSpecifier
//
//  Calculates the length of any keywords to subtract from the total number of 
//  spaces to indent. Unparser::printSpecifier prints out any keywords 
//  necessary for the declaration and this function subtracts the length of keywords
//  printed.  Read the comments for the function special_cases to find out why this
//  is needed. However, the keywords are sometimes generated automatically by Sage,
//  such as "auto." If the original file did not contain "auto," but the option to
//  print "auto" is true, then this function will erroneously add to subcol. Yet if
//  the original file did include "auto" and the option is true, then we would be
//  correctly adding to subcol. This discrepancy is impossible for the unparser to 
//  distinguish. 
//-----------------------------------------------------------------------------------
int
UnparseOrigFormat::cases_of_printSpecifier(SgLocatedNode* node, SgUnparse_Info& info)
   {
     int subcol = 0;
  // if (decl_stmt->isExtern() && decl_stmt->get_linkage()) {
  //      (*os) << "extern \"" << decl_stmt->get_linkage() << "\" ";
  //      if (decl_stmt->isExternBrace()) (*os) << "{ ";
  //    }
     SgDeclarationStatement* decl_stmt = isSgDeclarationStatement(node);
     if (decl_stmt != NULL)
        {
       // DQ (4/25/2004): Moved to function decalration case below
       // if (decl_stmt->isVirtual()) subcol += 8;
          if (decl_stmt->get_declarationModifier().isFriend())
               subcol += 7;
       // DQ (4/25/2004): Moved to function decalration case below
       // if (decl_stmt->isInline()) subcol += 7;
       // DQ (4/25/2004): Removed CC++ support
       // if (decl_stmt->.isAtomic() && !info.SkipAtomic()) subcol += 7;
          if (decl_stmt->get_declarationModifier().get_storageModifier().isStatic())
               subcol += 7;
          if ( decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && 
              !decl_stmt->get_linkage() )
               subcol += 7;
       // DQ (4/25/2004): Removed CC++ support
       // if (decl_stmt->isGlobalClass() && !info.SkipGlobal() ) subcol += 7;
       // checks option status before adding to subcol
       // if (opt.get_auto_opt())
       // if (decl_stmt->isAuto()) subcol += 5;
          if (decl_stmt->get_declarationModifier().get_storageModifier().isRegister())
               subcol += 9;
        }

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL)
        {
          if (functionDeclaration->get_functionModifier().isInline())
               subcol += 7;
          if (functionDeclaration->get_functionModifier().isVirtual())
               subcol += 8;
        }

     return subcol;
   }
#endif

#if 0
// DQ (3/18/2006): This appears to be old code, I think we can remove it at some point.

//-----------------------------------------------------------------------------------
//  int UnparseOrigFormat::special_cases
//
//  Handles and calculates the length to subtract from the total number of spaces
//  to indent. The column information returned from a declaration is designated
//  at the name. Thus, the length of the type must be found to subtract from the 
//  column number of the name. 
//-----------------------------------------------------------------------------------
int
UnparseOrigFormat::special_cases(SgLocatedNode* node)
   {
  // column length to subtract
     int subcol = 0;

     if (isSgVariableDeclaration(node))
        {
          SgVariableDeclaration* vardecl_stmt = isSgVariableDeclaration(node);
          assert(vardecl_stmt != NULL);
          SgInitializedNamePtrList initname_list = vardecl_stmt->get_variables();
       // SgInitializedNamePtrList::const_iterator iter = initname_list.begin();
          SgInitializedNamePtrList::iterator iter = initname_list.begin();
          if (iter != initname_list.end())
             {
               ROSE_ASSERT ((*iter) != NULL);
               SgType* tmp_type = (*iter)->get_type();
               assert(tmp_type != NULL);

            // adding one since there's a space in between the type and name
               subcol += get_type_len(tmp_type) + 1; 
             }
        }
       else
        {
          if (isSgMemberFunctionDeclaration(node))
             {
               SgMemberFunctionDeclaration* mfuncdecl_stmt = isSgMemberFunctionDeclaration(node);
               assert(mfuncdecl_stmt != NULL);
               SgType* rtype = NULL;
               if ( !( mfuncdecl_stmt->get_specialFunctionModifier().isConstructor() || 
                       mfuncdecl_stmt->get_specialFunctionModifier().isDestructor()  ||
                       mfuncdecl_stmt->get_specialFunctionModifier().isConversion() ) )
                  {
                 // this means that the function has a return type, so calculate the length of this type
                    if (mfuncdecl_stmt->get_orig_return_type())
                         rtype = mfuncdecl_stmt->get_orig_return_type();
                      else
                         rtype = mfuncdecl_stmt->get_type()->get_return_type();

                    subcol += get_type_len(rtype) + 1;
                  }

               if ( !isSgClassDefinition(mfuncdecl_stmt->get_parent()) )
                  {
                 // this means that the function is not in a class structure, so we must get
                 // the length of the class name <class>::<function name>
                    SgName scopename;
                 // DQ (11/17/2004): Interface modified, use get_class_scope() if we want a
                 // SgClassDefinition, else use get_scope() if we want a SgScopeStatement.
                 // scopename = mfuncdecl_stmt->get_scope()->get_declaration()->get_qualified_name();
                    scopename = mfuncdecl_stmt->get_class_scope()->get_declaration()->get_qualified_name();
                    subcol += strlen(scopename.str()) + 2;     // 2 extra spaces from the "::"
                  }
             }
            else
             {
               if (isSgFunctionDeclaration(node))
                  {
                    SgFunctionDeclaration* funcdecl_stmt = isSgFunctionDeclaration(node);
                    assert(funcdecl_stmt != NULL);
                    SgType* tmp_type = funcdecl_stmt->get_type()->get_return_type();
                    assert(tmp_type != NULL);
                    subcol += get_type_len(tmp_type) + 1;
                  }
                 else
                  {
                    if (isSgClassDeclaration(node))
                       {
                         SgClassDeclaration* classdecl_stmt = isSgClassDeclaration(node);
                         assert(classdecl_stmt != NULL);
                         subcol += 6;   //for "class "
                       }
                      else
                       {
                         if (isSgCaseOptionStmt(node))
                            {
                              SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(node);
                              assert(case_stmt != NULL);
                              subcol += 5;   //for "case "
                            }
                           else
                            {
                              if (isSgLabelStatement(node))
                                 {
                                   SgLabelStatement* label_stmt = isSgLabelStatement(node);  
                                   assert(label_stmt != NULL);
                                   // assert(label_stmt->get_label().str() != NULL);
                                   subcol += strlen(label_stmt->get_label().str());
                                 }
                                else
                                 {
                                   if (isSgTypedefDeclaration(node))
                                      {
                                        SgTypedefDeclaration* typedef_stmt = isSgTypedefDeclaration(node);
                                        assert(typedef_stmt != NULL);
                                        subcol += 8;   //for "typedef "
                                        SgType* tmp_type = typedef_stmt->get_base_type();
                                        subcol += get_type_len(tmp_type) + 1;
                                      }
                                 }
                            }
                       }
                  }
             }
        }

  // may need to take care of more special cases here
  
     return subcol;
   }
#endif


#if 0
// DQ (3/18/2006): This appears to be old code, I think we can remove it at some point.

int UnparseOrigFormat::getCol(SgLocatedNode *node, SgUnparse_Info& info, FormatOpt opt)
{
  assert( node!=NULL );
  int r = -1;
  if( opt == FORMAT_BEFORE_STMT && node->get_file_info()!=NULL) {
    r = node->get_file_info()->get_col();
    r -= cases_of_printSpecifier(node, info);
  }
  return r;
}
#endif

#if 0
// DQ (3/18/2006): This appears to be old code, I think we can remove it at some point.

//-----------------------------------------------------------------------------------
//  int UnparseOrigFormat::getLine(SgLocatedNode *node) 
//
//  Get the line number from the Sage LocatedNode Object
//-----------------------------------------------------------------------------------
int UnparseOrigFormat::getLine(SgLocatedNode *node, SgUnparse_Info& info, FormatOpt opt)
{
  assert( node!=NULL );
  int r = -1;
  if( opt == FORMAT_BEFORE_STMT && node->get_file_info()!=0) {
    r = node->get_file_info()->get_line();
  }
  return r;
}
#endif


