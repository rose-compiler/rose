// This test code is taken from test2006_08.C and modified to address debugging 
// specific to code generation for the Intel compiler.

namespace Geometry
   {
     class PolyMesh {};

  // template <typename CoreMesh> class MeshBase {};

  // template <typename CoreMesh> void getCoreMesh(MeshBase<CoreMesh>& mesh) {}
     template <typename CoreMesh> void getCoreMesh() {}
   }

using namespace Geometry;

// DQ (11/18/2017): The GNU compiler permits name qualification of the getCoreMesh<>() function to be
// omitted, but both Intel (and EDG) require the name qualification (as I agree it should be required).
// So the error is in the unparser not outputing the name qualification in the genereated code, however
// this the name qualification is only required for the template function declaration, and not the 
// functions parameter type specification (because the using declaration is sufficent).
// template void getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);
// template void Geometry::getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);
template void Geometry::getCoreMesh<PolyMesh>();
