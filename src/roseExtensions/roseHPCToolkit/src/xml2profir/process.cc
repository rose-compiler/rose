// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml2profir/process.cc
 *  \brief Process command-line options and files.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  Command-line processing routines use Brian Gunney's SLA library.
 *
 *  $Id: process.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */
#include "rose.h"
//#include "rosehpct/xml2profir/xml2profir.hh"
#include "sla.h"
//#include "rosehpct/util/general.hh"
#include "rosehpct/rosehpct.hh"

using namespace RoseHPCT;

/* ------------------------------------------------------------ */

/*!
 *  \brief Class to translate short-hand metric names to full names.
 */
class MetricTranslator : public Trees::PreTraversal<IRTree_t, false>
{
public:
  typedef std::map<std::string, std::string> RenameMap_t;

  /*! \brief Initialize with a specific translation map */
  MetricTranslator (const RenameMap_t& mettab);
  virtual ~MetricTranslator (void);

  /*! \brief Returns 'true' if any paths have been translated. */
  bool didChange (void) const;

protected:
  /*! \brief Only visits observable nodes */
  virtual void visit (TreeParamPtr_t tree);

  /*! \brief Observable-node specific translator */
  virtual void visitObservable (Observable* node);

private:
  /*! \brief Associative map of metric name translations */
  std::map<std::string, std::string> metrics_;

  /*! \brief True <==> any metric names were translated during traversal */
  bool did_change_;
};

/* ------------------------------------------------------------ */

/*!
 *  \brief Class to collect metric names.
 */
class MetricCollector : public Trees::PreTraversal<IRTree_t>
{
public:
  //! Map of metrics to values
  typedef std::map<std::string, size_t> MetricMap_t;

  MetricCollector (MetricMap_t& metric_store);

protected:
  //! Visits Observable nodes only.
  virtual void visit (TreeParamPtr_t tree);

  //! Observable node-specific visit.
  virtual void visitObservable (Observable* node);

private:
  //! List of metric names.
  MetricMap_t& metrics_;
};

/* ------------------------------------------------------------ */

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;

/* ---------------------------------------------------------------- */

ProgramTreeList_t
RoseHPCT::loadXMLTrees (const FilenameList_t& filenames)
{
  ProgramTreeList_t trees;
  for (FilenameList_t::const_iterator i = filenames.begin ();
       i != filenames.end ();
       ++i)
    {
      cerr << "[Reading profile: " << (*i) << "]" << endl;
      XMLParser p (*i);
      if (p.getDoc () != NULL) // valid file
	{ 
          // Collect all metrics available in the xml input
	  map<string, string> mettab = XMLQuery::getMetricTable (p.getDoc ());
	  IRTree_t* prog_root = convertXMLToProfIR (p.getDoc ());
	  translateMetricNames (prog_root, mettab);
	  trees.push_back (prog_root);
	}
    }
  return trees;
}

/* ---------------------------------------------------------------- */

/*!
 *  \brief Class to translate paths in filenames embedded in an
 *  RoseHPCT-IR tree.It also fixes up the File Node pointer for each located node
 *
 *  Pre-order traversal, isConst = false
 */
class PathTranslator : public Trees::PreTraversal<IRTree_t, false>
{
public:
  /*! \brief Initialize with a specific path translation map */
  PathTranslator (const EquivPathMap_t& eqpaths);
  virtual ~PathTranslator (void);

  /*! \brief Returns 'true' if any paths have been translated. */
  bool didChange (void) const;

private:
  /*! \brief Only translates paths for File nodes */
  virtual void visit (TreeParamPtr_t tree);

  /*! \brief File-node specific translation */
  void visitFile (File* node) ;

  /*! \brief Associative map of paths and their translations */
  EquivPathMap_t eqpaths_;

  /*! \brief True <==> any paths were changed during traversal */
  bool did_change_;

  /*! \brief The current file node's value */
  File* file_;
};

PathTranslator::PathTranslator (const EquivPathMap_t& eqpaths)
  : eqpaths_ (eqpaths), did_change_ (false)
{
}

PathTranslator::~PathTranslator (void)
{
}

bool
PathTranslator::didChange (void) const
{
  return did_change_;
}

void
PathTranslator::visit (TreeParamPtr_t tree)
{
  File* filenode = dynamic_cast<File *> (tree->value);
  if (filenode != NULL)
  {
    // Save file nodes for later use
    RoseHPCT::profFileNodes_.insert(filenode);
    visitFile (filenode);
  }
  else
  {  // for other located nodes, fix up file node pointer if necessary
     Located * locatedNode=  dynamic_cast<Located*> (tree->value);
     if (locatedNode)
     {
       Statement* s = dynamic_cast<Statement*> (tree->value);
       // Save non-scope statement nodes for later use
       if (s)
         RoseHPCT::profStmtNodes_.insert(s);
       if (locatedNode->getFileNode()==NULL)
       {
         ROSE_ASSERT(file_ != NULL);
         locatedNode->setFileNode(file_);
       }
       else
       {// if it has a file node pointer, double check it here
         ROSE_ASSERT(file_== locatedNode->getFileNode());
       }
     }
  }
}

void
PathTranslator::visitFile (File* node) 
{
  file_ = node; // save this node
  string filename = node->getName ();
  // find a matching entry for the eqpaths
#if 1 
  // Liao, 4/28/2009 we extend this function to replace a root dir of a source tree to aonther
  // for example: /root1/smg2000/file1 --> /root2/smg2000/file1
  // The original function only deals with full path replacement, not a portion of it(root path),
  // which is less useful in practice.
   EquivPathMap_t::const_iterator iter = eqpaths_.begin(); 
   for (;iter!=eqpaths_.end(); iter++)
   {
     string oldpath = iter->first;
     string newpath = iter->second;
     //replace the matching old root path of a file with the new root path
     size_t pos1= filename.find(oldpath);
     if (pos1==0) // ensure it is the root path
     {
       if (enable_debug)
         cout<<"Replacing a file path from:"<<filename;
       filename.replace(pos1,oldpath.size(), newpath);
       if (enable_debug)
       {
         cout<<" to: "<<filename<<endl;
       }
       node->setName (filename);  
     }
   }
#else  
  string dirname = getDirname (filename);
  EquivPathMap_t::const_iterator ep = eqpaths_.find (dirname);
  if (ep != eqpaths_.end ())
  {
    // replace the file path with the second path of the entry
    string basename = getBaseFilename (filename);
    string new_filename = ep->second + "/" + basename;
    node->setName (new_filename);
  }
#endif  
}

bool
RoseHPCT::translateFilePaths (IRTree_t* root, const EquivPathMap_t& eqpaths)
{
  PathTranslator xlator (eqpaths);
  xlator.traverse (root);
  return xlator.didChange ();
}

/* ---------------------------------------------------------------- */

MetricTranslator::MetricTranslator (const RenameMap_t& mettab)
  : metrics_ (mettab), did_change_ (false)
{
}

MetricTranslator::~MetricTranslator (void)
{
}

bool
MetricTranslator::didChange (void) const
{
  return did_change_;
}

void
MetricTranslator::visit (TreeParamPtr_t tree)
{
  Observable* node = dynamic_cast<Observable *> (tree->value);
  if (node != NULL)
    visitObservable (node);
}

void
MetricTranslator::visitObservable (Observable* node)
{
  if (node->getNumMetrics () <= 0) return;

  for (Observable::MetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      RenameMap_t::iterator m = metrics_.find (i->getName ());
      if (m != metrics_.end ())
	{
	  i->setName (m->second);
	  did_change_ = true;
	}
    }

#if 0 // leave preprocessor block in for debugging later
  for (Observable::ConstMetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      cerr << "New metric name: " << i->getName () << endl;
    }
#endif
}

bool
RoseHPCT::translateMetricNames (IRTree_t* root,
                                const map<string, string>& mets)
{
  MetricTranslator xlator (mets);
  xlator.traverse (root);
  return xlator.didChange ();
}

/* ---------------------------------------------------------------- */

MetricCollector::MetricCollector (MetricMap_t& metric_store)
  : metrics_ (metric_store)
{
}

void
MetricCollector::visit (TreeParamPtr_t tree)
{
  Observable* node = dynamic_cast<Observable *> (tree->value);
  if (node != NULL)
    visitObservable (node);
}

void
MetricCollector::visitObservable (Observable* node)
{
  if (node->getNumMetrics () <= 0) return;

  for (Observable::MetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      string key = i->getName ();
      MetricMap_t::iterator m = metrics_.find (key);
      if (m == metrics_.end ()) // first encounter
	metrics_[key] = 1;
      else
	metrics_[key] += 1;
    }

#if 0 // leave preprocessor block in for debugging later
  for (Observable::ConstMetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      cerr << "New metric name: " << i->getName () << endl;
    }
#endif
}

void
RoseHPCT::collectMetricNames (const IRTree_t* root,
                              map<string, size_t>& metrics)
{
  MetricCollector collection (metrics);
  collection.traverse (root);
}

/* eof */
