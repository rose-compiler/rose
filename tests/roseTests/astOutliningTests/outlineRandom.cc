/*!
 *  \file outlineRandom.cc
 *
 *  \brief Outlines random statements from a program.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 *
 *  This utility has a special option, "-rose:outline:preproc-only",
 *  which can be used just to see the results of the outliner's
 *  preprocessing phase.
 *
 *  \note This utility duplicates functionality (and code!) in
 *  liaoutline.cc and injectOutlinePragmas.cc. This program enables
 *  testing of the outliner independent of whether '#pragmas' are
 *  working properly.
 */
#include <rose.h>
#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <cstdlib>
#include <algorithm>


#include <commandline_processing.h>

#include <ASTtools.hh>
#include <PreprocessingInfo.hh>
#include "Outliner.hh"
#include "RandomStmtSelector.hh"

using namespace std;

// =====================================================================

//! Stores a list of statements.
typedef std::list<SgStatement *> StmtList_t;

// =====================================================================

//! Stores command-line options.
struct ProgramOptions
{
  ProgramOptions (void)
    : preproc_only_ (false),
      node_type_ (""),
      num_outline_ (0),
      make_pdfs_ (false),
      emit_stages_ (false)
  {
  }

  bool preproc_only_;
  std::string node_type_; //!< Only outline this kind of node.
  size_t num_outline_;
  bool make_pdfs_;
  bool emit_stages_; //!< Emit outlining results in stages.
};

//! Process command-line options.
static bool getOptions (vector<string>& argvList, ProgramOptions& opts);

// =====================================================================

//! Generates a PDF into the specified file.
static void makePDF (const SgProject* proj,
                     const std::string& = std::string (""));

//! Randomly outline statements.
static size_t outlineRandom (SgProject* proj,
                             size_t max_outline,
                             bool emit_stages,
                             const std::string& node_type);

//! Run just the outlining preprocessor phase on random statements.
static size_t preprocessRandom (SgProject* proj,
                                size_t max_outline,
                                bool emit_stages,
                                const std::string& node_type);

//! Generate an informative message about a statement.
static std::string toString (const SgStatement* s, size_t id = 0);

// =====================================================================

using namespace std;

// =====================================================================

int
main (int argc, char* argv[])
{
  vector<string> argvList(argv, argv + argc);

  ProgramOptions opts;
  if (!getOptions (argvList, opts))
    return 1;

  cerr << "[Processing source...]" << endl;
  SgProject* proj = frontend (argvList);
  ROSE_ASSERT (proj);

  if (opts.make_pdfs_)
    {
      cerr << "[Generating a PDF...]" << endl;
      makePDF (proj);
    }

  if (!proj->get_skip_transformation ())
    {
      size_t count = 0;
      if (opts.preproc_only_)
        {
          cerr << "[Running outliner's preprocessing phase only...]" << endl;
          count = preprocessRandom (proj,
                                    opts.num_outline_, opts.emit_stages_,
                                    opts.node_type_);
        }
      else
        {
          cerr << "[Outlining...]" << endl;
          count = outlineRandom (proj,
                                 opts.num_outline_, opts.emit_stages_,
                                 opts.node_type_);
        }
      cerr << "  [Processed " << count << " outline directives.]" << endl;

      if (opts.make_pdfs_)
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
bool
getOptions (vector<string>& argvList, ProgramOptions& opts)
{
  // Required
  int num_outlines = 0;
  if (!CommandlineProcessing::isOptionWithParameter (argvList,
                                                     "-rose:outline:",
                                                     "random",
                                                     num_outlines,
                                                     true)
      || num_outlines < 1)
    {
      cerr << endl
           << "usage: " << argvList[0]
           << " -rose:outline:random <k>"
           << " [-rose:outline:preproc-only]"
           << " [-rose:outline:node <SgNode type>]"
           << " ..." << endl
           << endl
           << "This program randomly  selects <k> statements from" << endl
           << "the input file to outline." << endl
           << endl;
      return false;
    }

  opts.num_outline_ = (size_t)num_outlines;
  cerr << "==> Selecting up to " << num_outlines << " statements." << endl;

  // Optional
  if (CommandlineProcessing::isOption (argvList,
                                       "-rose:outline:",
                                       "preproc-only",
                                       true))
    {
      cerr << "==> Running the outliner's preprocessing phase only." << endl;
      opts.preproc_only_ = true;
    }

  string node_type;
  if (CommandlineProcessing::isOptionWithParameter (argvList,
                                                    "-rose:outline:", "node",
                                                    node_type, true))
    {
      cerr << "==> Only outlining '" << node_type << "' nodes." << endl;
      opts.node_type_ = node_type;
    }

  int verbosity_level = 0;
  if (CommandlineProcessing::isOptionWithParameter (argvList,
                                                    "-rose:", "verbose",
                                                    verbosity_level,
                                                    false)
      && verbosity_level >= 2)
    {
      cerr << "==> Making PDFs." << endl;
      opts.make_pdfs_ = true;
    }

  if (CommandlineProcessing::isOption (argvList,
                                       "-rose:outline:", "emit-stages",
                                       true))
    {
      cerr << "==> Emitting intermediate outlining results." << endl;
      opts.emit_stages_ = true;
    }

  // Args seem OK
  return true;
}

// =====================================================================

static
string
toString (const SgStatement* s, size_t id)
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

template <typename StmtsConstIterator>
void
dump (StmtsConstIterator b, StmtsConstIterator e)
{
  cerr << "========== OUTLINE TARGETS ==========" << endl;
  size_t id = 0;
  for (StmtsConstIterator i = b; i != e; ++i)
    cerr << "  " << toString (*i, ++id) << endl;
}

// ========================================================================

static
void
unparseIt (SgFile* f)
{
  static size_t stage = 0;
  ROSE_ASSERT (f);

  // Save current default unparse output filename.
  string outname_default = f->get_unparse_output_filename ();

  // Created a 'staged' name.
  stringstream s;
  s << "OUT" << ++stage << "-";
  if (!outname_default.empty ()) // Has a name set.
    s << "OUT" << ++stage << "-" << outname_default;
  else // No name set yet; create one.
    s << "rose_" << f->get_sourceFileNameWithoutPath ();
  f->set_unparse_output_filename (s.str ());

  // Unparse it.
  unparseFile (f);

  // Restore default name.
  f->set_unparse_output_filename (outname_default);
}

// ========================================================================

static
SgFile *
getRoot (SgNode* node)
{
  if (isSgFile (node))
    return isSgFile (node);
  else if (node)
    return getRoot (node->get_parent ());

  // Default: Return error condition (null).
  return 0;
}

// ========================================================================

/*!
 *  \brief Selects up to the specified number of statements for
 *  outlining.
 *
 *  This routine returns a list of statements to outline, in the
 *  forward-iteration order in which they should be outlined.
 */
static
void
selectStatements (SgProject* proj,
                  size_t max_outline,
                  const string& node_type,
                  StmtList_t& final_targets)
{
  //! Traversal to determine the order of the targets.
  class ReorderTraversal : public AstSimpleProcessing
  {
  public:
    ReorderTraversal (RandomStmtSelector::StmtSet_t& initial_targets,
                      StmtList_t& final_targets)
      : initial_targets_ (initial_targets),
        final_targets_ (final_targets)
    {
    }

    virtual void visit (SgNode* n)
    {
      SgStatement* s = isSgStatement (n);
      if (s && initial_targets_.find (s) != initial_targets_.end ())
        final_targets_.push_front (s);
    }

  private:
    //! List of statements to find (input).
    RandomStmtSelector::StmtSet_t& initial_targets_;

    //! Final ordering of targets, in preorder traversal order.
    StmtList_t& final_targets_;
  };

  // Collect a set of outlining targets.
  RandomStmtSelector::StmtSet_t initial_targets;
  RandomStmtSelector::selectOutlineable (proj, max_outline,
                                         initial_targets);

  if (!node_type.empty ())
    {
      RandomStmtSelector::StmtSet_t temp_targets;
      copy (initial_targets.begin (), initial_targets.end (),
            inserter (temp_targets, temp_targets.begin ()));
      initial_targets.clear ();
      for (RandomStmtSelector::StmtSet_t::iterator i = temp_targets.begin ();
           i != temp_targets.end (); ++i)
        if ((node_type == "SgBasicBlock" && isSgBasicBlock (*i))
            || (node_type == "SgReturnStmt" && isSgReturnStmt (*i)))
          initial_targets.insert (*i);
    }

  // Put them in the proper order for sequential outlining.
  ReorderTraversal reorderer (initial_targets, final_targets);
  reorderer.traverseInputFiles (proj, preorder);

  ROSE_ASSERT (initial_targets.size () == final_targets.size ());
}

//! Runs the outlining preprocessor on a single statement.
static
void
preprocess (SgStatement* s, bool emit = false)
{
  static size_t count = 0;
  cerr << "=== PREPROCESSING: " << toString (s, ++count) << " ===" << endl;
  SgBasicBlock* b = Outliner::preprocess (s);
  ROSE_ASSERT (b);
  ASTtools::attachComment (toString (s, count), b);

  if (emit) // Unparse this intermediate result
    unparseIt (getRoot (b));
}

static
size_t
preprocessRandom (SgProject* proj, size_t max_outline, bool emit_stages,
                  const string& node_type)
{
  StmtList_t targets;
  selectStatements (proj, max_outline, node_type, targets);
  dump (targets.begin (), targets.end ());
  for_each (targets.begin (), targets.end (),
            bind2nd (ptr_fun (preprocess), emit_stages));
  return targets.size ();
}

//! Outlines a single statement.
static
void
outline (SgStatement* s, bool emit = false)
{
  static size_t count = 0;
  cerr << "=== OUTLINING: " << toString (s, ++count) << " ===" << endl;
  Outliner::Result r = Outliner::outline (s);
  ROSE_ASSERT (r.isValid ());
  ASTtools::attachComment (toString (s, count), r.decl_);

  if (emit)
    unparseIt (getRoot (s));
}

static
size_t
outlineRandom (SgProject* proj, size_t max_outline, bool emit_stages,
               const string& node_type)
{
  StmtList_t targets;
  selectStatements (proj, max_outline, node_type, targets);
  dump (targets.begin (), targets.end ());
  for_each (targets.begin (), targets.end (),
            bind2nd (ptr_fun (outline), emit_stages));
  return targets.size ();
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
