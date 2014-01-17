// Author: Markus Schordan
// $Id: AstAttributeMechanism.h,v 1.3 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTATTRIBUTEMECHANISM_H
#define ASTATTRIBUTEMECHANISM_H

#include "AttributeMechanism.h"
#include "rosedll.h"

class SgNode;

class ROSE_DLL_API AstAttribute
   {
  // This class contains no data and is to be used as a based class (typically, but not required)
  // to support under-defined attributes to be attached to AST IR nodes.

     public:

      // DQ (7/4/2008): Added support for attibutes to specify edges in the dot graphs.
          class AttributeEdgeInfo
             {
               public:
                    SgNode* fromNode;
                    SgNode* toNode;
                    std::string label;
                    std::string options;

                    AttributeEdgeInfo (SgNode* fromNode, SgNode* toNode, std::string label, std::string options )
                       : fromNode(fromNode), toNode(toNode), label(label), options(options)
                       {
                       }

                   ~AttributeEdgeInfo () { fromNode = NULL; toNode = NULL; };
             };

       // DQ (7/5/2008): Added support for adding nodes to DOT graphs
          class AttributeNodeInfo
             {
               public:
                    SgNode* nodePtr;
                    std::string label;
                    std::string options;

                    AttributeNodeInfo (SgNode* nodePtr, std::string label, std::string options )
                       : nodePtr(nodePtr), label(label), options(options)
                       {
                       }

                   ~AttributeNodeInfo () {};
             };
#if 1
          AstAttribute();
          virtual ~AstAttribute();
      /*! This function is used by other components to print the value of an attribute. For example the pdf generation
          calls this function to print the value of an attribute in the pdf file. The default implementation is to
          return an empty string.
       */
          virtual std::string toString();

       // JH (12/21/2005): Adding Methods for storing the Ast Attribute data
          AstAttribute* constructor();
          std::string attribute_class_name();

       // Packing support (by JH)
          virtual int packed_size();
          virtual char* packed_data();
          virtual void unpacked_data( int size, char* data );

       // DQ (7/4/2008): Added DOT support.
          virtual std::string additionalNodeOptions();
       // virtual std::string additionalEdgeInfo();

       // virtual std::vector<std::pair<SgNode*,SgNode*> > additionalEdgeInfo();
          virtual std::vector<AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AttributeNodeInfo> additionalNodeInfo();

       // DQ (7/27/2008): The support for deep copies of attributes on AST IR
       // node requires a virtual copy function that can be overwritten.
       // This acts just like a virtual constructor (same concept).
          virtual AstAttribute* copy();

       // DQ (7/27/2008): Added support to eliminate IR nodes in DOT graphs
       // (to tailor the presentation of information about ASTs).
          virtual bool commentOutNodeInGraph();
#else
          AstAttribute() {}
          virtual ~AstAttribute() {}
      /*! This function is used by other components to print the value of an attribute. For example the pdf generation
          calls this function to print the value of an attribute in the pdf file. The default implementation is to
          return an empty string.
       */
          virtual std::string toString() { return ""; }

       // JH (12/21/2005): Adding Methods for storing the Ast Attribute data
          AstAttribute* constructor() { return new AstAttribute(); }
          std::string attribute_class_name() { return "AstAttribute"; }

       // Packing support (by JH)
          virtual int packed_size() { return 0; }
          virtual char* packed_data() { return NULL; }
          virtual void unpacked_data( int size, char* data ) {}

       // DQ (7/4/2008): Added DOT support.
          virtual std::string additionalNodeOptions() { return ""; }
       // virtual std::string additionalEdgeInfo()    { return ""; }

       // virtual std::vector<std::pair<SgNode*,SgNode*> > additionalEdgeInfo();
          virtual std::vector<AttributeEdgeInfo> additionalEdgeInfo() { std::vector<AttributeEdgeInfo> v; return v; }
          virtual std::vector<AttributeNodeInfo> additionalNodeInfo() { std::vector<AttributeNodeInfo> v; return v; }

       // DQ (7/27/2008): The support for deep copies of attributes on AST IR
       // node requires a virtual copy function that can be overwritten.
       // This acts just like a virtual constructor (same concept).
          virtual AstAttribute* copy() { return new AstAttribute(*this); }

       // DQ (7/27/2008): Added support to eliminate IR nodes in DOT graphs
       // (to tailor the presentation of information about ASTs).
          virtual bool commentOutNodeInGraph() { return false; }
#endif

   };


/*!
 *  \brief Attribute corresponding to a metric.
 *
 *  A metric attribute represents a numeric value obtained by either
 *  dynamic analysis (gprof or hpct) or static analysis (for example
 *  number of flop operations in a function)
 *  It MetricAttribute can be a raw (observed), or a
 *  propagated (derived) count
 *  It containes no name-string, because the attribute is stored in an
 *  AttributeMechanism-map, where the name is the key
 */
class MetricAttribute : public AstAttribute
{
    public:
        MetricAttribute();
        MetricAttribute(double value, bool is_derived=false);

        MetricAttribute& operator+= (const MetricAttribute & other);
        MetricAttribute& operator-= (const MetricAttribute & other);
        MetricAttribute& operator*= (const MetricAttribute & other);
        MetricAttribute& operator/= (const MetricAttribute & other);

        virtual AstAttribute* constructor();
        virtual AstAttribute* copy();

        virtual std::string attribute_class_name();

        virtual int   packed_size();
        virtual char* packed_data();
        virtual void  unpacked_data( int size, char* data );

        virtual bool        isDerived() const;
        virtual double      getValue()  const;
        virtual void        setValue(double newVal);
        virtual std::string toString();

    protected:
        bool   is_derived_;
        double value_;
};


// DQ (6/28/2008):
// Since this is implemented using AttributeMechanism which is derived from
// std::map<Key,Value>, "Value" is a template parameter for "AstAttribute*"
// And so the copy constructor will copy the pointer.  This is OK, but it
// means that the AstAttribute objects are shared.  Alternatively, a copy
// constructor implemented in this class could be implemented to support
// deep copies.  This might be a good idea.
class AstAttributeMechanism : public AttributeMechanism<std::string,AstAttribute*>
   {
     public:
       // DQ (7/27/2008): Build a copy constructor that will do a deep copy
       // instead of calling the default copy constructor.
          AstAttributeMechanism ( const AstAttributeMechanism & X );

       // DQ (7/27/2008): Because we add an explicit copy constructor we
       // now need an explicit default constructor.
          AstAttributeMechanism ();
   };


// DQ (11/21/2009): Added new kind of attribute for handling regex trees.
/*!
 *  \brief Attribute corresponding to a regex expression.
 *
 *  A RegEx attribute is added to each lart of an AST tree pattern 
 *  specification to a RegEx tree.
 */
class AstRegExAttribute : public AstAttribute
   {
     public:
          std::string expression;

          AstRegExAttribute();
          AstRegExAttribute(const std::string & s);
   };

// PC (10/21/2012): Added new kind of attribute for handling regex trees.
/*!
 *  \brief Attribute corresponding to an SgNode.
 *
 *  A SgNode attribute may be used to "dynamically add" an SgNode field to
 * an AST node.
 */
class AstSgNodeAttribute : public AstAttribute
   {
          SgNode *node;

     public:
          SgNode *getNode();

          AstSgNodeAttribute();
          AstSgNodeAttribute(SgNode *node);
   };

class AstSgNodeListAttribute : public AstAttribute
   {
          std::vector<SgNode *> nodeList;

     public:
          std::vector<SgNode *> &getNodeList();
          void addNode(SgNode *);
          void setNode(SgNode *, int);
          SgNode *getNode(int);
          int size();

          AstSgNodeListAttribute();
          AstSgNodeListAttribute(std::vector<SgNode *> &);
   };

#endif
