
// DQ (2/20/2010): Added to handle g++ 4.x compilers.
// template <typename Mesh> struct SpaceConcept {};

template <typename Mesh>
struct MeshConcept
   {
     void constraints()
        {
       // using namespace boost;
       // Every model of MeshConcept must also model SpaceConcept.
       // DQ (2/20/2010): I think that this only works with older GNU compilers.
#if (__GNUC__ == 3)
          function_requires< SpaceConcept<Mesh> >();
#endif
        }

   }; // end struct MeshConcept

template <typename Mesh>
class PackageMesh
   {
     public:
//        typedef void (MeshConcept<Mesh>::* funcMeshMeshConcept)();
   };

void foo()
   {
  // PackageMesh<int> package;
     typedef void (MeshConcept<int>::* funcMeshMeshConcept)();
  // PackageMesh<int>::funcMeshMeshConcept functionPointer;
   }

