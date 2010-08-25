/* unparser_opt.C */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"
 // include "unparser_opt.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

Unparser_Opt::Unparser_Opt()
   {
  // default setting: all options are false;
     auto_opt = false;
     linefile_opt = false;
     overload_opt = false;
     boolnum_opt = false;
     this_opt = false;
     caststring_opt = false;
     debug_opt = false;
     class_opt = false;
     forced_transformation_format = false;
     unparse_includes_opt = false;

 // display("In Unparser_Opt default constructor");
   }

Unparser_Opt::Unparser_Opt(bool _auto, bool linefile, bool useOverloadedOperators, bool num, bool _this, 
                           bool caststring, bool _debug, bool _class, bool _forced_transformation_format, bool _unparse_includes )
   {
     auto_opt = _auto;
     linefile_opt = linefile;
     overload_opt = useOverloadedOperators;
     boolnum_opt = num;
     this_opt = _this;
     caststring_opt = caststring;
     debug_opt = _debug;
     class_opt = _class;
     forced_transformation_format = _forced_transformation_format;
     unparse_includes_opt = _unparse_includes;

  // display("In Unparser_Opt non-default constructor");
   }

void
Unparser_Opt::display ( const string & label )
   {
     printf ("Inside of Unparser_Opt::display(%s) \n",label.c_str());

     printf ("auto_opt       = %s \n",(auto_opt) ? "true" : "false");
     printf ("linefile_opt   = %s \n",(linefile_opt) ? "true" : "false");
     printf ("overload_opt   = %s \n",(overload_opt) ? "true" : "false");
     printf ("boolnum_opt    = %s \n",(boolnum_opt) ? "true" : "false");
     printf ("this_opt       = %s \n",(this_opt) ? "true" : "false");
     printf ("caststring_opt = %s \n",(caststring_opt) ? "true" : "false");
     printf ("debug_opt      = %s \n",(debug_opt) ? "true" : "false");
     printf ("class_opt      = %s \n",(class_opt) ? "true" : "false");

     printf ("forced_transformation_format = %s \n",(forced_transformation_format) ? "true" : "false");
     printf ("unparse_includes_opt         = %s \n",(unparse_includes_opt) ? "true" : "false");
   }

Unparser_Opt::~Unparser_Opt()
   {
   }

bool Unparser_Opt::get_auto_opt()
   {
    return auto_opt;
   }

void Unparser_Opt::set_auto_opt(bool opt) {
  auto_opt = opt;
}

bool Unparser_Opt::get_linefile_opt() {
  return linefile_opt;
}

void Unparser_Opt::set_linefile_opt(bool opt) {
  linefile_opt = opt;
}

bool Unparser_Opt::get_overload_opt() {
  return overload_opt;
}

void Unparser_Opt::set_overload_opt(bool opt) {
  overload_opt = opt;
}

bool Unparser_Opt::get_num_opt() {
  return boolnum_opt;
}

void Unparser_Opt::set_num_opt(bool opt) {
  boolnum_opt = opt;
}

bool Unparser_Opt::get_this_opt() {
  return this_opt;
}

void Unparser_Opt::set_this_opt(bool opt) {
  this_opt = opt;
}

bool Unparser_Opt::get_caststring_opt() {
  return caststring_opt;
}

void Unparser_Opt::set_caststring_opt(bool opt) {
  caststring_opt = opt;
}

bool Unparser_Opt::get_debug_opt() {
  return debug_opt;
}

void Unparser_Opt::set_debug_opt(bool opt) {
  debug_opt = opt;
  printf ("Inside of Unparser_Opt::set_debug_opt() \n");
// ROSE_ABORT();
}

bool Unparser_Opt::get_class_opt() {
  return class_opt;
}

void Unparser_Opt::set_class_opt(bool opt) {
  class_opt = opt;
}

bool Unparser_Opt::get_forced_transformation_opt() {
  return forced_transformation_format;
}

void Unparser_Opt::set_forced_transformation_opt(bool opt) {
  forced_transformation_format = opt;
}

bool Unparser_Opt::get_unparse_includes_opt() {
  return unparse_includes_opt;
}

void Unparser_Opt::set_unparse_includes_opt(bool opt) {
  unparse_includes_opt = opt;
}

