#ifndef UNPARSER_OPT_H
#define UNPARSER_OPT_H

/*!  unparser_opt.h
 *  This class contains all options we want to have available when
 *  generating C++ code
 *
 *  Controls the printing of:
 *  auto_opt - "auto" in front of variable declarations
 *  linefile_opt - line and file information printed by Gary_Unparser::output
 *  overload_opt - overloaded operators for non-primitive types. For example,
 *                prints out "+" instead of "operator+"
 *  boolnum_opt - booleans as numbers (0 and 1) to designate false and true or
 *               as words ("false" and "true") themselves
 *  this_opt - "this" in from of data or function members    
 *  caststring_opt - "const char*" in front of strings
 *  debug_opt - debugging information to standard output
 *  class_opt - "class" in front of function parameters
 *  forced_transformation_format - controls whether or not to use file information
 *                                 when formatting 
!*/
#include <stdio.h>

class Unparser_Opt
   {
 //! This class controls the final unparsing of the AST
     public:

       // constructors
          Unparser_Opt();    // all options are set to false by default
          Unparser_Opt(bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);

       // destructor
         ~Unparser_Opt();

       // access functions
          bool get_auto_opt();
          void set_auto_opt(bool opt);
          bool get_linefile_opt();
          void set_linefile_opt(bool opt);
          bool get_overload_opt();
          void set_overload_opt(bool opt);
          bool get_num_opt();
          void set_num_opt(bool opt);
          bool get_this_opt();
          void set_this_opt(bool opt);
          bool get_caststring_opt();
          void set_caststring_opt(bool opt);
          bool get_debug_opt();
          void set_debug_opt(bool opt);
          bool get_class_opt();
          void set_class_opt(bool opt);
          bool get_forced_transformation_opt();
          void set_forced_transformation_opt(bool opt);
          bool get_unparse_includes_opt();
          void set_unparse_includes_opt(bool opt);

          void display ( const std::string& label );

     private:
          bool auto_opt;
          bool linefile_opt;
          bool overload_opt;
          bool boolnum_opt;
          bool this_opt;
          bool caststring_opt;
          bool debug_opt;
          bool class_opt;
          bool forced_transformation_format;
          bool unparse_includes_opt;
   };

#endif


















