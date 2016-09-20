
class Descriptor
   {
     public:
          class ExtensionRange{};
   };

class DescriptorPool
   {
     public:
          class Tables;
   };

template<typename Type> Type* foo_AllocateArray(int count);

class DescriptorPool::Tables 
   {
     public:
          template<typename Type> Type* AllocateArray(int count);
   };

class DescriptorBuilder 
   {
     DescriptorPool::Tables* tables_;  // for convenience

     void NewPlaceholder();

   };

void DescriptorBuilder::NewPlaceholder()
   {
  // Example using a non-member function
     foo_AllocateArray<Descriptor::ExtensionRange>(1);

  // Original example using member function.
     tables_->AllocateArray<Descriptor::ExtensionRange>(1);
   }


  
