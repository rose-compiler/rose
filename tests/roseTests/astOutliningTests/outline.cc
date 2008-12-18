/*!
 *  \file outline.cc
 *
 *  \brief Demonstrates the pragma-interface of the Outliner.
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
#include "Outliner.hh"

//! Generates a PDF into the specified file.
static void makePDF (const SgProject* proj,
                     const std::string& = std::string (""));

// =====================================================================

using namespace std;

// =====================================================================

int
main (int argc, char* argv[])
{
  //! Accepting command line options to the outliner
  vector<string> argvList(argv, argv + argc);
  Outliner::commandLineProcessing(argvList);

  SgProject* proj = frontend (argvList);
  ROSE_ASSERT (proj);

  bool make_pdfs = proj->get_verbose () >= 2;
  if (make_pdfs)
    makePDF (proj);

  if (!proj->get_skip_transformation ())
  {
    size_t count = 0;
    count = Outliner::outlineAll (proj);

    if (make_pdfs)
      makePDF (proj, "outlined-");
  }

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
  const SgFilePtrList& files = const_cast<SgProject *> (proj)->get_fileList();
  for_each (files.begin (), files.end (),
      bind2nd (ptr_fun (makePDF_SgFile), fn_prefix));
}

// eof
