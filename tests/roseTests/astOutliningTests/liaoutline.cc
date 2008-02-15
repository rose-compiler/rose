/*!
 *  \file liaoutliner.cc
 *
 *  \brief Demonstrates the pragma-interface of the LiaOutliner.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 *
 *  This utility has a special option, "-rose:outline:preproc-only",
 *  which can be used just to see the results of the outliner's
 *  preprocessing phase.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>

#include <rose.h>
#include <commandline_processing.h>

#include "LiaOutliner.hh"

//! Generates a PDF into the specified file.
static void makePDF (const SgProject* proj,
                     const std::string& = std::string (""));

// =====================================================================

using namespace std;

// =====================================================================

int
main (int argc, char* argv[])
{
  vector<string> argvList(argv, argv + argc);
  bool preproc_only = CommandlineProcessing::isOption (argvList,
                                                       "-rose:outline:",
                                                       "preproc-only",
                                                       true);

  cerr << "[Processing source...]" << endl;
  SgProject* proj = frontend (argvList);
  ROSE_ASSERT (proj);

  bool make_pdfs = proj->get_verbose () >= 2;
  if (make_pdfs)
    {
      cerr << "[Generating a PDF...]" << endl;
      makePDF (proj);
    }

  if (!proj->get_skip_transformation ())
    {
      size_t count = 0;
      if (preproc_only)
        {
          cerr << "[Running outliner's preprocessing phase only...]" << endl;
          count = LiaOutliner::preprocessAll (proj);
        }
      else
        {
          cerr << "[Outlining...]" << endl;
          count = LiaOutliner::outlineAll (proj);
        }
      cerr << "  [Processed " << count << " outline directives.]" << endl;

      if (make_pdfs)
        {
          cerr << "  [Making PDF of transformed AST...]" << endl;
          makePDF (proj, "outlined-");
        }
    }

  cerr << "[Unparsing...]" << endl;
  return backend (proj);
}

// =====================================================================

static
void
makePDF_SgFile (const SgFile* f, string fn_prefix)
{
  ROSE_ASSERT (f);

  string filename = fn_prefix + f->get_sourceFileNameWithoutPath ();
  AstPDFGeneration pdf_gen;
  pdf_gen.generateWithinFile (filename, const_cast<SgFile *> (f));
}

static
void
makePDF (const SgProject* proj, const string& fn_prefix)
{
  ROSE_ASSERT (proj);
  SgFilePtrListPtr files = const_cast<SgProject *> (proj)->get_fileList();
  if (files)
    for_each (files->begin (), files->end (),
              bind2nd (ptr_fun (makePDF_SgFile), fn_prefix));
}

// eof
