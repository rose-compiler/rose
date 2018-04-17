/**
 *  \file injectOutlinePragmas.cc
 *
 *  \brief Demonstrate the Outliner by outlining random statements
 *  in a source file.
 *
 *  \author Rich Vuduc <richie@llnl.gov>
 *
 *  This program randomly selects 'k' statements from the input file,
 *  and inserts '#pragma rose_outline' declarations.
 */
#include <rose.h>
#include <iostream>
#include <set>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <cstdlib>

#include <commandline_processing.h>

#include "Outliner.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "RandomStmtSelector.hh"

// ========================================================================

using namespace std;

// ========================================================================

static
string
toString (const SgStatement* s, size_t id = 0)
{
  ROSE_ASSERT (s);
  const SgFunctionDeclaration* decl =
    SageInterface::getEnclosingFunctionDeclaration (const_cast<SgStatement*>(s));
  ROSE_ASSERT (decl);
  string func_tag (decl->get_qualified_name ().str ());

  stringstream str;
  if (id)
    str << "(" << id << ") ";
  str << func_tag << "()"
    << " -> "
    << ASTtools::toStringFileLoc (isSgLocatedNode (s))
    << " : <" << s->class_name () << ">";
  return str.str ();
}

// ========================================================================

static
void
insertOutlineDirectives (RandomStmtSelector::StmtSet_t& S)
{
  size_t num_outlines = 0;
  for (RandomStmtSelector::StmtSet_t::iterator i = S.begin ();
       i != S.end (); ++i)
    {
      SgStatement* s = *i;
      SgBasicBlock* b = isSgBasicBlock (s->get_parent ());
      if (b)
        {
          cerr << "  " << toString (s, ++num_outlines) << endl;

          // Generate pragma, randomly choosing between
          // pass-by-reference or pass-by-pointer outlining styles.
          char* pragma_text = new char[30];
          ROSE_ASSERT (pragma_text);
          strcpy (pragma_text, "rose_outline");

          SgPragma* pragma = new SgPragma (pragma_text,
                                           ASTtools::newFileInfo ());
          ROSE_ASSERT (pragma);
          SgPragmaDeclaration* pragma_decl =
            new SgPragmaDeclaration (ASTtools::newFileInfo (), pragma);
          ROSE_ASSERT (pragma_decl);

          // Insert pragma
          ASTtools::moveBeforePreprocInfo (s, pragma_decl);
          isSgStatement (b)->insert_statement (s, pragma_decl, true);
          pragma_decl->set_parent (b);

          string comment ("Randomly selected outline target: ");
          comment += ASTtools::toStringFileLoc (s);
          ASTtools::attachComment (comment, pragma_decl);
        }
    }
}

// ========================================================================

int
main (int argc, char* argv[])
{
  int num_outlines = 0;
  vector<string> argvList(argv, argv + argc);
  if (!CommandlineProcessing::isOptionWithParameter (argvList,
                                                     "-rose:", "num-outline",
                                                     num_outlines,
                                                     true)
      || num_outlines < 1)
    {
      cerr << endl
           << "usage: " << argv[0] << " -rose:num-outline <k> ..." << endl
           << endl
           << "This program randomly  selects <k> statements from" << endl
           << "the input file to outline." << endl
           << endl;
      return 1;
    }

  cerr << "[Parsing...]" << endl;
  SgProject* proj = frontend (argvList);

  cerr << "[Selecting "
       << num_outlines
       << " statements to outline...]" << endl;
  RandomStmtSelector::StmtSet_t stmts;
  RandomStmtSelector::selectOutlineable (proj, (size_t)num_outlines, stmts);

  cerr << "[Inserting 'outline' directives...]" << endl;
  insertOutlineDirectives (stmts);

  cerr << "[Unparsing...]" << endl;
  return backend (proj);
}

// eof
