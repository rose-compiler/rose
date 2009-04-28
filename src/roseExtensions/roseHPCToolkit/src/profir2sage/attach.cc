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
#include <iostream>
#include <sstream>
#include <string>
#include "rosehpct/rosehpct.hh"
//#include "rosehpct/profir2sage/profir2sage.hh"
/* ---------------------------------------------------------------- */
namespace RoseHPCT
{
  //! A quick reference to all file and non-stmt nodes of the original Profile IR trees
  // This gives a global view of where to find hot portions
  std::set<const RoseHPCT::IRNode *> profFileNodes_;	
  std::set<const RoseHPCT::IRNode *> profStmtNodes_;	
}

//! Tell if a Sage node's file info. matches a given filename
// A match means both path and name match
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

//! Check if the code portions specified by Sage Node and PROF ir'S located node [b-start, b_end]
// have any kind of overlap 
static
bool
doLinesOverlap (const SgLocatedNode* node,
                const RoseHPCT::Located * plnode)
 //                size_t b_start, size_t b_end)
{
  ROSE_ASSERT(plnode);
  if (node == NULL)
    return false;
  size_t b_start = plnode->getFirstLine ();
  size_t b_end = plnode->getLastLine ();

  Sg_File_Info* info_start = node->get_startOfConstruct ();
  ROSE_ASSERT (info_start != NULL);
  // check file name first if possible
  if (plnode->getFileNode())    
  {
    if (plnode->getFileNode()->getName()!= info_start->get_filename())
      return false;
  }
  // Then check for line numbers  
  size_t a_start = (size_t)info_start->get_line ();
  Sg_File_Info* info_end = node->get_endOfConstruct ();
  size_t a_end = (info_end == NULL) ? a_start : info_end->get_line ();
  // adjust for wrong line information, See Bugs-Internal 311
  // some non-scope statement's end line numbers are not correct from ROSE
  // especially for SgForInitStatement and loop condition statements
  // We adjust their end line info. to be the same as the beginning line numbers
  // This is generally risky for ROSE, but should work for ROSE-HPCT 
  // since the HPCToolkit generates metrics specified with beginning line only 
  // Liao, 2/2/2009
  if ((a_end < a_start)||(isSgStatement(node)&&!(isSgScopeStatement(node))))
  {
    a_end = a_start;
    //ROSE_ASSERT(false); // should not happen for ROSE, but live with it for now
  }
  if (b_end < b_start)
  {
    b_end = b_start;
    //ROSE_ASSERT(false); // Some metrics location information from HPCToolkit are buggy
  }

  return (b_start <= a_start && a_end <= b_end) // SgNode's file portion is a subset of Profile's information 
    || (a_start <= b_start && b_end <= a_end);  // Profile's info is a subset of SgNode's portion
//    || (a_start <= b_start && b_end <= a_end)  // redundant condition? TODO should be partial overlap
//     like (b_start <= a_start && b_end <= a_end)
//    || (b_start <= a_start && a_end <= b_end);
//    // or (a_start <= b_start && a_end <= b_end);??
}

/* ---------------------------------------------------------------- */

/*!
 *  \brief Implements a RoseHPCTIR tree walk which searches for a metric
 *  that matches the given Sage node.
 *  true means the traversal is read-only
 */
class MetricFinder : public Trees::Traversal<RoseHPCT::IRTree_t, true>
{
public:
  typedef std::set<const RoseHPCT::IRNode *> MatchSet_t;

  /*! \brief Initialize, specifying the target node, and a pointer to a record for matched IRNode to  SgNode to avoid redundant attachment*/
  MetricFinder (const SgLocatedNode* target, std::map<const RoseHPCT::IRNode *, std::set<SgLocatedNode *> >  * historyRecord);
  virtual ~MetricFinder (void) {}

  /*! \brief Enable verbose messaging during traversal. */
  void setVerbose (bool enable = false);

  /*! \brief Performs the search. */
  virtual void traverse (TreeParamPtr_t tree);

  /*! \brief Returns 'true' if a match found. */
  bool found (void) const;

  /*! \brief Returns a set of matching nodes. */
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

  bool doLinesOverlap (const RoseHPCT::Located* l) const;
  //bool doLinesOverlap (size_t a, size_t b) const;

  /*! \brief Check if the target node is a shadow node of 
   * the previously matched nodes to a profile node */
 bool isShadowNode(const RoseHPCT::IRNode* profNode) const; 

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
  std::map<const RoseHPCT::IRNode *, std::set<SgLocatedNode *> >  * historyRecord_; //! A map between IRNode to matched SgNodes, used to avoid redundant attaching for code expanded from macro, Liao, 2/3/2009


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
#if 0  
  std::set<const RoseHPCT::IRNode *>& getProfFileNodes (void);
  std::set<const RoseHPCT::IRNode *>& getProfStmtNodes (void);
#endif  
  //! Insert the metrics info. into source as comments, mostly for debugging
  void annotateSourceCode(void);
protected:
  MetricAttachTraversal (void);
  virtual void visit (SgNode* n);

private:
  const RoseHPCT::IRTree_t* hpc_root_; //!< Root of profiling data tree
  AttachedNodes_t attached_; //!< Lists of attached nodes
#if 0  
  std::set<const RoseHPCT::IRNode *> files_; //!< set of all profir file nodes
  std::set<const RoseHPCT::IRNode *> stmts_; //!< set of all profir statement nodes
#endif  
  bool verbose_; //!< Enable verbose messaging
};

/* ---------------------------------------------------------------- */


using namespace std;
using namespace RoseHPCT;

/* ---------------------------------------------------------------- */

MetricFinder::MetricFinder (void)
  : target_ (0), verbose_ (false), prune_branch_ (false),
    nonscope_stmt_target_ (0),historyRecord_(0)
{
}

MetricFinder::MetricFinder (const SgLocatedNode* target,std::map<const RoseHPCT::IRNode *, SgLocNodeSet_t>  * historyRecord)
  : target_ (target), verbose_ (false), prune_branch_ (false),
    nonscope_stmt_target_ (0),historyRecord_(historyRecord)
{
#if 0  // not in use since it messes up metrics normalization by leaving a metric gap in AST
  //! SgForInitStatement is special, it is not under scope statement but should be
  //Liao, 2/2/2009
  if (dynamic_cast<const SgForInitStatement*> (target_))
    nonscope_stmt_target_ = 0;
  else  
#endif    
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
#if 0
const MetricFinder::MatchSet_t&
MetricFinder::getFiles(void) const
{
  return files_;
}
const MetricFinder::MatchSet_t&
MetricFinder::getStmts(void) const
{
  return stmts_;
}
#endif
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
MetricFinder::doLinesOverlap (const RoseHPCT::Located * l) const
{
  return ::doLinesOverlap (target_, l);
  //return ::doLinesOverlap (target_, a, b);
}

//! Check if the target SgNode is a shadow node of a previously  matched 
// SgNode to the current RoseHPCT::IRNode (profNode) node. 
//
// This is used to help avoid redundant metric attachment to 
// both parent and child nodes expanded from a single line of macro.
// It takes advantage of the preorder traversal to attach a metric only once
// on the highest level of AST node matching the source line, 
// all other nodes below the highest level of node are called shadow nodes 
// and are ignored.  If we have multiple nodes at the same highest level,
// we still attach them all and let the Normalization phase to adjust metrics 
// later.
//
// The original code naively attached performance metrics to all AST nodes 
// with matching source line numbers,
// which caused problems when an AST subtree is expanded from a single line macro call.
// In this case, all nodes in the expanded AST tree will have the same source line number as the macro
// and they all got the same metrics attached. A later metric propagation phase would wrongfully 
// accumulate the redundant metrics within the subtree and got very off metrics in the end. 
//
// Limitations: Some crazy macros used in SMG2000 are expanded to both loop control headers 
// and part of the loop body. This shadow node concept only helps get the correct final accumulated metrics
// and cannot attribute the metrics to headers and partial loop body (the performance tool does 
// not provide sufficient information neither). 
// In fact, the metrics of the crazy macros are attached to loop headers only since loop headers 
// are visited before the loop body.
//
// TODO A final top-down propagation phase may be needed to attribute the top level metric further
// down the children and grandchildren nodes. But We don't see the need yet right now.
// Liao, 2/3/2009
bool 
MetricFinder::isShadowNode(const RoseHPCT::IRNode* profNode) const
{
  ROSE_ASSERT(historyRecord_!=NULL);
  ROSE_ASSERT(profNode!=NULL);

  bool result=false;
  if ((*historyRecord_)[profNode].size()==0)
    result = false; // not record at all, not shadowed
  else
  {
    SgNode* existing_node= *((*historyRecord_)[profNode].begin());
    if (existing_node->get_parent() == target_->get_parent())
      result = false; // same level in tree path, not shadowed
    else
      result = true; // all other cases mean shadowed, 
      //either a child-parent relationships or different tree paths
  }
  return result;
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
#if 0  // moved to pathFinder traversal   
    //take advantage of this traversal to accumulate file nodes
    RoseHPCT::profFileNodes_.insert(f);
#endif    
    if (!doFilenamesMatch (target_, f->getName ()))
      prune_branch_ = true;
    else  // match file name
    {
      if (isTargetSgGlobal () )
      // match SgNode type: looking for a file (global) scope
      {
        matches_.insert (f);
        prune_branch_ = true;
      }
    }
  }
}

void
MetricFinder::visit (const Procedure* p)
{
  if (p && isTargetSgProcedure ()
      && doLinesOverlap (p))
      //&& doLinesOverlap (p->getFirstLine (), p->getLastLine ()))
    {
      matches_.insert (p);
      prune_branch_ = true;
    }
}

void
MetricFinder::visit (const Loop* l)
{
  if (l && isTargetSgLoop ()
      && doLinesOverlap (l))
      //&& doLinesOverlap (l->getFirstLine (), l->getLastLine ()))
    {
      matches_.insert (l);
      prune_branch_ = true;
    }
}

void
MetricFinder::visit (const Statement* s)
{
  if (s)
  {
#if 0    
    //cout<<"found a statement prof IR node:"<<s->toString()<<endl;
    //  take advantage of this traversal to accumulate statement nodes   
    //  This traversal can abort once a match is found so one execution will not collect all stmt nodes.
    RoseHPCT::profStmtNodes_.insert(s);
#endif    
    if (isTargetSgStatementNonScope ())
    { 
      if (doLinesOverlap (s))
      //if (doLinesOverlap (s->getFirstLine (), s->getLastLine ()))
      {
        if (!isShadowNode(s))
          // We only consider shadow SgNodes for statement profile results for now
        {
          matches_.insert (s);
          prune_branch_ = true;

        }
      }
    }
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
 cerr<<node->unparseToString()<<endl;      
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
#if 0
std::set<const RoseHPCT::IRNode *>&
MetricAttachTraversal::getProfFileNodes(void)
{
  return files_;
}
std::set<const RoseHPCT::IRNode *>&
MetricAttachTraversal::getProfStmtNodes(void)
{
  return stmts_;
}
#endif
void
MetricAttachTraversal::visit (SgNode* n)
{
  static int exe_counter=0; // used to collect file nodes of profir only once
  SgLocatedNode* n_loc = isSgLocatedNode (n);
  if (n_loc)
    {
      MetricFinder finder (n_loc,&attached_);
      finder.setVerbose (verbose_);
      // for current SgNode, find matching prof ir nodes
      finder.traverse (hpc_root_);
      // for collecting File nodes of profile IR
      exe_counter++;
#if 0         
      if (exe_counter==1) 
       {
         // store them into MetricAttachTraversal::files
         files_ = finder.getFiles();
         stmts_ = finder.getStmts();
         cout<<"MetricAttachTraversal::visit() obtained stmts from MetricFinder:count="<<stmts_.size()<<endl;
       } 
#endif         
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
      o<<node <<" at "<<node->get_startOfConstruct()->get_line();
      std::string text = "\n/* ROSE-HPCT raw data: " +profileNode->toString();
      text += " -> "+ string(node->sage_class_name()) + " " + o.str() + " */\n"; 
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
 *     Another example is loop headers
 *     for (i=0;i<100; i++)
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
          {
            cerr << "[Normalizing by " << n_matches
              << " at depth " << depth
              << ": [" << toFileLoc (*np) << "]"
              << " " << attr_name
              << " (" << old_val << ")"
              << " -> " << attr_sage->toString ()
              << "]" << endl;
            // Annotate source code for debugging
              std::ostringstream o;
              o << "\n/* ROSE-HPCT normalizes it by " << n_matches
              << " at depth " << depth
              << " " << attr_name
              << " old (" << old_val << ")"
              << " -> new " << attr_sage->toString ()
              << ": [" << (*np)->sage_class_name()<< " "<<(*np) << "]"
              << "*/\n"; 
             SageInterface::addTextForUnparser(*np,o.str(), AstUnparseAttribute::e_after); 
          }
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
// attach a profile ir tree to a sage project
void
RoseHPCT::attachMetrics (const IRTree_t* hpc_root, SgProject* sage_root,
			bool verbose)
{
  MetricAttachTraversal attacher (hpc_root);
  attacher.setVerbose (verbose);
  attacher.traverse (sage_root, preorder);
  if (enable_debug)
  {  
    attacher.annotateSourceCode();
  }
#if 0 // We use namespace level variables to accumulate them now
  // Accumulate file metrics, used to find hot files later on
  std::set<const RoseHPCT::IRNode *>&files = attacher.getProfFileNodes();
  std::set<const RoseHPCT::IRNode *>::iterator iter= files.begin();
  for (;iter!=files.end(); iter++)
   profFileNodes_.insert(*iter); 
  // Accumulate non-scope stmt metrics, used to find hot stmts later on
  std::set<const RoseHPCT::IRNode *>&stmts= attacher.getProfStmtNodes();
  std::set<const RoseHPCT::IRNode *>::iterator iter2= stmts.begin();
  cout<<"Store stmt prof IR nodes of count="<<stmts.size()<<endl;
  for (;iter2!=stmts.end(); iter2++)
  {
   profStmtNodes_.insert(*iter2); 
  }
#endif
  MetricAttachTraversal::AttachedNodes_t& attached
    = attacher.getAttachedNodes ();
  normalizeMetrics (sage_root, attached, verbose);

  MetricNames_t metrics;
  gatherMetricNames (attached, metrics);
  MetricFixupTraversal fixer (metrics, verbose);
  fixer.traverse (sage_root, preorder);
}

/* eof */
