// Demonstrates bug specific to how the template declaration is handled in merge
// This has cause the generateUniqueName function to add an explicit counter value to the 
// names being generated (see the note there) since both "assign" template member functions 
// will be mangled to be the same!
template<typename T>
struct template_class
   {
  // In EDG this will be of kind: templk_member_function
     void assign(int parameter_a) {}

  // In EDG this will be of kind: templk_function
     template<typename S> void assign(int parameter_b) {}

  // In EDG this will be of kind: templk_member_function
     void assign(int parameter_c, int parameter_d) {}
   };

template_class<int> class_x;

