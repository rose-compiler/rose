
// These namespace declarations are required to demonstrate the bug.
// namespace google {
// namespace protobuf {
namespace internal {

}  // namespace internal
// }  // namespace protobuf
// }  // namespace google


// These namespace declarations are required to demonstrate the bug.
// namespace google {
// namespace protobuf {
namespace internal {

// This namespace is required so that 
//    "internal::FindRegisteredExtension(number);"
// has something to refer to that is without a function declaration for 
//    "const int *FindRegisteredExtension(int number);"
namespace internal {

}  // namespace internal

}  // namespace internal
// }  // namespace protobuf
// }  // namespace google

// The namespace that is not the first non-defining declaration
// namespace google {
// namespace protobuf {
namespace internal {

namespace ABC {

const int* FindRegisteredExtension(int number) 
   {
     return 0L;
   }

}  // namespace

void Find(int number) 
   {
     const int* extension = ABC::FindRegisteredExtension(number);
   }

}  // namespace internal
// }  // namespace protobuf
// }  // namespace google

