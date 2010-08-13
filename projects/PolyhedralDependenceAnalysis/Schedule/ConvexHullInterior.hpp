
#include <vector>

#ifndef __CONVEX_HULL_INTERIOR_
#define __CONVEX_HULL_INTERIOR_

namespace Parma_Polyhedra_Library {
	class Polyhedron;
}

namespace PolyhedralToolsPPL {

	std::vector<int * > * allPointsInPolyhedron(Parma_Polyhedra_Library::Polyhedron * polyhedron);
	
	void printPoints(std::ostream & out, std::vector<std::vector<int> * > * set);

}

#endif /* __CONVEX_HULL_INTERIOR_ */
