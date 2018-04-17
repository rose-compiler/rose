// DQ (4/8/2014): This is a copy of the test2006_08.C file from Cxx_tests.
// it is part of a bug related to a change in the traversal for the 
// SgTemplateInstantiationDirective IR node.

namespace Geometry
   {
     class PolyMesh {};

     template <typename T> void getCoreMesh(T&) {}
   }

using namespace Geometry;

template void Geometry::getCoreMesh<>(Geometry::PolyMesh&);
// template void getCoreMesh<>(PolyMesh&);
