#ifndef ORTHO_ARRAY_ANALYSIS_H
#define ORTHO_ARRAY_ANALYSIS_H

#include "compose.h"

namespace fuse
{
/***********************************
 ***** OrthoArrayMemLocObject *****
 ***********************************/

extern int orthoArrayAnalysisDebugLevel;
class OrthogonalArrayAnalysis;
  
// This is an analysis that interprets array expressions of the form a[i] by orthogonally 
// combining information from a memory location analysis to interpret "a" and a value analysis
// to interpret "i".

class OrthoIndexVector_Impl : public IndexVector
{
  public:
    size_t getSize() const {  return index_vector.size(); };
    /*GB: Deprecating IndexSets and replacing them with ValueObjects.
    std::vector<IndexSet *> index_vector; // a vector of memory objects of named objects or temp expression objects */
    std::vector<ValueObjectPtr> index_vector; // a vector of memory objects of named objects or temp expression objects
    std::string str(std::string indent) const; // pretty print for the object
    std::string str(std::string indent) { return ((const OrthoIndexVector_Impl*)this)->str(indent); }
    
    // Allocates a copy of this object and returns a pointer to it
    IndexVectorPtr copyIV() const;

    bool mayEqual (IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
    bool mustEqual(IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
    
    // Returns whether the two abstract IndexVectors denote the same set of concrete vectors
    bool equalSet(IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
    
    // Returns whether this abstract IndexVector denotes a non-strict subset (the sets may be equal) of the set denoted
    // by the given abstract IndexVector.
    bool subSet(IndexVectorPtr other, PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
    
    bool isFull (PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
    bool isEmpty(PartEdgePtr pedge, Composer* comp, ComposedAnalysis* analysis);
};
typedef boost::shared_ptr<OrthoIndexVector_Impl> OrthoIndexVector_ImplPtr;

// Memory object wrapping the information about array
class OrthoArrayML : public MemLocObject
{
  protected:
    // The parent array reference
    SgNode *array_ref;
    // memory object for the top level array object
    MemLocObjectPtr p_array;
    // list of value objects for the subscripts
    IndexVectorPtr p_iv;
    
    // to represent other memory objects that are not array
    MemLocObjectPtr p_notarray;
    
    // Records whether the p_array/p_iv/p_notarray objects refer to the original MemLocObjectPtr that was 
    // passed to this object's constructor (true) or is a copy of this original object
    bool origML; 
    
    // useful to distinguish this object from other memory objects
    typedef enum {empty, array, notarray, full} oaLevels;
    oaLevels level;
    //bool isArrayElement;
    
    // The analysis that produced this MemLoc
    OrthogonalArrayAnalysis* oaa;

  public:
    OrthoArrayML(SgNode* sgn, MemLocObjectPtr arrayML, IndexVectorPtr iv, OrthogonalArrayAnalysis* oaa) :
      MemLocObject(sgn), array_ref(sgn), p_array(arrayML), p_iv(iv), origML(true), level(array)/*isArrayElement(true)*/, oaa(oaa) { }
    OrthoArrayML(SgNode* sgn, MemLocObjectPtr notarrayML, OrthogonalArrayAnalysis* oaa) : 
      MemLocObject(sgn), array_ref(sgn), p_notarray(notarrayML), origML(true), level(notarray)/*isArrayElement(false)*/, oaa(oaa) { }
    OrthoArrayML(const OrthoArrayML& that) : MemLocObject(that)
    {
      p_array    = that.p_array;
      p_iv       = that.p_iv;
      p_notarray = that.p_notarray;
      origML     = that.origML;
      //isArrayElement = that.isArrayElement;
      level      = that.level;
      oaa        = that.oaa;
    }
    // pretty print
    std::string str(std::string indent="") const;
    std::string str(std::string indent="") { return ((const OrthoArrayML*)this)->str(indent); }

    // copy this object and return a pointer to it
    MemLocObjectPtr copyML() const { return boost::make_shared<OrthoArrayML>(*this); }

    bool mayEqualML(MemLocObjectPtr that, PartEdgePtr pedge);
    bool mustEqualML(MemLocObjectPtr that, PartEdgePtr pedge);
    
    // Returns whether the two abstract objects denote the same set of concrete objects
    bool equalSet(AbstractObjectPtr o, PartEdgePtr pedge);
    
    // Returns whether this abstract object denotes a non-strict subset (the sets may be equal) of the set denoted
    // by the given abstract object.
    bool subSet(AbstractObjectPtr o, PartEdgePtr pedge);
    
    bool isLiveML(PartEdgePtr pedge);
    
    // Computes the meet of this and that and saves the result in this
    // returns true if this causes this to change and false otherwise
    bool meetUpdateML(MemLocObjectPtr that, PartEdgePtr pedge);
    
    // Returns whether this AbstractObject denotes the set of all possible execution prefixes.
    bool isFull(PartEdgePtr pedge);
    // Returns whether this AbstractObject denotes the empty set.
    bool isEmpty(PartEdgePtr pedge);
    
    // Set this object to represent the set of all possible MemLocs
    // Return true if this causes the object to change and false otherwise.
    bool setToFull();
    // Set this Lattice object to represent the empty set of MemLocs.
    // Return true if this causes the object to change and false otherwise.
    bool setToEmpty();
};
typedef boost::shared_ptr<OrthoArrayML> OrthoArrayMLPtr;

/*class OrthoArrayMemLocObject : public MemLocObject
{
  protected:
  // If this MemLocObject represents an array expression a[i] this represents the "a".
  // Otherwise, this represents the entire expression.
  //MemLocObjectPtr array;
  // If this MemLocObject represents an array expression a[i] this represents the "i".
  // Otherwise, it is null
  //ValueObjectPtr value;
  // If this MemLocObject represents an array expression a[i] represents the entire "a[i]".
  // Otherwise, it is null
  MemLocObjectPtr arrayElt;
  // If this MemLocObject doesn't represent an array expression, it is pointed to by this
  MemLocObjectPtr notArray;
  PartPtr part;

  public:
  OrthoArrayMemLocObject(MemLocObjectPtr array, OrthoIndexVector_ImplPtr value, PartPtr p);
  OrthoArrayMemLocObject(MemLocObjectPtr notArray, PartPtr p);
  OrthoArrayMemLocObject(const OrthoArrayMemLocObject& that);

  bool mayEqual(MemLocObjectPtr o, PartPtr p) const;
  bool mustEqual(MemLocObjectPtr o, PartPtr p) const;
  
  // pretty print for the object
  std::string str(std::string indent="") const;
  std::string str(std::string indent="") { return ((const OrthoArrayMemLocObject*)this)->str(indent); }
  std::string strp(PartPtr part, std::string indent="") const;
  std::string strp(PartPtr part, std::string indent="") { return ((const OrthoArrayMemLocObject*)this)->strp(part, indent); }
  
  // Allocates a copy of this object and returns a pointer to it
  MemLocObjectPtr copyML() const;
  
  private:
  // Dummy Virtual method to make it possible to dynamic cast from OrthoArrayMemLocObjectPtr to MemLocObjectPtr
  virtual void foo() {}
};
typedef boost::shared_ptr<OrthoArrayMemLocObject> OrthoArrayMemLocObjectPtr;
*/

/***********************************
 ***** OrthogonalArrayAnalysis *****
 ***********************************/

class OrthogonalArrayAnalysis : virtual public UndirDataflow
{
  public:
  OrthogonalArrayAnalysis() : UndirDataflow() {}
    
  // Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
  ComposedAnalysisPtr copy() { return boost::make_shared<OrthogonalArrayAnalysis>(); }
    
  // The genInitLattice, genInitFact and transfer functions are not implemented since this 
  // is not a dataflow analysis.
   
  // Maps the given SgNode to an implementation of the MemLocObject abstraction.
  // Variant of Expr2Val where Part field is ignored since it makes no difference for the syntactic analysis.
  MemLocObjectPtr  Expr2MemLoc(SgNode* n, PartEdgePtr pedge);
  bool implementsExpr2MemLoc() { return true; }
  
  // pretty print for the object
  std::string str(std::string indent="")
  { return "OrthogonalArrayAnalysis"; }
};

}; //namespace fuse

#endif
