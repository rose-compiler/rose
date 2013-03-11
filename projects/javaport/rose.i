%module(directors="1") rose

// DQ (3/10/2013): These are handld from the SWIG command line now.
// Common warnings: 315, 325, 350, 351, 401, 451, 503, 516, 822
// We can ignore specific warnings:
//    Warning 315: Nothing known about 'boost::hash'.
//    Warning 325: Nested struct not currently supported (NESectionTableEntry_disk ignored)
//    Warning 350: operator new ignored
//    Warning 351: operator delete ignored
//    Warning 401: Nothing known about base class 'RangeMap< Extent >'. Ignored.
//    Warning 451: Setting a const char * variable may leak memory.
//    Warning 503: Can't wrap 'operator ==' unless renamed to a valid identifier.
//    Warning 516: Overloaded method SgFile::get_libraryArchiveObjectFileNameList() ignored
//    Warning 822: Covariant return types not supported in Java. Proxy method will return SgAsmGenericSection *.
// This suppression of warning does not work is over-ridden by -Wall and/or -v on the swig command line.
// #pragma SWIG nowarn=516
// #pragma SWIG nowarn=315,325,350,351,401,451,503,516,822

class SgNode;
%typemap(javaout) SgNode *, SgNode const *, SgNode const*& %{{return gov.llnl.casc.rose.MakeSgNode.makeDerivedJavaNodeClass($jnicall, $owner);}%}

%include <various.i>
%include <std_vector.i>
%include <std_except.i>
%include <std_list.i>
%include <std_pair.i>
%include <std_string.i>
%include <std_common.i>
%include <typemaps.i>

// I had to find this on the web, now in the javaport directory (not present in SWIG distribution).
%include <std_map.i>

// Copied this from the web (not present in SWIG distribution).
%include <std_set.i>

#define Rose_STL_Container std::vector

// DQ (3/7/2013): Including specific header files directly.
%include rosedll.h

%include rose_paths.h
%{
// Include global variables used in rose_paths.h
#include "rose_paths.h"
%}

%include rosedefs.h
%{
// Need access to rose_addr_t typedef.
#include "rosedefs.h"
%}

%include rose_attributes_list.h
%{
// Include global types used in rose_attributes_list.h
#include "rose_attributes_list.h"
%}

%include string_functions.h
%{
#include "string_functions.h"
%}

%include escape.h
%{
#include "escape.h"
%}

%include sla.h
%{
#include "sla.h"
%}

%include AstAttributeMechanism.h
%{
#include "AstAttributeMechanism.h"
%}

// Need to define Sg_Options class for SWIG.
%{
class Sg_Options {} __sg_opt_a;
Sg_Options& Sg_options()
   {
     return __sg_opt_a;
   }
%}

#define ALT_FIXUP_COPY 1

%include Cxx_Grammar.h
%{
// This appears to be ignored by SWIG (swig issues a warning to this effect).
// #pragma SWIG nowarn=516
#define ROSE_DEPRECATED_FUNCTION
#include "Cxx_Grammar.h"
%}

// ******************************************************************
// These files below this line represent additional functionality 
// that could be added to the Java API support in ROSE.  It is not
// clear how far we should go toward handling everything.  The 
// support (above) is sufficient to define the API for all of the 
// IR.  We can iterate on defining more support as needed.
// ******************************************************************

%include utility_functions.h
 // %{
 // #include "utility_functions.h"
 // %}

// Markus Schordan: temporary fixes for Ast flaws (modified by DQ)
// #include <typeinfo>

// DQ (12/9/2004): The name of this file has been changed to be the new location
// of many future Sage III AST manipulation functions in the future.  A namespace
// (SageInterface) is defined in sageInterface.h.
%include sageInterface.h
// %{
// #include "sageInterface.h"
// %}


// DQ (3/29/2006): Moved Rich's support for better name mangling to a 
// separate file (out of the code generation via ROSETTA).
// #include "manglingSupport.h"

// Markus Kowarschik: we use the new mechanism of handling preprocessing info;
// i.e., we output the preprocessing info attached to the AST nodes.
// See the detailed explanation of the mechanisms in the beginning of file
// attachPreprocessingInfo.C
#define USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO 0

// DQ (9/1/2006): It is currently an error to normalize the source file names stored 
// in the SgProject IR node to be absolute paths if they didn't originally appear 
// that way on the commandline.  We have partial support for this but it is a bug
// at the moment to use this.  However, we do now (work by Andreas) normalize the
// source file name when input to EDG so that all Sg_File_Info objects store an
// absolute path (unless modified using a #line directive, see test2004_60.C as an 
// example).  The current work is an incremental solution.
#define USE_ABSOLUTE_PATHS_IN_SOURCE_FILE_LIST 0

// #include "RoseBin_support.h"
// DQ (7/6/2005): Added to support performance analysis of ROSE.
// This is located in ROSE/src/midend/astDiagnostics
// #include "AstPerformance.h"

// %include AstProcessing.h
// %include AstSimpleProcessing.h
%include AstConsistencyTests.h
%{
#include "AstConsistencyTests.h"
%}

%include unparser.h
%{
#include "unparser.h"
%}

// %include rose_attributes_list.h
// %include AstProcessing.h
   // %template(AstTDP_ParentAttr) AstTopDownProcessing<ParentAttribute>;
   // %include AstFixParentTraversal.h
// %include AST_FILE_IO.h
// %include astQueryInheritedAttribute.h
// %include astQuery.h
// %include nodeQuery.h

