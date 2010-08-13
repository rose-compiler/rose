// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
\defgroup ROSEHPCT_SAGE  ROSE-HPCT Sage Support
\ingroup ROSEHPCT

This submodule defines the metric attribute, \ref
RoseHPCT::MetricAttr, which is interpreted from the profile IR (\ref
ROSEHPCT_PROFIR) and is derived from AstAttribute so that it can be
attached to Sage nodes.

In addition, this submodule provides a variety of support routines
for traversing the Sage III IR and propagating metrics from low-level
scopes to higher-level scopes (e.g., from statements to their surrounding
loop and function scopes).

$Id: sage.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
*/

/*!
 *  \file rosehpct/sage/sage.hh
 *  \brief Implements utility routines that operate on the annotated
 *  Sage III tree.
 *  \ingroup ROSEHPCT_SAGE
 *
 *  $Id: sage.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_SAGE_ROSEHPCT_HH)
//! rosehpct/sage/sage.hh included.
#define INC_SAGE_ROSEHPCT_HH

#include <iostream>
#include <string>
#include <vector>

#include <rosehpct/sage/collectors.hh>
// #include "rose.h"

namespace RoseHPCT
{
  //! \addtogroup ROSEHPCT_SAGE
  //@{

  //! List of metric names (strings), with forward iterator semantics
  typedef std::vector<std::string> MetricNameList_t;

  /*!
   *  \brief Attribute corresponding to a metric.
   *
   *  A metric attribute is a pair (name, value). In addition, a
   *  metric attribute may be tagged as to whether or not the value is
   *  a 'raw' observed count, or some kind of 'derived' value.
   */
  class MetricAttr : public MetricAttribute
  {
  public:
    MetricAttr (void);
    MetricAttr (const std::string& name, double value,
                bool is_derived = false);

    std::string getName (void) const;

    virtual MetricAttr* clone (void) const;

    MetricAttr& operator+= (const MetricAttr& b);


    // Liao, 12/3/2007, add support for AST_FILE_IO
     // must provide this, otherwise, base class's unpacked_data() will be used
    MetricAttr * constructor() {return new MetricAttr();}
  //  std::string attribute_class_name() { return "MetricAttr"; }

    virtual int packed_size()
    {
      int size=0;
      // name + Tab + value + "**"
      size = getName().size()+toString().size()+1;
       // derived value has ** in the rear.
      if (isDerived())
        size+=2;
      return size;
    };

    virtual char* packed_data()
    {
      std::string result;
      //toString() will entail the value string with ** if it is derived.
      result= name_ + "\t" + toString();
      return const_cast<char *>(result.c_str());
    } ;
    virtual void unpacked_data(int size, char* data)
    {
      if (size <=0) return;
      //1. check tail **
      char * head = data;
      char * tail = head + strlen(head) - 1;
      if (*tail=='*')
        is_derived_ = true;

       // 2. retrieve name portion
       std::string tname;
       while ((*head!='\t')&&(*head!='\0'))
       {
         tname+=*head;
         head++;
       }
       name_ = tname;

      //3. retrieve value
      // strtod() is smart enough to skip tab and ignore tailing **
       char * endp;
       value_=strtod(head,&endp);
    };

  private:
    std::string name_; //!< Name of the metric
  };

  //! Computes the sum of two metric attributes.
  MetricAttr operator+ (const MetricAttr& a, const MetricAttr& b);

  //! Returns a metric attribute, if any.
  MetricAttr* getMetric (const std::string& metric_name, SgNode* node);

  /*!
   *  \brief Class to propagate metrics to higher scopes.
   *
   *  A caller instantiates this class with the name of a particular
   *  metric to propagate.
   */
  class MetricScopePropagator : public AstBottomUpProcessing<MetricAttr>
  {
  public:
    /*!
     * Initializes to propagate a metric specified by its attribute
     * name.
     */
    MetricScopePropagator (const std::string& name);

  protected:
    virtual MetricAttr evaluateSynthesizedAttribute (SgNode* node,
                                                     SynthesizedAttributesList attr_list);

    template <class SgType>
    bool eval (SgType* node,
               const SynthesizedAttributesList& attr_list);

  private:
    std::string metric_name_;
  };

  //! Convert an SgLocatedNode to a string (for debugging display)
  std::string toFileLoc (const SgLocatedNode* node);

  /*! \brief Returns the filename associated with the start of an
   *  SgLocatedNode, or the empty string if one cannot be determined.
   */
  std::string getFilename (const SgLocatedNode* node);

  /*! \brief Returns the starting line number associated with the
   *  start of an SgLocatedNode, or 0 if one cannot be determined.
   */
  int getLineStart (const SgLocatedNode* node);

  /*! \brief Returns the ending line number associated with the end of
   *  an SgLocatedNode, or 0 if one cannot be determined.
   */
  int getLineEnd (const SgLocatedNode* node);

  //! Propagate a list of metrics by name
  void propagateMetrics (SgProject* node,
                         const MetricNameList_t& metrics,
			 const std::string& prefix = std::string(""));

  //! Propagate a list of metrics by name
  void propagateMetrics (SgProject* node,
                         const MetricNameList_t& metrics,
			 const char* prefix);

  /*! \brief Attribute storing the depth of a tree node (path length
   *  from the root).
   */
  class DepthAttr : public AstAttribute
  {
  public:
    size_t depth;
    DepthAttr (size_t d = 0);
    std::string toString (void);
  };

  /*!
   *  \brief Compute an attribute, "tree_depth", the length of the path to
   *  each node from the root.
   */
  class TreeDepthCalculator : public AstTopDownProcessing<size_t>
  {
  public:
    static const std::string ATTR_NAME;
    TreeDepthCalculator (SgProject* root);

  protected:
    virtual size_t evaluateInheritedAttribute (SgNode* node,
                                               size_t d_parent);
  };

  //@}
}

#endif

/* eof */
