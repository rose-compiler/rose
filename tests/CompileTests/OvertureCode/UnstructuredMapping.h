#ifndef UNSTRUCTURED_MAPPING_H
#define UNSTRUCTURED_MAPPING_H 

#include "Mapping.h"
#include "CompositeGrid.h"

class MappingProjectionParameters;
class CompositeSurface;
class GeometricADT3dInt;

//-------------------------------------------------------------
//  Define a Mapping for an unstructured surface
//-------------------------------------------------------------
class UnstructuredMapping : public Mapping
{

public:

  enum ElementType 
  {
    triangle,
    quadrilateral,
    tetrahedron,
    pyramid,
    triPrism,
    septahedron,  // pray we never need...
    hexahedron,
    other,
    boundary
  };

  UnstructuredMapping();

  UnstructuredMapping(int domainDimension_ /* =3 */, 
		      int rangeDimension_ /* =3 */, 
		      mappingSpace domainSpace_ /* =parameterSpace */,
		      mappingSpace rangeSpace_ /* =cartesianSpace */ );

  // Copy constructor is deep by default
  UnstructuredMapping( const UnstructuredMapping &, const CopyType copyType=DEEP );

  ~UnstructuredMapping();

  UnstructuredMapping & operator =( const UnstructuredMapping & X0 );

  //
  // overload of this method, it doesn't really make sense for UnstructuredMappings (?)
  // watch out, the base class method Mapping::getGrid actually calls Mapping::map, which
  // is private in UnstructuredMapping since it makes little sense.
  //
  virtual const realArray& getGrid(MappingParameters & params=Overture::nullMappingParameters()) { return node; }

  int getNumberOfNodes() const;

  int getNumberOfElements() const;

  int getNumberOfFaces() const;

  int getNumberOfEdges() const;

  int getMaxNumberOfNodesPerElement() const;
  
  int getMaxNumberOfFacesPerElement() const;
  
  int getMaxNumberOfNodesPerFace() const;
  
  int getNumberOfBoundaryFaces() const;

  void setPreferTriangles( bool trueOrFalse=true ){ preferTriangles=trueOrFalse;}
  void setElementDensityTolerance(real tol);
  
  const realArray & getNodes() const;
  const intArray & getElements() const;
  const intArray & getFaces() const;
  const intArray & getFaceElements() const;
  const intArray & getEdges() const;
  const intArray & getTags() const;   
  const intArray & getElementFaces();
  const intArray & getBoundaryFace() const;


  // iteration helpers
  inline int getNumberOfNodesThisElement(int element) const;
  inline int getNumberOfNodesThisFace(int face_) const;
  inline int getBoundaryFace(int bdyface_) const;
  inline int elementGlobalVertex(int element, int vertex) const;
  inline int faceGlobalVertex(int face, int vertex) const;

  int setNodesAndConnectivity( const realArray & nodes, 
                               const intArray & elements,
			       int domainDimension =-1 );

  int setNodesAndConnectivity( const realArray & nodes, 
                               const intArray & elements,
			       const intArray & faces,
                               const intArray & faceElements,
                               const intArray & elementFaces,
                               int numberOfFaces=-1,
                               int numberOfBoundaryFaces=-1,
                               int domainDimension =-1 );


  int setNodesElementsAndNeighbours(const realArray & nodes, 
				    const intArray & elements, 
				    const intArray & neighbours,
                                    int numberOfFaces=-1,
                                    int numberOfBoundaryFaces=-1,
				    int domainDimension =-1 );

  void setTags(const intArray &new_tags);

  int splitElement( int e, int relativeEdge, real *x ); // add a new node that splits an edge.
  

  virtual int intersects(Mapping & map2, 
                         const int & side1=-1, 
			 const int & axis1=-1,
			 const int & side2=-1, 
			 const int & axis2=-1,
			 const real & tol=0. ) const;

  // project points onto the surface
  int project( realArray & x, MappingProjectionParameters & mpParameters );

  int printConnectivity( FILE *file =stdout );
  int printStatistics(FILE *file =stdout );

  //IntegerArray buildFromAMapping( Mapping & map, int elementType=-1 );
  intArray buildFromAMapping( Mapping & map, intArray &maskin = Overture::nullIntegerDistributedArray() );

  int buildFromARegularMapping( Mapping & map );  // an optimized build for domainDimension==2
  int buildUnstructuredGrid( Mapping & map, int numberOfGridPoints[2]);
  
  void buildFromACompositeGrid( CompositeGrid &cg );
  int buildFromACompositeSurface( CompositeSurface & mapping );

  int findBoundaryCurves(int & numberOfBoundaryCurves, Mapping **& boundaryCurves );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  // read an unstructured grid from an ascii file.
  virtual int get( const aString & fileName );

  // save the unstructured grid to an ascii file.
  virtual int put(const aString & fileName = nullString ) const;

  Mapping *make( const aString & mappingClassName );

  int update( MappingInformation & mapInfo ) ;

  inline ElementType getElementType(int e) const;

  virtual aString getClassName() const { return UnstructuredMapping::className; }
 protected:
  
  int numberOfNodes, numberOfElements, numberOfFaces, numberOfEdges;
  int maxNumberOfNodesPerElement, maxNumberOfNodesPerFace, maxNumberOfFacesPerElement;
  int numberOfInternalElements, numberOfInternalFaces;
  int numberOfBoundaryElements, numberOfBoundaryFaces;
  bool preferTriangles;  // prefer building triangles or tets.
  real elementDensityTolerance; // for choosing the number of elements on a Mapping.
  real stitchingTolerance;      // relative tol for stitching surfaces together.
  real absoluteStitchingTolerance; // absolute tol for stitching surfaces together.
  int debugs;                   // debug for stitching

  GeometricADT3dInt *search;  // used to search for triangles nearby a point (Alternating Digital Tree)
  

  realArray node;
  intArray element, face, bdyFace, faceElements, edge;
  intArray *elementFaces;  // optionally holds elementFaces.

  enum TimingsEnum
  {
    totalTime=0,
    timeForBuildingSubSurfaces,
    timeForConnectivity,
    timeForProjectGlobalSearch,
    timeForProjectLocalSearch,
    timeForStitch,
    numberOfTimings
  };
  real timing[numberOfTimings];

  int buildConnectivityLists();
  void initMapping();

  // FEZ means Finite Element Zoo
  // connectivity templates for a finite element zoo
  // the connectivity should be abstracted away in some nice way...
  // for now, keep it simple, allocate some extra arrays and just do the bookkeeping...

  // parameters
  int numberOfElementTypes;

  // zone based templates
  IntegerArray numberOfFacesThisElementType;
  IntegerArray numberOfNodesThisElementType;
  IntegerArray numberOfNodesThisElementFaceType;
  IntegerArray elementMasterTemplate;

  // auxillary connectivity arrays
  intArray elementType;
  intArray faceZ1Offset;

  // element tag array ( for denoting regions, etc.)
  intArray tags;

  // auxillary connectivity methods (used when building the connectivity)
  // most of the following methods abstract out the ugliness of the FEZ connectivity
  int FEZComputeElementTypes();
  inline IntegerArray getElementFaceNodes(int element_, int faceOffset) const;
  inline int getNumberOfFacesThisElement(int element_) const;

  int FEZInitializeConnectivity();  // initialize the Finite Element Zoo Connectivity

  int addNodeToInterface( int s1, int & i1, int & j1, int & e1m, int & e1p, IntegerArray & connectionInfo1,
			  int s2, int & i2, int & j2, int & e2m, int & e2p, IntegerArray & connectionInfo2,
			  const intArray & elementface2, 
                          intArray * bNodep, IntegerArray & faceOffset, IntegerArray & elementOffset,
			  int maxNumberOfElements, int maxNumberOfFaces );

  int buildSearchTree();

  int computeConnection(int s, int s2, 
			intArray *bNodep,
			IntegerArray & numberOfBoundaryNodes,
			UnstructuredMapping *boundaryp,
			real epsx,
			IntegerArray & connectionInfo );
  
  void getNormal( int e, real *normalVector );

  bool isDuplicateNode(int i, int n, int e, int s, int s2, real & r0, real & r1,
                       realArray & x,
		       real epsDup,
		       intArray & bNode,
		       intArray & nodeInfo,
		       int & localEdge,           
		       real & dist0, real & dist1, real & dist2, int debugFlag);

  void replaceNode( int n, int n0, intArray & nodeInfo, intArray & ef );

  bool validStitch( int n, realArray & x0, realArray & x, intArray & nodeInfo, real tol, int debug);
  
 private:

  aString className;

  // the map function should not be called
  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((UnstructuredMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((UnstructuredMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new UnstructuredMapping(*this, ct); }

};

// most of the following inlined methods abstract out the ugliness of the FEZ connectivity
//\begin{>UnstructuredMappingInclude.tex}{\subsection{getElementFaceNodes}}
inline IntegerArray UnstructuredMapping::
getElementFaceNodes(int element_, int faceOffset) const
//===========================================================================
// /Description: get a list of the nodes on a particular face of an element
// /element\_ (input): the element in question
// /faceOffset (input): the face in the element whose nodes we want
// /Returns : IntegerArray of global node indices
// /Throws : nothing
//\end{UnstructuredMappingInclude.tex}
//===========================================================================
{
  assert(element_>-1 && element_<numberOfElements);
  int etype = elementType(element_);

  IntegerArray emslice;
  Index Iface(0,numberOfNodesThisElementFaceType(etype, faceOffset));
  IntegerArray retArray;

#if 0
  // this works but I am not sure it is any faster...
  emslice = elementMasterTemplate(etype, faceOffset, Iface);
  emslice.reshape(Iface);
  IntegerArray ia(Iface);
  ia = element_;
  retArray = element(ia, emslice);
  retArray.reshape(Iface);
#else
  retArray.resize(Iface);
  for (int i=0; i<Iface.getLength(); i++) 
    retArray(i) = element(element_, elementMasterTemplate(etype, faceOffset, i));
#endif
 
  return retArray;
}

//\begin{>>UnstructuredMappingInclude.tex}{\subsection{getNumberOfFacesThisElement}}
inline int UnstructuredMapping::
getNumberOfFacesThisElement(int element_) const
//===========================================================================
// /Description: get the number of faces in a particular element
// /element\_ (input): the element in question
// /Returns : int containing the number of faces in element\_
// /Throws : nothing
//\end{UnstructuredMappingInclude.tex}
//===========================================================================
{
  assert(element_>-1);
  return numberOfFacesThisElementType(elementType(element_));
}

//\begin{>>UnstructuredMappingInclude.tex}{\subsection{getNumberOfNodesThisElement}}
inline int UnstructuredMapping::
getNumberOfNodesThisElement(int element_)  const
//===========================================================================
// /Description: get the number of nodes in a particular element
// /element\_ (input): the element in question
// /Returns : int containing the number of nodes in element\_
// /Throws : nothing
//\end{UnstructuredMappingInclude.tex}
//===========================================================================
{
  assert(element_>-1);
  if( maxNumberOfNodesPerElement==3 )
    return 3;
  else
  return numberOfNodesThisElementType(elementType(element_));
}

//\begin{>>UnstructuredMappingInclude.tex}{\subsection{getNumberOfNodesThisFace}}
inline int UnstructuredMapping::
getNumberOfNodesThisFace(int face_)  const
//===========================================================================
// /Description: get the number of nodes in a particular face
// /face\_ (input): the face in question
// /Returns : int containing the number of nodes in face\_
// /Throws : nothing
//\end{UnstructuredMappingInclude.tex}
//===========================================================================
{
  assert(face_>-1);
  if( maxNumberOfNodesPerElement==3 )
    return 2;
  else
    return numberOfNodesThisElementFaceType(elementType(faceElements(face_,0)), faceZ1Offset(face_));
}

//\begin{>>UnstructuredMappingInclude.tex}{\subsection{getElementType}}
inline UnstructuredMapping::ElementType UnstructuredMapping::
getElementType(int e) const
//===========================================================================
// /Description: get the type of a particular element
// /element\_ (input): the element in question
// /Returns : UnstructuredMapping::ElementType
// /Throws : nothing
//\end{UnstructuredMappingInclude.tex}
//===========================================================================
{
  if( maxNumberOfNodesPerElement==3 )
    return triangle;
  else
    return ElementType(elementType(e));
}

//\begin{>>UnstructuredMappingInclude.tex}{\subsection{elementGlobalVertex}}
inline int UnstructuredMapping::
elementGlobalVertex(int elem, int vertex) const
//===========================================================================
// /Description: get the global vertex index for a node in an element
// /elem (input): the element in question
// /vertex (input): the node in the element whose global index is required
// /Returns : int, the global index of node vertex in element elem
// /Throws : nothing
//\end{UnstructuredMappingInclude.tex}
//===========================================================================
{
  return element(elem, vertex);
}

//\begin{>>UnstructuredMappingInclude.tex}{\subsection{faceGlobalVertex}}
inline int UnstructuredMapping::
faceGlobalVertex(int f, int v) const 
//===========================================================================
// /Description: get the global vertex index for a node in a face
// /f (input): the face in question
// /v (input): the node in the face whose global index is required
// /Returns : int, the global index of node v in face f
// /Throws : nothing
//\end{UnstructuredMappingInclude.tex}
//===========================================================================
{
  return element(face(f,0), elementMasterTemplate(elementType(face(f,0)), faceZ1Offset(f), v));
}

inline int UnstructuredMapping::
getBoundaryFace(int bdyface_) const
{
  return bdyFace(bdyface_);
}

#endif  



