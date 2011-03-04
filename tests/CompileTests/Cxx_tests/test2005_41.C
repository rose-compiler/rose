#if (__GNUC__ == 3 && __GNUC_MINOR__ == 4)
/* TOO (2/15/2011): support for Thrifty (gcc 3.4.4)
 /usr/lib/gcc/x86_64-redhat-linux/4.1.2/../../../../lib64/crt1.o: In function `_start':
 (.text+0x20): undefined reference to `main'
*/
int main () {return 0;}
#else
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
#endif
