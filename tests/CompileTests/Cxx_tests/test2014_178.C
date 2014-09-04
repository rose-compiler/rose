
#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 10) )
// This version of the code is supported by EDG, but not by at least GNU 4.4.7.
class GeneratedExtensionFinder
   {
     public:
          bool Find(int number);
   };

namespace internal {

#else
// This version of code is required for GNU 4.4.7 (and likely othe versions)
namespace internal {

class GeneratedExtensionFinder
   {
     public:
          bool Find(int number);
   };
#endif

namespace {

const int FindRegisteredExtension(int number) 
   {
     return 0;
   }
}  // namespace

bool GeneratedExtensionFinder::Find(int number) 
   {
  // We need to do the name qualification based on the structural position (set of scopes) 
  // of this function instead of the semantic position (set of scopes). 
     const int extension = FindRegisteredExtension(number);

     return false;
   }

}  // namespace internal

