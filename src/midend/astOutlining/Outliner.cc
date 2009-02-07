/**
 *  \file Outliner.cc
 *  \brief An outlining implementation.
 */

#include <iostream>
#include <string>
#include <sstream>

#include <rose.h>

#include "NameGenerator.hh"
#include "Outliner.hh"
#include "Preprocess.hh"
#include "Transform.hh"
#include "commandline_processing.h"

// =====================================================================

using namespace std;

namespace Outliner {
  //! A set of flags to control the internal behavior of the outliner
  // use a wrapper for all variables or one parameter for a variable or a wrapper for all variables
  bool useParameterWrapper;  // use a wrapper for parameters of the outlined function
  bool preproc_only_;  // preprocessing only
  bool useNewFile; // generate the outlined function into a new source file
};

// =====================================================================

//! Factory for unique outlined function names.
static NameGenerator g_outlined_func_names ("OUT__", 0, "__");
static NameGenerator g_outlined_arg_names ("__out_argv", 0, "__");

//! Hash a string into an unsigned long integer.
static
unsigned long
hashStringToULong (const string& s)
{
  unsigned long sum = 0;
  for (size_t i = 0; i < s.length (); ++i)
    sum += (unsigned long)s[i];
  return sum;
}

string
Outliner::generateFuncName (const SgStatement* stmt)
{
  // Generate a prefix.
  stringstream s;
  s << g_outlined_func_names.next ();

  // Use the statement's raw filename to come up with a file-specific
  // tag.
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);
  s << hashStringToULong (info->get_raw_filename ()) << "__";

  return s.str ();
}
string
Outliner::generateFuncArgName (const SgStatement* stmt)
{
  // Generate a prefix.
  stringstream s;
  s << g_outlined_arg_names.next ();

  // Use the statement's raw filename to come up with a file-specific
  // tag.
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);
  s << hashStringToULong (info->get_raw_filename ()) << "__";

  return s.str ();
}
// =====================================================================

Outliner::Result
Outliner::outline (SgStatement* s)
{
  string func_name = generateFuncName (s);
  return outline (s, func_name);
}

Outliner::Result
Outliner::outline (SgStatement* s, const std::string& func_name)
{
  SgBasicBlock* s_post = preprocess (s);
  ROSE_ASSERT (s_post);
#if 0
  //Liao, enable -rose:outline:preproc-only, 
  // then any translator can accept it even if their drivers do not handle it individually
  // Internal usage only for debugging
  static bool preproc_only_ = false; 
  SgFile * file= SageInterface::getEnclosingFileNode(s);
  SgStringList argvList = file->get_originalCommandLineArgumentList ();	


  if (CommandlineProcessing::isOption (argvList,"-rose:outline:",
                                     "preproc-only",true))
  {
    cout << "==> Running the outliner's preprocessing phase only." << endl;
    preproc_only_ = true;
  // Avoid passing the option to the backend compiler 
    file->set_originalCommandLineArgumentList(argvList);
  }  
#endif
  if (preproc_only_)
  {
    Outliner::Result fake;
    return fake;
  }  
  else
    return Transform::outlineBlock (s_post, func_name);
}

//! Set internal options based on command line options
void Outliner::commandLineProcessing(std::vector<std::string> &argvList)
{
  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","preproc-only",true))
    preproc_only_ = true;
  else 
    preproc_only_ = false;

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","parameter_wrapper",true))
  {
    cout<<"Enabling parameter wrapping..."<<endl;
    useParameterWrapper= true;
  }
  else
    useParameterWrapper= false;

  if (CommandlineProcessing::isOption (argvList,"-rose:outline:","new_file",true))
  {
    cout<<"Enabling new source file for outlined functions..."<<endl;
    useNewFile= true;
  }
  else
    useNewFile= false;

  // keep --help option after processing, let other modules respond also
  if (CommandlineProcessing::isOption (argvList,"--help","",false))
  {
    cout<<"Outliner-specific options"<<endl;
    cout<<"Usage: outline [OPTION]... FILENAME..."<<endl;
    cout<<"Main operation mode:"<<endl;
    cout<<"\t-rose:outline:preproc-only           preprocessing only, no actual outlining"<<endl;
    cout<<"\t-rose:outline:parameter_wrapper     use an array of pointers for the variables to be passed"<<endl;
    cout<<"\t-rose:outline:new_file              use a new source file for the generated outlined function"<<endl;
    cout <<"---------------------------------------------------------------"<<endl;
  }
}


SgBasicBlock *
Outliner::preprocess (SgStatement* s)
{
  ROSE_ASSERT (isOutlineable (s, SgProject::get_verbose () >= 1));
  SgBasicBlock* s_post = Preprocess::preprocessOutlineTarget (s);
  ROSE_ASSERT (s_post); 
  return s_post;
}

/* =====================================================================
 *  Container to store the results of one outlining transformation.
 */

Outliner::Result::Result (void)
  : decl_ (0), call_ (0)
{
}

Outliner::Result::Result (SgFunctionDeclaration* decl,
                             SgStatement* call, SgFile* file/*=NULL*/)
  : decl_ (decl), call_ (call), file_(file)
{
}

Outliner::Result::Result (const Result& b)
  : decl_ (b.decl_), call_ (b.call_)
{
}

bool
Outliner::Result::isValid (void) const
{
  return decl_ && call_;
}
// eof
