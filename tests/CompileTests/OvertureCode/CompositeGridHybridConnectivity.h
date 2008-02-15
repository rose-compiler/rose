#ifndef __COMPOSITE_GRID_HYBRID_CONNECTIVITY__
#define __COMPOSITE_GRID_HYBRID_CONNECTIVITY__

#include <iostream.h>
#include "OvertureTypes.h"
#include "aString.H"
#include "AbstractException.h"


class CompositeGridHybridConnectivity
//\begin{>CompositeGridHybridConnectivity.tex}{\subsection{Composite Grid Hybrid Connectivities}}
//\no function header:
// A {\tt CompositeGridHybridConnectivity} manages the mappings between the structured and 
// unstructured components of hybrid meshes contained in a CompositeGrid.  This class
// enables a user to iterate through the boundary faces of an unstructured mesh and access
// the adjacent structured elements.  The inverse is also available where
// the user iterates through the elements in a structured grid seeking the adjacent unstructured 
// elements, if any exist.
//\end{CompositeGridHybridConnectivity.tex}
{

public:

  //
  // constructors
  //
  CompositeGridHybridConnectivity();
  CompositeGridHybridConnectivity(const int &grid_,
				  intArray * gridIndex2UVertex_,
				  intArray & uVertex2GridIndex_,
				  intArray * gridVertex2UVertex_,
				  intArray & boundaryFaceMapping_);

  //
  // destructor
  //
  ~CompositeGridHybridConnectivity();

  //
  // public methods
  //

  // set the mapping data structures
  void setCompositeGridHybridConnectivity(const int &grid_,
					  intArray * gridIndex2UVertex_,
					  intArray & uVertex2GridIndex_,
					  intArray * gridVertex2UVertex_,
					  intArray & boundaryFaceMapping_);

  // destroy all the connectivity data structures
  void destroy();

  // get the vertex id mapping for a particular grid 
  const intArray & getGridIndex2UVertex(int grid_) const; 

  // get vertexIndexMapping (inverse of vertexIDMapping)
  const intArray & getUVertex2GridIndex() const;

  // get the indices on a particular grid that are on the structured/unstructured boundary
  const intArray & getGridVertex2UVertex(int grid_) const;

  // return the number of vertices on the structured/unstructred interface of a particular grid
  int getNumberOfInterfaceVertices(int grid_) const;

  // get the boundary face mapping array dimensioned (nBoundaryFaces, 4) where on the second index :
  //  0 - unstructured mesh vertex
  //  1 - grid number for the adjacent structured zone
  //  2,3,4 - i1,i2,i3 : the indices for the adjacent zone in the structured grid
  const intArray & getBoundaryFaceMapping() const;

  int getUnstructuredGridIndex() const { return grid; }
  // 
  // exceptions
  //
  //class CompositeGridHybridConnectivityError;

protected:

//\begin{>>CompositeGridHybridConnectivity.tex}{\subsection{Connectivity data}}
//\no function header:
// \itemize
//  \item gridIndex2UnstructuredVertex : maps a particular grid and index 
// into an unstructured grid and vertex id
//  \item unstructuredVertex2GridIndex : maps an unstructured vertex into a grid and index
//  \item gridVertex2UnstructuredVertex : condensation of vertexIDMapping, contains all the vertices on a particular grid that are on
// a hybrid interface
//  \item boundaryFaceMapping : maps the boundary element of an unstructured mesh into a grid and zone index
//\end{itemize}
//\end{CompositeGridHybridConnectivity.tex}
  //
  // vertex connectivities
  //
  intArray *gridIndex2UnstructuredVertex;
  intArray unstructuredVertex2GridIndex;
  intArray *gridVertex2UnstructuredVertex;

  //
  // face/element connectivities
  //
  intArray boundaryFaceMapping;

private:

  //
  // grid number in cg that holds the unstructured grid referred to in this instance
  //
  int grid;

};

class CompositeGridHybridConnectivityError : public AbstractException
{

public:
  CompositeGridHybridConnectivityError() { message = ""; }
  virtual void debug_print() const { cout<<"CompositeGridHybridConnectivityError : "<<message; }
  aString message;
};

#endif
