// DQ (4/8/2014): This is a copy of the test2006_08.C file from Cxx_tests.
// it is part of a bug related to a change in the traversal for the 
// SgTemplateInstantiationDirective IR node.


#if 1
// #4 BUG MESSAGE GONE WHEN: remove the following line
// This causes the scope of subsequent declarations to reference this scope
// instead of the scope of the firstDefiningDeclaration.
namespace Geometry
   {
   } // end namespace Geometry
#endif

namespace Geometry
   {
     class PolyMesh
        {
          public:
        };

#if 1
     template <typename CoreMesh>
     class MeshBase;

     template <typename CoreMesh>
     void getCoreMesh(MeshBase<CoreMesh>& mesh) {}

     template <typename CoreMesh>
     class MeshBase
        {
          public:
            // #1 BUG MESSAGE GONE WHEN: remove the following line and the definition
               explicit MeshBase(const char* meshFile);
          protected:
            // #2 BUG MESSAGE GONE WHEN: remove the following line
               const CoreMesh   mCoreMeshPtr;
            // #3 BUG MESSAGE GONE WHEN: remove following line
               friend void getCoreMesh<CoreMesh>(MeshBase<CoreMesh>& mesh);
        }; // end class MeshBase

  // #1 BUG MESSAGE GONE WHEN: remove the following line and the declaration
     template <> MeshBase<PolyMesh >::MeshBase(const char* meshFile);
#else
     template <typename CoreMesh> void getCoreMesh(CoreMesh&) {}
#endif
   } // end namespace Geometry

using namespace Geometry;

// Note that specification of template argument as in getCoreMesh<Geometry::PolyMesh>
// is optional but only one instantiation directive is allowed
// template void Geometry::getCoreMesh<Geometry::PolyMesh>(Geometry::MeshBase<Geometry::PolyMesh >&);

// The following is not allowed:
// template void Geometry::getCoreMesh<Geometry::PolyMesh>(Geometry::MeshBase<>&);

// This is permitted (i.e. without the template argument explicitly
// specified as in getCoreMesh<Geometry::PolyMesh>
template void Geometry::getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);

// template void Geometry::getCoreMesh<>(Geometry::PolyMesh&);
