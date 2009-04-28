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
    ~Named (void);

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
    ~Metric (void);

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
    ~Observable (void);

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
    Located (size_t b, size_t e, File* f=NULL);
    ~Located (void);

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
  public:
    virtual std::string toString() const;
  protected:
    IRNode (void) {}
    IRNode (const std::string& name) : Named (name) {}
    virtual ~IRNode (void) {}
  };

  //! Program node ("PGM")
  class Program : public IRNode
  {
  public:
    Program (const std::string& name) : IRNode (name) {}
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
  public:
    Procedure (const std::string& name, size_t b, size_t e)
      : IRNode (name), Located(b, e) {}
    virtual std::string toString() const;  
  };

  //! Loop node ("L")
  class Loop : public IRNode, public Located
  {
  public:
    Loop (const std::string& name, size_t b, size_t e)
      : IRNode (name), Located(b, e) {}
    virtual std::string toString() const;  
  };

  //! Statement node ("S")
  class Statement : public IRNode, public Located
  {
  public:
    typedef unsigned id_t;

    Statement (void);
    Statement (const Statement& s);
    Statement (const std::string& name, size_t b, size_t e)
      : IRNode (name), Located(b, e) {}
    ~Statement (void);

    id_t getId (void) const;
    void setId (id_t new_id);
    virtual std::string toString() const;  

  private:
    id_t id;
  };

  //! IR tree representation
  typedef Trees::Tree<IRNode *> IRTree_t;
  /*@}*/

  //@}
}

#endif

/* eof */
