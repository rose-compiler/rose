// This test code is taken from test2006_08.C and modified to address debugging 
// specific to code generation for the Intel compiler.

namespace Geometry
   {
  // class PolyMesh {};

     class MeshBase {};

     void getCoreMesh(MeshBase& mesh) {}
   }

using namespace Geometry;

// DQ (11/18/2017): Note that when this is non-template, then the syntax is valid with and without 
// name qualifications, but means different things.
// DQ (11/18/2017): The GNU compiler permits name qualification of the getCoreMesh<>() function to be
// omitted, but both Intel (and EDG) require the name qualification (as I agree it should be required).
// So the error is in the unparser not outputing the name qualification in the genereated code, however
// this the name qualification is only required for the template function declaration, and not the 
// functions parameter type specification (because the using declaration is sufficent).
// template void getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);
// template void Geometry::getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);
// void Geometry::getCoreMesh(Geometry::MeshBase::PolyMesh >&);
// void Geometry::getCoreMesh(Geometry::MeshBase &);
void Geometry::getCoreMesh(MeshBase &);
