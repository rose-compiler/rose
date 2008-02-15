template<typename T>
struct template_class
   {
  // This fails the share IR nodes test in AST Consistancy tests
     void assign() {}
   };

template_class<int> class_x;
