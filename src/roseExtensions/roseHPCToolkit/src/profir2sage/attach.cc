// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/profir2sage/attach.cc
 *  \brief Implements attachment of metrics from an RoseHPCTIR tree
 *  to a Sage III tree.
 *  \ingroup ROSEHPCT_PROFIR2SAGE
 *
 *  See \ref attachIt for information on the basic attachment
 *  algorithm.
 *
 *  $Id: attach.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */
#include <rose.h>
#include <map>
#include <list>
#include <set>
#include "rosehpct/profir2sage/profir2sage.hh"
#include <iostream>
#include <sstream>
#include <string>

/* ---------------------------------------------------------------- */

static
bool
doFilenamesMatch (const SgLocatedNode* node, const std::string& filename)
{
  using std::string;
  string sg_fullpath = RoseHPCT::getFilename (node);
  string sg_dir = GenUtil::getDirname (sg_fullpath);
  string sg_file = GenUtil::getBaseFilename (sg_fullpath);

  string target_dir = GenUtil::getDirname (filename);
  string target_file = GenUtil::getBaseFilename (filename);

  return sg_dir == target_dir && sg_file == target_file;
}

static
bool
doLinesOverlap (const SgLocatedNode* node,
                size_t b_start, size_t b_end)
{
  if (node == NULL)
    return false;

  Sg_File_Info* info_start = node->get_startOfConstruct ();
  ROSE_ASSERT (info_start != NULL);

  size_t a_start = (size_t)info_start->get_line ();
  Sg_File_Info* info_end = node->get_endOfConstruct ();
  size_t a_end = (info_end == NULL) ? a_start : info_end->get_line ();

  if (a_end < a_start)
    a_end = a_start;
  if (b_end < b_start)
    b_end = b_start;

  return (b_start <= a_start && a_end <= b_end)
    || (a_start <= b_start && b_end <= a_end)
    || (a_start <= b_start && b_end <= a_end)
    || (b_start <= a_start && a_end <= b_end);
}

/* ---------------------------------------------------------------- */

/*!
 *  \brief Implements a RoseHPCTIR tree walk which searches for a metric
 *  that matches the given Sage node.
 */
class MetricFinder : public Trees::Traversal<RoseHPCT::IRTree_t, true>
{
public:
  typedef std::set<const RoseHPCT::IRNode *> MatchSet_t;

  /*! \brief Initialize, specifying the target node. */
  MetricFinder (const SgLocatedNode* target);
  virtual ~MetricFinder (void) {}

  /*! \brief Enable verbose messaging during traversal. */
  void setVerbose (bool enable = false);

  /*! \brief Performs the search. */
  virtual void traverse (TreeParamPtr_t tree);

  /*! \brief Returns 'true' if a match found. */
  bool found (void) const;

  /*! \brief Returns a list of matching nodes. */
  const MatchSet_t& getMatches (void) const;

protected:

  MetricFinder (void);

  virtual void visit (TreeParamPtr_t tree);

  /*! \brief Handles File nodes. */
  virtual void visit (const RoseHPCT::File* node);
  /*! \brief Handles Procedure nodes. */
  virtual void visit (const RoseHPCT::Procedure* node);
  /*! \brief Handles Loop nodes. */
  virtual void visit (const RoseHPCT::Loop* node);
  /*! \brief Handles Statement nodes. */
  virtual void visit (const RoseHPCT::Statement* node);

  bool doLinesOverlap (size_t a, size_t b) const;

private:
  const SgLocatedNode* target_; //!< Node to find
  MatchSet_t matches_; //!< All matching ProfIR nodes
  bool verbose_; //!< True <==> verbose messaging desired by caller
  bool prune_branch_; //!< Indicates a tree branch may be pruned from search

protected:
  //! \name Query target node for Sage type information
  //@{
  const SgStatement* nonscope_stmt_target_; //! Cache cast of non-SgScopeStatement target
  bool isTargetSgGlobal (void) const;
  bool isTargetSgProcedure (void) const;
  bool isTargetSgLoop (void) const;
  bool isTargetSgStatementNonScope (void) const;
  //@}
};

/* ---------------------------------------------------------------- */

typedef std::set<SgLocatedNode *> SgLocNodeSet_t;

//! Implements a pass over the AST that attaches metrics as attributes.
/*!
 *  This pass simply attaches a metric to all matching statements.
 *  If HPCToolkit reports 100 cycles for a source line consisting
 *  of four distinct statements, e.g.,
 *    a[0] = 0; a[1] = 1; a[2] = 2; a++;
 *  then this pass will assign 100 to all four statements.
 *
 *  To normalize the metrics correctly, see the normalizeMetrics
 *  routine.
 */
class MetricAttachTraversal : public AstSimpleProcessing
{
public:
  MetricAttachTraversal (const RoseHPCT::IRTree_t* hpc_root);
  void setVerbose (bool enable = false);
  //a map between profile IR nodes and their matching AST nodes
  //note that a profile IR node may corresponding multiple AST nodes(a code range)
  typedef std::map<const RoseHPCT::IRNode *, SgLocNodeSet_t> AttachedNodes_t;
  AttachedNodes_t& getAttachedNodes (void);
  //! Insert the metrics info. into source as comments, mostly for debugging
  void annotateSourceCode(void);
protected:
  MetricAttachTraversal (void);
  virtual void visit (SgNode* n);

private:
  const RoseHPCT::IRTree_t* hpc_root_; //!< Root of profiling data tree
  AttachedNodes_t attached_; //!< Lists of attached nodes
  bool verbose_; //!< Enable verbose messaging
};

/* ---------------------------------------------------------------- */


using namespace std;
using namespace RoseHPCT;

/* ---------------------------------------------------------------- */

MetricFinder::MetricFinder (void)
  : target_ (0), verbose_ (false), prune_branch_ (false),
    nonscope_stmt_target_ (0)
{
}

MetricFinder::MetricFinder (const SgLocatedNode* target)
  : target_ (target), verbose_ (false), prune_branch_ (false),
    nonscope_stmt_target_ (0)
{
  if (!dynamic_cast<const SgScopeStatement *> (target_))
    nonscope_stmt_target_ = dynamic_cast<const SgStatement *> (target_);
}

void
MetricFinder::setVerbose (bool enable)
{
  verbose_ = enable;
}

/*!
 *  \brief
 *
 *  Implements a preorder traversal of the tree, with an early exit if
 *  a matching node is found, or if an entire subtree may be pruned
 *  from the search.
 */
void
MetricFinder::traverse (TreeParamPtr_t tree)
{
  if (tree == NULL)
    return;

  visit (tree);
  if (!found ())
    {
      if (!prune_branch_)
        {
          this->descending ();
          for (ChildIter_t i = tree->beginChild (); i != tree->endChild (); ++i)
            traverse (i->second);
        }
      else
        prune_branch_ = false; // Reset
    }
  this->ascending ();
}

bool
MetricFinder::found (void) const
{
  return !matches_.empty ();
}

const MetricFinder::MatchSet_t&
MetricFinder::getMatches (void) const
{
  return matches_;
}

bool
MetricFinder::isTargetSgGlobal (void) const
{
  return target_ ? (target_->variantT () == V_SgGlobal) : false;
}

bool
MetricFinder::isTargetSgProcedure (void) const
{
  return target_ ? (target_->variantT () == V_SgFunctionDefinition) : false;
}

bool
MetricFinder::isTargetSgLoop (void) const
{
  if (target_)
    switch (target_->variantT ())
      {
      case V_SgForStatement:
      case V_SgWhileStmt:
      case V_SgDoWhileStmt:
        return true;
      default:
        break;
      }
  return false; // Default: not a loop
}

bool
MetricFinder::isTargetSgStatementNonScope (void) const
{
  return nonscope_stmt_target_ != 0;
}

bool
MetricFinder::doLinesOverlap (size_t a, size_t b) const
{
  return ::doLinesOverlap (target_, a, b);
}

void
MetricFinder::visit (TreeParamPtr_t tree)
{
  visit (dynamic_cast<const File *> (tree->value));
  visit (dynamic_cast<const Procedure *> (tree->value));
  visit (dynamic_cast<const Loop *> (tree->value));
  visit (dynamic_cast<const Statement *> (tree->value));
}

void
MetricFinder::visit (const File* f)
{
  if (f)
    {
      if (!doFilenamesMatch (target_, f->getName ()))
        prune_branch_ = true;
      else if (isTargetSgGlobal ()) // looking for a file (global) scope
        {
          matches_.insert (f);
          prune_branch_ = true;
        }
    }
}

void
MetricFinder::visit (const Procedure* p)
{
  if (p && isTargetSgProcedure ()
      && doLinesOverlap (p->getFirstLine (), p->getLastLine ()))
    {
      matches_.insert (p);
      prune_branch_ = true;
    }
}

void
MetricFinder::visit (const Loop* l)
{
  if (l && isTargetSgLoop ()
      && doLinesOverlap (l->getFirstLine (), l->getLastLine ()))
    {
      matches_.insert (l);
      prune_branch_ = true;
    }
}

void
MetricFinder::visit (const Statement* s)
{
  if (s && isTargetSgStatementNonScope ()
      && doLinesOverlap (s->getFirstLine (), s->getLastLine ()))
    {
      matches_.insert (s);
      prune_branch_ = true;
    }
}

/* ---------------------------------------------------------------- */

/*!
 *  Print a message to stdout indicating a metric was successfully
 *  attached to a Sage IR node (only when verbose message enabled).
 */
static
void
notifyAttached (SgLocatedNode* node,
		const std::string& attr_name)
{
  if (node == NULL) return;

  MetricAttr* attr = dynamic_cast<MetricAttr *> (node->getAttribute (attr_name));

  cerr << "[" << toFileLoc (node) << "]"
       << "  " << attr_name << " = " << attr->toString ()
       << endl;
}

/*!
 *  \brief Attaches a given attribute to a given Sage node.
 */
template <class SgType>
static
bool
attachOne (SgType* node, const string& attr_name, MetricAttr* attr,
	   bool verbose = false)
{
  if (node == NULL) return false;
  if (node->attributeExists (attr_name))
    {
      AstAttribute* old_attr_raw = node->getAttribute (attr_name);
      MetricAttr* old_attr = dynamic_cast<MetricAttr *> (old_attr_raw);
      ROSE_ASSERT (old_attr);
      attr->setValue (attr->getValue () + old_attr->getValue ());
      node->updateAttribute (attr_name, attr);
    }
  else // node does not already have an attribute named attr_name
    node->setAttribute (attr_name, attr);
  if (verbose)
    notifyAttached (node, attr_name);
  return true;
}

/*! \brief Maps tree depths to counts */
typedef std::map<size_t, size_t> DepthMap_t;

/*!
 *  \brief Returns the tree depth attribute of a node, or 0 if none is
 *  defined.
 *
 *  This routine assumes the attribute name given by
 *  TreeDepthCalculator::ATTR_NAME.
 */
static
size_t
getTreeDepthAttr (const SgNode* node)
{
  if (node == NULL) return 0;

  const string& ATTR_NAME = TreeDepthCalculator::ATTR_NAME;
  size_t depth;

  if (((SgNode *)node)->attributeExists (ATTR_NAME))
    {
      AstAttribute* attr = ((SgNode *)node)->getAttribute (ATTR_NAME);
      const DepthAttr* d_node = dynamic_cast<const DepthAttr *> (attr);
      ROSE_ASSERT (d_node != NULL);
      depth = d_node->depth;
    }
  else
    depth = 0;
  return depth;
}

/*!
 *  Special helper function to be called for SgFunctionDefinition
 *  nodes which attaches the specified metric to the body of the
 *  function definition.
 */
static
void
attachPostFunctionDefinition (SgFunctionDefinition* func,
                              const MetricAttr* attr_sage,
                              bool verbose = false)
{
  if (func && attr_sage)
    {
      SgBasicBlock* body = func->get_body ();
      attachOne (body, attr_sage->getName (), attr_sage->clone (), verbose);
    }
}

/*!
 *  Special helper function to be called for SgScopeStatement
 *  nodes which attaches the specified metric to the body of the
 *  scope statement.
 *
 *  This function is enabled by defining the compile-time macro,
 *  'DO_ATTACH_LOOP_BODIES'.
 */
static
void
attachPostScopeStatement (SgScopeStatement* loop,
                          const MetricAttr* attr_sage,
                          bool verbose = false)
{
#if defined(DO_ATTACH_LOOP_BODIES)
  if (loop && attr_sage)
    {
      SgBasicBlock* body = NULL;

      if (body == NULL)
        {
          SgForStatement* L = isSgForStatement (loop);
          if (L != NULL)
            body = L->get_loop_body ();
        }
  
      if (body == NULL)
        {
          SgWhileStmt* L = isSgWhileStmt (loop);
          if (L != NULL)
            body = L->get_body ();
        }

      if (body == NULL)
        {
          SgDoWhileStmt* L = isSgDoWhileStmt (loop);
          if (L != NULL)
            body = L->get_body ();
        }

      if (body != NULL)
        attachOne (body, attr_sage->getName (), attr_sage->clone (), verbose);
    }
#endif
}

/* ---------------------------------------------------------------- */

MetricAttachTraversal::MetricAttachTraversal (void)
  : hpc_root_ (0), verbose_ (false)
{
}

MetricAttachTraversal::MetricAttachTraversal (const IRTree_t* hpc_root)
  : hpc_root_ (hpc_root), verbose_ (false)
{
}

void
MetricAttachTraversal::setVerbose (bool enable)
{
  verbose_ = enable;
}

MetricAttachTraversal::AttachedNodes_t &
MetricAttachTraversal::getAttachedNodes (void)
{
  return attached_;
}

void
MetricAttachTraversal::visit (SgNode* n)
{
  SgLocatedNode* n_loc = isSgLocatedNode (n);
  if (n_loc)
    {
      MetricFinder finder (n_loc);
      finder.setVerbose (verbose_);
      finder.traverse (hpc_root_);
      if (finder.found ())
        {
          MetricFinder::MatchSet_t matches = finder.getMatches ();
          for (MetricFinder::MatchSet_t::const_iterator hpc_match = matches.begin ();
               hpc_match != matches.end (); ++hpc_match)
            {
              ROSE_ASSERT (*hpc_match);
              attached_[*hpc_match].insert (n_loc);
              for (Observable::ConstMetricIterator m = (*hpc_match)->beginMetric ();
                   m != (*hpc_match)->endMetric (); ++m)
                {
                  string attr_name = m->getName ();
                  double attr_value = m->getValue ();
                  MetricAttr* attr_sage = new MetricAttr (attr_name, attr_value);
                  attachOne (n_loc, attr_name, attr_sage, verbose_);

                  if (dynamic_cast<const Loop *> (*hpc_match))
                    attachPostScopeStatement (isSgScopeStatement (n_loc),
                                              attr_sage, verbose_);
                  if (dynamic_cast<const Statement *> (*hpc_match))
                    attachPostFunctionDefinition (isSgFunctionDefinition (n_loc),
                                                  attr_sage, verbose_);
                }
            }
        }
    }
}
//! 
void MetricAttachTraversal::annotateSourceCode(void)
{
  AttachedNodes_t::const_iterator iter = attached_.begin();
  for (;iter!=attached_.end();iter++)
  {
    const RoseHPCT::IRNode * profileNode = (*iter).first;
    SgLocNodeSet_t sgNodeSet=(*iter).second;
    SgLocNodeSet_t::iterator iter2 = sgNodeSet.begin();
    for (;iter2!=sgNodeSet.end();iter2++)
    {
      SgLocatedNode* node = *iter2;
      std::ostringstream o;
      o<<node;
      std::string text = "\n/* HPCToolKit raw data: " +profileNode->toString();
      text += " -> "+ string(node->sage_class_name()) + " " + o.str() + "*/\n"; 
      // attach the text before the node (node is e_after the text)
      SageInterface::addTextForUnparser(node,text,AstUnparseAttribute::e_after);
#if 0
      // Not all SgLocatedNode can have comments, so we use arbitrary text support instead
      if ((isSgForInitStatement(node))||isSgTypedefSeq(node)||isSgCtorInitializerList(node))
      {
        cerr<<"Warning: annotation for the following Node is not allowed"<<endl;
        cerr<<profileNode->toString()<<endl;
        cerr<<node->sage_class_name()<<node->unparseToString()<<endl;
      }
      else
      {  
        SageInterface::attachComment(node, text);
      }
#endif      
    } 
  }
}
/* ---------------------------------------------------------------- */

//! Normalizes AST metrics.
/*!
 *  A normalization pass over the AST ensures that metrics assigned to
 *  multiple nodes at the same depth in the AST sum to the original
 *  raw metric value. For example, suppose the HPCToolkit reports 100
 *  cycles for a source line consisting four distinct statements:
 *
 *     a[0] = 0; a[1] = 1; a[2] = 2; a++;
 *
 *  The MetricAttachTraversal will assign 100 to each of the
 *  statements. This pass will assign each statement the value of 25,
 *  so the total sums to the original metric value of 100.
 */
static
void
normalizeMetrics (SgProject* sage_root,
                  MetricAttachTraversal::AttachedNodes_t& attached,
                  bool verbose = false)
{
  if (verbose)
    cerr << "[Normalizing metrics...]" << endl;

  // Compute the depth of each node
  TreeDepthCalculator dcalc (sage_root);

  // Normalize
  MetricAttachTraversal::AttachedNodes_t::iterator ir_node;
  for (ir_node = attached.begin (); ir_node != attached.end (); ++ir_node)
    {
      // Extract key-value pair, <i, sg_nodes>
      const IRNode* i = ir_node->first;
      SgLocNodeSet_t& sg_nodes = ir_node->second;

      // Build a map, depth_map[k] == # of nodes at depth k
      using std::map;
      map<size_t, size_t> depth_map;
      for (SgLocNodeSet_t::iterator np = sg_nodes.begin ();
           np != sg_nodes.end (); ++np)
        {
          size_t depth = getTreeDepthAttr (*np);
          depth_map[depth]++;
        }

      // Normalize attributes
      for (Observable::ConstMetricIterator m = i->beginMetric ();
           m != i->endMetric (); ++m)
        {
          string attr_name = m->getName ();
          for (SgLocNodeSet_t::iterator np = sg_nodes.begin ();
               np != sg_nodes.end (); ++np)
            {
              MetricAttr* attr_sage = getMetric (attr_name, *np);
              ROSE_ASSERT (attr_sage);

              size_t depth = getTreeDepthAttr (*np);
              size_t n_matches = depth_map[depth];
              if (n_matches > 1)
                {
                  double old_val = attr_sage->getValue ();
                  double new_val = old_val / n_matches;
                  attr_sage->setValue (new_val);

                  if (verbose)
                    cerr << "[Normalizing by " << n_matches
                         << " at depth " << depth
                         << ": [" << toFileLoc (*np) << "]"
                         << " " << attr_name
                         << " (" << old_val << ")"
                         << " -> " << attr_sage->toString ()
                         << "]" << endl;
                }
            }
        }
    }
}

/* ---------------------------------------------------------------- */

//! Stores a list of metric names.
typedef set<std::string> MetricNames_t;

//! Returns a set of all possible metric names.
static
size_t
gatherMetricNames (const MetricAttachTraversal::AttachedNodes_t& attached,
                   MetricNames_t& names)
{
  for (MetricAttachTraversal::AttachedNodes_t::const_iterator i =
         attached.begin (); i != attached.end (); ++i)
    {
      const IRNode* node = i->first;
      if (node)
        for (Observable::ConstMetricIterator m = node->beginMetric ();
             m != node->endMetric (); ++m)
          names.insert (m->getName ());
    }
  return names.size ();
}

/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
//! Implements a pass over the AST to fix-up parent/child metrics.
/*!
 *  For each AST node v with a metric attribute m(v), this pass
 *  enforces the condition that m(v) <= m(parent(v)). 
 *  It set child node's metric value to its parent node's value
 *  if m(v) > m(parent(v))
 *  This pass is a companion to normalizeMetrics(), and should be called
 *  _afterwards_.
 *
 *  TODO any metrics become smaller after aggregation??
 */
class MetricFixupTraversal : public AstSimpleProcessing
{
public:
  MetricFixupTraversal (const MetricNames_t& names, bool verbose = false);
  void setVerbose (bool enable = false) { verbose_ = false; }

protected:
  MetricFixupTraversal (void) : names_ (0), verbose_ (false) {}
  virtual void visit (SgNode* n);

private:
  const MetricNames_t* names_; //!< Names of metrics to check.
  bool verbose_; //!< Enable verbose messaging
};

MetricFixupTraversal::MetricFixupTraversal (const MetricNames_t& names,
                                            bool verbose)
  : names_ (&names), verbose_ (verbose)
{
}

void
MetricFixupTraversal::visit (SgNode* n)
{
  SgNode* par = n->get_parent ();
  if (names_ && par)
    {
      for (MetricNames_t::const_iterator name = names_->begin ();
           name != names_->end (); ++name)
        {
          if (n->attributeExists (*name) && par->attributeExists (*name))
            {
              MetricAttr* m_n =
                dynamic_cast<MetricAttr *> (n->getAttribute (*name));
              const MetricAttr* m_par =
                dynamic_cast<const MetricAttr *> (par->getAttribute (*name));
              ROSE_ASSERT (m_n && m_par);
              // synchronize a node's performance metric value with its parent node's value
              // should be <= parent_value
              if (m_n->getValue () > m_par->getValue ())
                {
                  m_n->setValue (m_par->getValue ());
                  n->updateAttribute (*name, m_n);
                }
            }
        }
    }
}

/* ---------------------------------------------------------------- */
void
RoseHPCT::attachMetrics (const IRTree_t* hpc_root, SgProject* sage_root,
			bool verbose)
{
  MetricAttachTraversal attacher (hpc_root);
  attacher.setVerbose (verbose);
  attacher.traverse (sage_root, preorder);
  if (verbose)
    attacher.annotateSourceCode();

  MetricAttachTraversal::AttachedNodes_t& attached
    = attacher.getAttachedNodes ();
  normalizeMetrics (sage_root, attached, verbose);

  MetricNames_t metrics;
  gatherMetricNames (attached, metrics);
  MetricFixupTraversal fixer (metrics, verbose);
  fixer.traverse (sage_root, preorder);
}

/* eof */
