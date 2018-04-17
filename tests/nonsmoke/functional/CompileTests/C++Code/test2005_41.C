
template <typename Mesh>
struct MeshConcept
   {
     void constraints()
        {
       // using namespace boost;
       // Every model of MeshConcept must also model SpaceConcept.
          function_requires<SpaceConcept<Mesh> >();
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

