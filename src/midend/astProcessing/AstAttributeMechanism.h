// Author: Markus Schordan
// $Id: AstAttributeMechanism.h,v 1.3 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTATTRIBUTEMECHANISM_H
#define ASTATTRIBUTEMECHANISM_H

#include "AttributeMechanism.h"

class SgNode;

class AstAttribute
   {
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

          AstAttribute() {}
          virtual ~AstAttribute() {}
      /*! This function is used by other components to print the value of an attribute. For example the pdf generation
          calls this function to print the value of an attribute in the pdf file. The default implementation is to
          return an empty string.
       */
          virtual std::string toString() { return ""; }

       // JH (12/21/2005): Adding Methods for storing the Ast Attribute data
          AstAttribute* constructor() {return new AstAttribute();}
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
   };

#endif
