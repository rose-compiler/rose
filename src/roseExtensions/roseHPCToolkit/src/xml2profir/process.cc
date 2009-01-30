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

#include "rosehpct/xml2profir/xml2profir.hh"
#include "sla.h"
#include "rosehpct/util/general.hh"

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
 *  RoseHPCTIR tree.
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
  void visitFile (File* node) const;

  /*! \brief Associative map of paths and their translations */
  EquivPathMap_t eqpaths_;

  /*! \brief True <==> any paths were changed during traversal */
  bool did_change_;
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
  File* node = dynamic_cast<File *> (tree->value);
  if (node != NULL)
    visitFile (node);
}

void
PathTranslator::visitFile (File* node) const
{
  string filename = node->getName ();
  string dirname = getDirname (filename);
  // find a matching entry for the eqpaths
  EquivPathMap_t::const_iterator ep = eqpaths_.find (dirname);
  if (ep != eqpaths_.end ())
    {
      // replace the file path with the second path of the entry
      string basename = getBaseFilename (filename);
      string new_filename = ep->second + "/" + basename;
      node->setName (new_filename);
    }
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
