#if 0
namespace Geometry
   {
     class PolyMesh
        {
          public:
        };

      typedef int testTypeA;

      void foo ( testTypeA x );

      class X
         {
           public:
                int a;
                friend void foo ( testTypeA x );
         };

      template <class T>
      class Y
         {
           public:
                T a;
                friend void foo ( testTypeA x ) {};
         };

   } // end namespace Geometry

void Geometry::foo ( Geometry::testTypeA x ) {};

void foobar ( Geometry::testTypeA x );

#if 1
// #4 BUG MESSAGE GONE WHEN: remove the following line
// This causes the scope of subsequent declarations to reference this scope
// instead of the scope of the firstDefiningDeclaration.
namespace Geometry
   {
   } // end namespace Geometry
#endif

using namespace Geometry;

#endif




namespace Geometry
   {
     class PolyMesh
        {
          public:
        };

     template <typename CoreMesh>
     class MeshBase;

         template <typename CoreMesh> void getCoreMesh(MeshBase<CoreMesh>& mesh) {}

     template <typename CoreMesh>
     class MeshBase
        {
          public:
            // #1 BUG MESSAGE GONE WHEN: remove the following line and the definition
               explicit MeshBase(const char* meshFile);
          protected:
            // #2 BUG MESSAGE GONE WHEN: remove the following line
            // const CoreMesh   mCoreMeshPtr;
            // #3 BUG MESSAGE GONE WHEN: remove following line
               friend void getCoreMesh<CoreMesh>(MeshBase<CoreMesh>& mesh);
        }; // end class MeshBase

  // #1 BUG MESSAGE GONE WHEN: remove the following line and the declaration
     template <> MeshBase<PolyMesh >::MeshBase(const char* meshFile);

   } // end namespace Geometry

#if 1
// #4 BUG MESSAGE GONE WHEN: remove the following line
// This causes the scope of subsequent declarations to reference this scope
// instead of the scope of the firstDefiningDeclaration.
namespace Geometry
   {
   } // end namespace Geometry
#endif

using namespace Geometry;

// Note that specification of template argument as in getCoreMesh<Geometry::PolyMesh>
// is optional but only one instantiation directive is allowed
// template void Geometry::getCoreMesh<Geometry::PolyMesh>(Geometry::MeshBase<Geometry::PolyMesh >&);

// The following is not allowed:
// template void Geometry::getCoreMesh<Geometry::PolyMesh>(Geometry::MeshBase<>&);

// This is permitted (i.e. without the template argument explicitly
// specified as in getCoreMesh<Geometry::PolyMesh>
template void Geometry::getCoreMesh<>(Geometry::MeshBase<Geometry::PolyMesh >&);
