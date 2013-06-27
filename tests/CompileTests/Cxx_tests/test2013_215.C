
namespace google {
namespace protobuf {

namespace internal {

template <class C>
class scoped_ptr 
   {
     public:
          typedef C element_type;

          C& operator*() const;
          C* ptr_;
   };

template <class C>
class scoped_array 
   {
     public:
          C& operator[](int i) const;
   };

}  // namespace internal


using internal::scoped_ptr;
using internal::scoped_array;

}  // namespace protobuf
}  // namespace google







namespace google {
namespace protobuf {


class FieldDescriptor {};

}  // namespace protobuf
}  // namespace google


namespace google {
namespace protobuf {
namespace compiler {
namespace java {

class FieldGenerator {};

class FieldGeneratorMap 
   {
     public:
       // const FieldGenerator& get(const FieldDescriptor* field) const;
          const FieldGenerator& get_extension(int index) const;

     private:
       // const Descriptor* descriptor_;
       // scoped_array<scoped_ptr<FieldGenerator> > field_generators_;
          scoped_array<scoped_ptr<FieldGenerator> > extension_generators_;
   };

}  // namespace java
}  // namespace compiler
}  // namespace protobuf

}  // namespace google


namespace google {
namespace protobuf {
namespace compiler {
namespace java {

#if 0
const FieldGenerator& FieldGeneratorMap::get(const FieldDescriptor* field) const 
   {
     GOOGLE_CHECK_EQ(field->containing_type(), descriptor_);

  // error: invalid initialization of reference of type 'const google::protobuf::compiler::java::FieldGenerator&' from expression of type 'google::protobuf::internal::scoped_ptr<google::protobuf::compiler::java::FieldGenerator>'
  // return (this) -> field_generators_[(field ->  index ())];
     return *field_generators_[field->index()];
   }
#endif

const FieldGenerator& FieldGeneratorMap::get_extension(int index) const 
   {
  // error: invalid initialization of reference of type 'const google::protobuf::compiler::java::FieldGenerator&' from expression of type 'google::protobuf::internal::scoped_ptr<google::protobuf::compiler::java::FieldGenerator>'
  // return (this) -> extension_generators_[index];
     return *extension_generators_[index];
   }

}  // namespace java
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
