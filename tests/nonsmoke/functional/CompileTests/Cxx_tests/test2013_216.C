template <class C>
class scoped_ptr 
   {
     public:
//        typedef C element_type;

          C& operator*() const;
//        C* ptr_;
   };

template <class C>
class scoped_array 
   {
     public:
          C& operator[](int i) const;
   };

class FieldGenerator {};

class FieldGeneratorMap 
   {
     public:
          const FieldGenerator& get_extension(int index) const;

     private:
          scoped_array<scoped_ptr<FieldGenerator> > extension_generators_;
   };


const FieldGenerator& FieldGeneratorMap::get_extension(int index) const 
   {
  // error: invalid initialization of reference of type 'const google::protobuf::compiler::java::FieldGenerator&' from expression of type 'google::protobuf::internal::scoped_ptr<google::protobuf::compiler::java::FieldGenerator>'
  // unparses as: return (this) -> extension_generators_[index];
     return *extension_generators_[index];
   }


