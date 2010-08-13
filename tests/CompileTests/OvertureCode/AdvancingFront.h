#ifndef __KKC_AdvancingFront__
#define __KKC_AdvancingFront__

#include "iostream.h"
#include "Overture.h"
#include "PlotStuff.h"
#include "Mapping.h"
#include "Face.h"
#include "GeometricADT.h"
#include "AbstractException.h"
#include "Mapping.h"
#include "UnstructuredMapping.h"
#include "CompositeGridFunction.h"
#include "PriorityQueue.h"
#include "AdvancingFrontParameters.h"
#ifndef OV_USE_OLD_STL_HEADERS
#include <vector>
#include <map>
#include <list>
//using namespace std;
#else
#include <vector.h>
#include <map.h>
#include <list.h>
#endif

//typedef PriorityQueueTemplate<Face *> PriorityQueue;
typedef PriorityBatchQueue<Face *> PriorityQueue;

class AdvancingFront
{

public:
  

  AdvancingFront();
  AdvancingFront(intArray &initialFaces, realArray &xyz_in, Mapping *backgroundMapping_=NULL);
  
  virtual ~AdvancingFront();

  void initialize(intArray &initialFaces, realArray &xyz_in, Mapping *backgroundMapping_= NULL);

  bool isFrontEmpty() const;

  int insertFace(const IntegerArray &vertexIDs, int z1, int z2);
  int advanceFront(int nSteps = 1);
  
  AdvancingFrontParameters & getParameters() { return parameters; }

  PriorityQueue::iterator getFrontIteratorForFace(const IntegerArray &faceVertices);
  bool existsInFront(const IntegerArray &);
  void plot(PlotStuff & ps, PlotStuffParameters & psp);

  // getFaces and getFront are used in the plotAdvancingFront method
  const vector<Face *> & getFaces() const { return faces; }
  const PriorityQueue & getFront() const { return front; }

  const realArray & getVertices() const { return xyz; }

  const intArray  generateElementList(); // generates the list of points in each element, used to build an unstructuredMapping

  // get various dimensional information
  int getRangeDimension() const { return rangeDimension; }
  int getDomainDimension() const { return domainDimension; }
  int getNumberOfVertices() const { return nptsTotal; }
  int getNumberOfFaces() const { return nFacesTotal; }
  int getNumberOfElements() const { return nElements; }

  real getAverageFaceSize() const { return averageFaceSize; }
  // get and set the spacing control grid
  const realCompositeGridFunction & getControlFunction() const { return controlFunction; }
  void setControlFunction(const CompositeGrid & controlGrid_, const realCompositeGridFunction & controlFunction_) 
    { 
      controlGrid.reference(controlGrid_);
      controlGrid.updateReferences();
      controlFunction.reference(controlFunction_);
    }

  // destroy the information in this class, reset to some basic state
  void destroyFront();

protected:

  AdvancingFrontParameters parameters;

  int nFacesFront;  // number of faces in the front
  int nFacesTotal;  // total number of faces generated
  int nFacesEst;    // estimated number of faces to be generated
  int nptsTotal;  // number of points in the mesh so far (size of used portion of xyz)
  int nptsEst;    // estimated number of points to be created
  int nElements;     // total number of elements generated
  int nElementsEst;  // current estimate on the total number of elements to be generated
  
  int rangeDimension;
  int domainDimension;
  int nFacesPerElementMax;
  int nVerticesPerFaceMax;

  real averageFaceSize;

  vector<Face *> faces;  // a vector of all the faces, indexed by face id
  vector<int> foo;
  vector< vector<int> > elements; // a list containing the elements identified by thier faces
  PriorityQueue front;       // a priority queue for the front
  realArray xyz;         // the positions of the points

  struct cmpFace { bool operator() (const int i1, const int i2) const { return (i1<i2); } }; // used in the map below
  map<int, vector< PriorityQueue::iterator >, cmpFace> pointFaceMapping; // maps a point id to the faces that have that point as point "1" (basically a simple hash table)
    
  GeometricADT<Face *> faceSearcher;  // Geometric search ADT used to detect intersections

  CompositeGrid controlGrid; // meshes used to define control functions
  realCompositeGridFunction controlFunction;  // function used to define stretching/distribution functions
  Mapping *backgroundMapping; // background parametric mapping

  real computeNormalizedGrowthDistance(real, real);

  void gatherExistingCandidates(const Face & face, real distance, realArray & pIdealTrans,
				realArray & T, realArray & Tinv, vector<int> &existing_candidates);
  void computeVertexCandidates(const realArray &currentFaceVerticesTrans, 
                               const realArray &pIdealTrans, vector<int> &existing_candidates, 
			       const realArray &T, realArray &new_candidates, real rad);

  bool makeTriTetFromExistingVertices(const Face &currentFace, int newElementID,
				      const vector<int> &existing_candidates, 
				      vector<PriorityQueue::iterator > &oldFrontFaces);

  bool makeTriTetFromNewVertex(const Face & currentFace, int newElementID, 
                               realArray &new_candidates);

  bool isFaceConsistent(const IntegerArray &, const Face & filterFace) const;  // essentially checks for intersections
  bool isFaceConsistent(const realArray & , const Face & filterFace) const; // again.
  bool isFaceConsistent2D(const realArray & p1, const realArray & p2, const int filterFace) const;
  bool isFaceConsistent2D(int v1, int v2, int filterFace) const;

  bool isOnFacePlane( const Face & face, realArray &vertex );
  bool checkVertexDirection( const Face & face, const realArray &vertex ) const;

  int resizeFaces(int newSize); // resizes face relevant arrays
  int addPoint(const realArray &newPt); // adds a new point to the list of points, resizes if needed
  void addFaceToElement(int face, int elem); // adds a face to an element
  void removeFaceFromFront(PriorityQueue::iterator&); // deletes a face from the front as well as associated data structures
  int transformCoordinatesToParametricSpace(realArray &xyz_in, realArray &xyz_param); // for parameterized surfacees
													  
  void computeFaceTransformation(const Face &face, realArray &T, realArray &Tinv);
  void computeFaceNormalTransformation(const realArray &vertices, realArray &T, double stretch=1.0); // computes a mesh control transformation  based on the current Face`s size and position
  int computeTransformationAtPoint(realArray &midPt, realArray &T); // computes a mesh control transformation based on the data in the controlFunction 

private:
  realArray matVectMult(const realArray &M, const realArray &V);  // matrix-vector multiply, just because I use it so much.
  

};

// AdvancingFront exceptions ( it would be nice to use namespaces...)
class AdvancingFrontError : public AbstractException
{
public:
  virtual void debug_print() const { cerr<<"\nAdvancingFront Error"; }
};

class BookKeepingError : public AdvancingFrontError
{
public:
  void debug_print() const 
  {
    //AdvancingFront::AdvancingFrontError::debug_print();
    cerr<<": BookKeepingError : Errors found with mesh bookeeping ";
  }
};

class DimensionError : public AdvancingFrontError
{
public:
  void debug_print() const 
  {
    //AdvancingFront::AdvancingFrontError::debug_print();
    cerr<<": DimensionError : Internal error with dimensions used in internal vectors";
  }
};

class AdvanceFailedError : public AdvancingFrontError
{
public:
  void debug_print() const 
  {
    //AdvancingFront::AdvancingFrontError::debug_print();
    cerr<<": AdvanceFailedError : The front is not empty but could not be advanced, probably a bug in the algorithm";
  }
};

class FrontInsertionFailedError : public AdvancingFrontError
{
public:
  void debug_print() const 
  {
    //AdvancingFront::AdvancingFrontError::debug_print();
    cerr<<": FrontInsertionFailedError : A problem occurred inserting a new face into the front, probably a bug in the algorithm";
  }
};


#endif
