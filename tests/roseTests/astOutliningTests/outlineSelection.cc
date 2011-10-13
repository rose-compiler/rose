/*!
 *  \file outlineSelection.cc
 *
 *  \brief Outlines statements from a program.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 *
 *  This command-line utility allows the user to outline a subset of
 *  outlineable statements in a given input source file. There are
 *  several ways of specifying which statements to outline:
 *
 *    - To outline the statement at line <n>, use "-rose:outline:line
 *    <n>".
 *
 *    - To outline up to <n> statements, starting with the first
 *    outlineable statement and including every <k>-th outlineable
 *    statement thereafter, use "-rose:outline:seq <n> <k>".
 *
 *    - To outline up to <n> random statements among all outlineable
 *    statements, use "-rose:outline:random <n>".
 *
 *  If several of the above options are specified, this utility will
 *  consider the union of all matching statements. However, only a
 *  single (last specified) instance of the ":random" option will be
 *  processed.
 *
 *  There are several others options of note:
 *
 *    - To run only the outliner's preprocessing phase, specify
 *    "-rose:outline:preproc-only". This option is useful for
 *    debugging.
 *
 *    - To create PDFs both before and after transformation, specify
 *    "-rose:verbose <k>" with any <k> at least 2.
 *
 *    - To see outlining results in stages (i.e., as each statement is
 *    outlined, rather than just the combined outlining result at the
 *    end), specify "-rose:outline:emit-stages".
 *
 *  \note This utility duplicates functionality (and code!) in
 *  liaoutline.cc and injectOutlinePragmas.cc. This program enables
 *  testing of the outliner independent of whether '#pragmas' are
 *  working properly.
 */
#include <rose.h>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <algorithm>


#include <commandline_processing.h>

#include <ASTtools.hh>
#include <PreprocessingInfo.hh>
#include "Outliner.hh"

using namespace std;

// =====================================================================

//! Stores a list of statements.
typedef std::list<SgStatement *> StmtList_t;

//! Stores a set of statements.
typedef std::set<SgStatement *> StmtSet_t;

// =====================================================================

//! Stores command-line options.
struct ProgramOptions_t
{
  ProgramOptions_t (void)
    : preproc_only_ (false),
      make_pdfs_ (false),
      emit_stages_ (false),
      max_rand_ (0)
  {
  }

  bool preproc_only_; //!< True if only preprocessing should be performed.
  bool make_pdfs_; //!< True if PDFs should be emitted.
  bool emit_stages_; //!< Emit outlining results in stages.

  // Options relevant to random statement collection.
  size_t max_rand_; //!< Maximum number of statements to select randomly.

  // Options relevant to sequence collection.
  typedef vector<size_t> Steps_t;
  Steps_t seqs_; //!< Sequence step sizes to collect.

  // Options relevant to source-position specification.
  typedef pair<std::string, size_t> SourcePos_t; //!< A source position.
  typedef vector<SourcePos_t> SourcePositions_t; //!< List of positions.
  SourcePositions_t lines_; //!< List of source positions.
};

//! Process command-line options.
static bool getOptions (vector<string>& argvList, ProgramOptions_t& opts);

// =====================================================================

//! Generates a PDF into the specified file.
static void makePDF (const SgProject* proj,
                     const std::string& = std::string (""));

//! Randomly outline statements.
static size_t outlineSelection (SgProject* proj,
                                const ProgramOptions_t& opts);

//! Run just the outlining preprocessor phase on random statements.
static size_t preprocessSelection (SgProject* proj,
                                   const ProgramOptions_t& opts);

//! Generate an informative message about a statement.
static std::string toString (const SgStatement* s, size_t id = 0);

// =====================================================================

using namespace std;

// =====================================================================

int
main (int argc, char* argv[])
{
  vector<string> argvList(argv, argv + argc);
  Outliner::commandLineProcessing(argvList);

  ProgramOptions_t opts;
  if (!getOptions (argvList, opts))
    return 1;

  if (SgProject::get_verbose() > 0)
       cerr << "[Processing source...]" << endl;
  SgProject* proj = frontend (argvList);
  ROSE_ASSERT (proj);

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *proj );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(proj,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");

  if (opts.make_pdfs_)
  {
    if (SgProject::get_verbose() > 0)
         cerr << "[Generating a PDF...]" << endl;
    makePDF (proj);
  }

  if (!proj->get_skip_transformation ())
  {
    size_t count = 0;
    string outfinal_prefix;
    if(Outliner::preproc_only_) // Liao, 2/24/2009, we moved this into Outliner::commandLineProcessing()
    //if (opts.preproc_only_)
    {
      if (SgProject::get_verbose() > 0)
           cerr << "[Running outliner's preprocessing phase only...]" << endl;
      count = preprocessSelection (proj, opts);
      outfinal_prefix = "outlined_preproc-";
    }
    else
    {
      if (SgProject::get_verbose() > 0)
           cerr << "[Outlining...]" << endl;
      count = outlineSelection (proj, opts);
      outfinal_prefix = "outlined_final-";
    }
    if (SgProject::get_verbose() > 0)
         cerr << "  [Processed " << count << " outline directives.]" << endl;

    if (opts.make_pdfs_)
    {
      if (SgProject::get_verbose() > 0)
           cerr << "  [Making PDF of transformed AST...]" << endl;
      makePDF (proj, outfinal_prefix);
    }
  }

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *proj, "_afterOutlining" );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     generateAstGraph(proj,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_afterOutlining");

  if (SgProject::get_verbose() > 0)
       cerr << "[Unparsing...]" << endl;
  return backend (proj);
}

// =====================================================================

//! Collect all statements at a particular source line.
static void collectStmtsAtPos (SgProject* root,
                               const string& filename, size_t line,
			       StmtSet_t& out_stmts);

//! Collect a sequence of statements.
static void collectStmtsSeq (SgProject* root,
                             size_t max_num_outline, size_t step,
			     StmtSet_t& out_stmts);

//! Collect a random number of statements.
static void collectRandom (SgProject* root,
                           size_t max_num_outline,
			   StmtSet_t& out_stmts);

//! \brief Simple traversal to collect outlineable statements.
/*!
 *  Child classes can customize which statements are collected by
 *  defining the pure virtual method, matches ().
 */
class OutlineableStmtCollector : public AstSimpleProcessing
{
  public:
    OutlineableStmtCollector (StmtSet_t& init_set)
      : out_stmts_ (init_set), init_size_ (init_set.size ())
    {
    }

    virtual void visit (SgNode* n)
    {
      SgStatement* s = isSgStatement (n);
      if (s && Outliner::isOutlineable (s) && !collected (s) && matches (s))
        collect (s);
    }

  protected:
    //! Returns the number of statements collected during the current traversal.
    size_t numCollected (void) const
    {
      return out_stmts_.size () - init_size_;
    }

    //! Returns 'true' if the given outlineable statement should be collected.
    /*!
     *  \pre The given statement, 's', is non-NULL and outlineable,
     *  according to Outliner::isOutlineable ().
     */
    virtual bool matches (const SgStatement* s) const = 0;

    //! Update any internal state, as necessary.
    virtual void updateAfterCollection (const SgStatement* s) {}

    //! Returns 'true' if 's' has been collected already.
    bool collected (const SgStatement* s) const
    {
      return out_stmts_.find (const_cast<SgStatement *> (s))
        != out_stmts_.end ();
    }

    //! Insert a given statement into the output set.
    void collect (SgStatement* s)
    {
      out_stmts_.insert (s);
    }

  private:
    StmtSet_t& out_stmts_; //!< Holds all collected statements.
    size_t init_size_; //!< Size of out_stmts_ when this object was constructed.
};

static
void
collectStmtsSeq (SgProject* root, size_t max_num_outline, size_t step,
		 StmtSet_t& out_stmts)
{
  //! Traversal to collect a sequence of statements.
  class StmtCollectorSeq : public OutlineableStmtCollector
  {
    public:
      StmtCollectorSeq (StmtSet_t& s, size_t n, size_t k)
        : OutlineableStmtCollector (s), max_ (n), step_ (k), num_checked_ (0)
      {
      }

    protected:
      /*!
       *  \brief Returns 'true' if the number of statements collected so
       *  far is less than max_ and the number of statements checked by
       *  this routine is a non-negative multiple of step_.
       */
      virtual bool matches (const SgStatement* s) const
      {
        return (numCollected () < max_) && ((num_checked_++ % step_) == 0);
      }

    private:
      size_t max_; //!< Maximum number to collect.
      size_t step_; //!< How many to skip between collections.
      mutable size_t num_checked_; //!< No. of times matches() was called.
  };

  // === Do collection. ===
  StmtCollectorSeq collector (out_stmts, max_num_outline, step);
  collector.traverseInputFiles (root, preorder);
}

static
void
collectStmtsAtPos (SgProject* root, const string& filename, size_t line,
		   StmtSet_t& out_stmts)
{
  //! Traversal to find statements at a given source line.
  class StmtFinderAtPos : public OutlineableStmtCollector
  {
  public:
    StmtFinderAtPos (StmtSet_t& s, const string& fn, size_t n)
      : OutlineableStmtCollector (s), filename_ (fn), lineno_ (n)
    {
    }
    
  protected:
    /*!
     * Match file and line. If the target filename is empty, then
     * match any filename; otherwise, filenames must match exactly.
     */
    virtual bool matches (const SgStatement* s) const
    {
      const Sg_File_Info* info = s->get_startOfConstruct ();
      ROSE_ASSERT (info);
      return (filename_.empty () || filename_ == info->get_raw_filename ())
	&& ((int)lineno_ == info->get_raw_line ());
    }
    
  private:
    string filename_; //!< Name of target file (empty for any)
    size_t lineno_; //!< Target line number.
  };

  // === Do collection. ===
  StmtFinderAtPos finder (out_stmts, filename, line);
  finder.traverseInputFiles (root, preorder);
}

static
void
collectRandom (SgProject* root, size_t max_num_outline, StmtSet_t& out_stmts)
{
  //! Traversal to select outlineable statements uniformly at random.
  class RandCollector : public OutlineableStmtCollector
  {
    typedef vector<const SgStatement *> StmtVec_t;

    public:
    RandCollector (StmtSet_t& s, size_t n)
      : OutlineableStmtCollector (s), max_ (n)
    {
    }

    void commit (void)
    {
      for (StmtVec_t::const_iterator i = keep_.begin ();
          i != keep_.end (); ++i)
        collect (const_cast<SgStatement *> (*i));
    }

    protected:
    //! Always returns false, but maintains separate candidate list.
    virtual bool matches (const SgStatement* s) const
    {
      if (max_) {
        if (keep_.empty ())
          keep_.push_back (s);
        else {
          size_t n_kept = keep_.size ();
          if (drand48 () < (1.0 / n_kept))
          {
            if (n_kept == max_) // evict
              keep_[lrand48 () % max_] = s;
            else // append
              keep_.push_back (s);
          }
        }
      }
      return false;
    }

    private:
    size_t max_; //!< Maximum no. of statements to collect.
    mutable StmtVec_t keep_; //!< Set of candidate statements.
  };

  // === Do collection. ===
  RandCollector collector (out_stmts, max_num_outline);
  collector.traverseInputFiles (root, preorder);
  collector.commit ();
}

// =====================================================================

#if 0 // JJW 10-17-2007 This is not used, but probably should be
static
void
usage (const string& prog_name)
{
  cerr << endl
       << "usage: " << prog_name
       << " [-rose:outline:random <n>]"
       << " [-rose:outline:seq <k>]"
       << " [-rose:outline:line <n>]"
       << " [-rose:outline:preproc-only]"
       << " [-rose:outline:emit-stages]"
       << " ..." << endl
       << endl;
}
#endif

static
bool
getOptions (vector<string>& argvList, ProgramOptions_t& opts)
{
  // Required: none.

  // Optional:
  if (CommandlineProcessing::isOption (argvList,
                                       "-rose:outline:",
                                       "preproc-only",
                                       true))
    {
      if (SgProject::get_verbose() > 0)
           cerr << "==> Running the outliner's preprocessing phase only." << endl;
      opts.preproc_only_ = true;
    }

  int verbosity_level = 0;
  if (CommandlineProcessing::isOptionWithParameter (argvList,
                                                    "-rose:", "verbose",
                                                    verbosity_level,
                                                    false)
      && verbosity_level >= 2)
    {
      if (SgProject::get_verbose() > 0)
           cerr << "==> Making PDFs." << endl;
      opts.make_pdfs_ = true;
    }

  if (CommandlineProcessing::isOption (argvList,
                                       "-rose:outline:", "emit-stages",
                                       true))
    {
      if (SgProject::get_verbose() > 0)
           cerr << "==> Emitting intermediate outlining results." << endl;
      opts.emit_stages_ = true;
    }

  int max_rand = 0;
  while (CommandlineProcessing::isOptionWithParameter (argvList,
						       "-rose:outline:",
						       "random",
						       max_rand,
						       true))
    if (max_rand > 0)
      opts.max_rand_ = max_rand;
    else
      opts.max_rand_ = 0;

    if (opts.max_rand_ > 0)
         if (SgProject::get_verbose() > 0)
              cerr << "==> Outlining " << opts.max_rand_ << " randomly-selected statements." << endl;

  int step = 0;
  while (CommandlineProcessing::isOptionWithParameter (argvList,
						       "-rose:outline:",
						       "seq",
						       step,
						       true))
    if (step > 0)
      {
        if (SgProject::get_verbose() > 0)
             cerr << "==> Outlining up to " << step << " statements." << endl;
        opts.seqs_.push_back (step);
      }
    else
      {
        if (SgProject::get_verbose() > 0)
             cerr << "*** WARNING: Ignoring invalid uplimit size, " << step << " ***" << endl;
      }

  int lineno = 0;
  while (CommandlineProcessing::isOptionWithParameter (argvList,
						       "-rose:outline:",
						       "line",
						       lineno,
						       true))
    if (lineno > 0)
      opts.lines_.push_back (ProgramOptions_t::SourcePos_t (string (""),lineno));
    else
      {
        if (SgProject::get_verbose() > 0)
             cerr << "*** WARNING: Ignoring bad line number, " << lineno << " ***" << endl;
      }
  
  // Args seem OK...
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

static
void
selectAllStmtsSeq (SgProject* proj,
                   const ProgramOptions_t::Steps_t& seqs,
                   StmtSet_t& targets)
{
  ProgramOptions_t::Steps_t::const_iterator i;
  for (i = seqs.begin (); i != seqs.end (); ++i)
    collectStmtsSeq (proj, *i, 10, targets);
}

static
void
selectAllStmtsPos (SgProject* proj,
                   const ProgramOptions_t::SourcePositions_t& lines,
                   StmtSet_t& targets)
{
  ProgramOptions_t::SourcePositions_t::const_iterator i;
  for (i = lines.begin (); i != lines.end (); ++i)
    {
      const string& filename = i->first;
      size_t line = i->second;
      collectStmtsAtPos (proj, filename, line, targets);
    }
}

static
void
selectAllStmtsRandom (SgProject* proj, size_t max_rand, StmtSet_t& targets)
{
  collectRandom (proj, max_rand, targets);
}

/*!
 *  \brief Selects up to the specified number of statements for
 *  outlining.
 *
 *  This routine returns a list of statements to outline, in the
 *  forward-iteration order in which they should be outlined.
 */
static
void
selectStatements (SgProject* proj, const ProgramOptions_t& opts,
                  StmtList_t& final_targets)
{
  //! Traversal to determine the order of the targets.
  class ReorderTraversal : public AstSimpleProcessing
  {
  public:
    ReorderTraversal (StmtSet_t& initial, StmtList_t& final)
      : initial_targets_ (initial),
        final_targets_ (final)
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
    StmtSet_t& initial_targets_;

    //! Final ordering of targets, in preorder traversal order.
    StmtList_t& final_targets_;
  };

  // Gather initial outline targets.
  StmtSet_t initial_targets;
  selectAllStmtsSeq (proj, opts.seqs_, initial_targets);
  selectAllStmtsPos (proj, opts.lines_, initial_targets);
  selectAllStmtsRandom (proj, opts.max_rand_, initial_targets);

  // Put them in the proper order for sequential outlining.
  ReorderTraversal reorderer (initial_targets, final_targets);
  reorderer.traverseInputFiles (proj, preorder);

  ROSE_ASSERT (initial_targets.size () == final_targets.size ());
}


// =====================================================================

//! Runs the outlining preprocessor on a single statement.
static
void
preprocess (SgStatement* s, bool emit = false)
{
  static size_t count = 0;

  if (SgProject::get_verbose() > 0)
       cerr << "=== PREPROCESSING: " << toString (s, ++count) << " ===" << endl;

  SgBasicBlock* b = Outliner::preprocess (s);
  ROSE_ASSERT (b);
#if 0 
//bug 151
  ASTtools::attachComment (toString (s, count), b);
#endif
  if (emit) // Unparse this intermediate result
    unparseIt (getRoot (b));
}

//! Outlines a single statement.
static
void
outline (SgStatement* s, bool emit = false)
{
  static size_t count = 0;

  if (SgProject::get_verbose() > 0)
       cerr << "=== OUTLINING: " << toString (s, ++count) << " ===" << endl;

  Outliner::Result r = Outliner::outline (s);
  ROSE_ASSERT (r.isValid ());
#if 0 
//bug 151
  ASTtools::attachComment (toString (s, count), r.decl_);
#endif
  if (emit)
    unparseIt (getRoot (s));
}

// =====================================================================

static
size_t
preprocessSelection (SgProject* proj, const ProgramOptions_t& opts)
{
  StmtList_t targets;
  selectStatements (proj, opts, targets);

  if (SgProject::get_verbose() > 0)
       dump (targets.begin (), targets.end ());

  for_each (targets.begin (), targets.end (),
            bind2nd (ptr_fun (preprocess), opts.emit_stages_));

  return targets.size ();
}

static
size_t
outlineSelection (SgProject* proj, const ProgramOptions_t& opts)
{
  StmtList_t targets;
  selectStatements (proj, opts, targets);

  if (SgProject::get_verbose() > 0)
       dump (targets.begin (), targets.end ());

  for_each (targets.begin (), targets.end (),
            bind2nd (ptr_fun (outline), opts.emit_stages_));

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
