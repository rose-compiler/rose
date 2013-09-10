// This test code should be modified to build a template variable so that we 
// can test when name qualification is required for it's template arguments.

template<typename T>
class Templ 
   {
     public:
          static int templStatic;
   };

// This does not compile with EDG...
// template<typename T> Templ<T>::templStatic = 0;

Templ<void> A;


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

  // Can we make a template variable to test name qualification of it's template arguments.
  // var_AllocateArray<Descriptor::ExtensionRange>;

     Templ<void>::templStatic++;

     Templ<Descriptor::ExtensionRange>::templStatic++;
   }


  
