// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
\defgroup ROSEHPCT_PROFIR  ROSE-HPCT Profile IR
\ingroup ROSEHPCT

This submodule provides a basic abstract representation
of the HPCTookit profile data, which we refer to as the
"profile IR" (or ProfIR). The ProfIR represents a program in
terms of just the following few high-level constructs:
- Programs (executable programs; see \ref RoseHPCT::Program)
- Loader Modules (e.g., library modules linked into a
program; see \ref RoseHPCT::Module)
- Files (e.g., submodules of a library or program;
see \ref RoseHPCT::File)
- Procedures (e.g., functions; see \ref RoseHPCT::Procedure)
- Loops (see \ref RoseHPCT::Loop)
- Statements (see \ref RoseHPCT::Statement)

Each node in the profile IR may have a location (file name
and starting/ending line numbers) and "metrics" (observed
hardware counter values) associated with it.

$Id: profir.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
*/

/*!
 *  \file rosehpct/profir/profir.hh
 *  \brief Intermediate representation for HPCToolkit profile data.
 *  \ingroup ROSEHPCT_PROFIR
 *
 *  $Id: profir.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#if !defined(INC_PROFIR_PROFIR_HH)
//! rosehpct/profir/ir.h included.
#define INC_PROFIR_PROFIR_HH

#include <iostream>
#include <string>
#include <vector>

#include <rosehpct/util/tree2.hh>

namespace RoseHPCT
{
  //! \addtogroup ROSEHPCT_PROFIR
  //@{

  //! \brief A class derived from Named has a name associated with it.
  class Named
  {
  public:
    //! Initialize with an empty name.
    Named (void);
    //! Initialize with a particular name.
    Named (const std::string& name);
    //! Copy another name.
    Named (const Named& n);
    virtual ~Named (void);

    const std::string& getName (void) const;
    virtual std::string toString(void) const;
    void setName (const std::string& new_name);

  private:
    std::string name_; //!< Name of this object.
  };

  //! Stores an observed and named value.
  class Metric : public Named
  {
  public:
    //! Initialize to a 0 value.
    Metric (void);
    //! Copy another name and value.
    Metric (const Metric& m);
    //! Initialize to a given name and value.
    Metric (const std::string& name, double value);
    virtual ~Metric (void);

    virtual std::string toString() const;  
    double getValue (void) const;
    void setValue (double new_val);

  private:
    double value_; //!< Value of this metric
  };

  /*!
   *  \brief A class derived from Observable can have one or more metrics
   *  associated with it.
   */
  class Observable
  {
  public:
    Observable (void);
    Observable (const Observable& o);
    virtual ~Observable (void);

    //! \name Provide iterator-like semantics over child metrics.
    /*@{*/
    typedef std::vector<Metric>::iterator MetricIterator;
    typedef std::vector<Metric>::const_iterator ConstMetricIterator;
    
    size_t getNumMetrics (void) const;
    MetricIterator beginMetric(void);
    ConstMetricIterator beginMetric (void) const;
    MetricIterator endMetric (void);
    ConstMetricIterator endMetric (void) const;
    /*@}*/

    //! \name Observe metric fields.
    /*@{*/
    const Metric& getMetric (size_t i) const;
    Metric& getMetric (size_t i);
    double getMetricValue (size_t i) const;
    //Get a metric's value by its name
    double getMetricValue (const std::string& metric_name) const;
    std::string getMetricName (size_t i) const;
    /*@}*/

   /*!
    *  \brief Add a metric to the current list.
    *  \note Uniqueness of metric names is not enforced.
    */
    void addMetric (const Metric& m);

    virtual std::string toString() const;

  private:
    std::vector<Metric> metrics_; //!< Contains child metrics.
  };

  //forward declaration
  class File;
  /*!
   *  \brief A class derived from Located has a source file position
   *  associated with it.
   */
  class Located
  {
  public:
    Located (void);
    Located (const Located& l);
    Located (size_t b, size_t e, File* f=nullptr);
    virtual ~Located (void);

    File* getFileNode(void) const;
    void setFileNode(File* filenode);

    size_t getFirstLine (void) const;
    void setFirstLine (size_t l);

    size_t getLastLine (void) const;
    void setLastLine (size_t l);

    void setLines (size_t b, size_t e);
    virtual std::string toString() const;

  private:
    size_t begin_;
    size_t end_;
    File * filenode_; // file location info is not complete unless a file name is specified! Liao, 4/27/2009
  };
  /*@}*/

  /* -------------------------------------------------------------- */
  /*!
   *  \name Node classes.
   */
  /*@{*/
  /*!
   *  \brief Abstract base class for profile IR nodes.
   */
  class IRNode : public Named, public Observable
  {
    // DXN: not all IRNodes (e.g. Statement) produced by HPCToolkit profiler have matching SgNode.
    bool hasMatchedSgNode;
  public:
    bool hasMatchingSgNode() const { return hasMatchedSgNode; }
    void setHasMatchingSgNode(bool flag) { hasMatchedSgNode = flag; }
    virtual std::string toString() const;
   protected:
    IRNode (void): hasMatchedSgNode(false) {}

 // DQ (12/6/2016): Eliminate warning we want to consider as error: -Wreorder.
 // IRNode (const std::string& name) : hasMatchedSgNode (false), Named (name)  { }
    IRNode (const std::string& name) : Named (name), hasMatchedSgNode (false)  { }

    virtual ~IRNode (void) {}
  };

  //! Program node ("PGM")
  class Program : public IRNode
  {
  public:
    Program (const std::string& name) : IRNode (name) {}
    virtual std::string toString() const;
  };

  //! Section Header node ("SecHeader")
  class SecHeader : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

  //! Metric Table node ("MetricTable")
  class MetricTable : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

  //! Metric Element node ("MetricElement")
  class MetricElement : public IRNode
  {
      std::string id;
      std::string val_type;
      std::string type;
      std::string show;
  public:
    MetricElement(const std::string& i, const std::string& n, std::string v,
            const std::string& t, const std::string& s):
        IRNode(n), id(i), val_type(v), type(t), show(s) {}
    virtual std::string toString() const;
  };

  //! Section Flat Profile Data node ("SecFlatProfileData")
  class SecFlatProfileData : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

  //! Group node ("G")
  class Group : public IRNode
  {
    public:
      virtual std::string toString() const;
  };

  //! Loader module node ("LM")
  class Module : public IRNode
  {
  public:
    Module (const std::string& name) : IRNode (name) {}
    virtual std::string toString() const;
  };
  
  //! File node ("F")
  class File : public IRNode
  {
  public:
    File (const std::string& name) : IRNode (name) {}
    virtual std::string toString() const;
  };

  //! Procedure node ("P")
  class Procedure : public IRNode, public Located
  {
    long int id;
  public:
    Procedure (long int i, const std::string& name, long int l);
    virtual std::string toString() const;  
  };

  //! Call site ("C")
  class CallSite: public IRNode
  {
      long int id;
      long int line;
  public:
    CallSite(long int i, long int l);
    virtual std::string toString() const;
  };

  //! Procedure frame ("PF")
  class ProcFrame: public IRNode
  {
      long int id;
      std::string name;
      long int line;
  public:
    ProcFrame(long int i, const std::string& n, long int l);
    virtual std::string toString() const;
  };

  //! Loop node ("L")
  class Loop : public IRNode  , public Located
  {
    long int id;
  public:
    Loop (const std::string& name, long int i, long int l);
    virtual std::string toString() const;  
  };

  //! Statement node ("S")
  class Statement : public IRNode, public Located
  {
    long int id;
   public:
    // typedef unsigned id_t;
    typedef long int id_t;

    Statement (void);
    Statement (const Statement& s);
    Statement (const std::string& name, id_t i, size_t l);

    ~Statement (void);

    id_t getId (void) const;
    void setId (id_t new_id);
    virtual std::string toString() const;  
  };

  //! IR tree representation
  typedef Trees::Tree<IRNode *> IRTree_t;

  /*@}*/

  //@}
  
  
}

#endif

/* eof */
