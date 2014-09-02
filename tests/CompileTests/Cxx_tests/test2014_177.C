
// class ExtensionInfo {};

#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 10) )
// This version of the code is supported by EDG, but not by at least GNU 4.4.7.
class GeneratedExtensionFinder
   {
     public:
       // bool Find(int number, ExtensionInfo* output);
          bool Find(int number);
   };

namespace google {
namespace protobuf {

#else
// This version of code is required for GNU 4.4.7 (and likely othe versions)
namespace google {
namespace protobuf {
namespace internal {

class GeneratedExtensionFinder
   {
     public:
       // bool Find(int number, ExtensionInfo* output);
          bool Find(int number);
   };
}
#endif

namespace internal {

namespace {

// const ExtensionInfo* FindRegisteredExtension(int number) 
const int FindRegisteredExtension(int number) 
   {
     return 0;
   }

}  // namespace

// bool GeneratedExtensionFinder::Find(int number, ExtensionInfo* output) 
// bool GeneratedExtensionFinder::Find(int number) 
// bool google::protobuf::GeneratedExtensionFinder::Find(int number) 
bool GeneratedExtensionFinder::Find(int number) 
   {
  // const ExtensionInfo* extension = FindRegisteredExtension(containing_type_, number);
  // const ExtensionInfo* extension = FindRegisteredExtension(number);
#if 0
     const int extension = FindRegisteredExtension(number);
#else
     const int extension = internal::FindRegisteredExtension(number);
#endif
     return false;
   }

}  // namespace internal
}  // namespace protobuf
}  // namespace google

