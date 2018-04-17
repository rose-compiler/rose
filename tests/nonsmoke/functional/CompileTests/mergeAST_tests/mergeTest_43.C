// template declaration (definition)
template<typename T>
class templated_class
   {
     public:
         T t;
   };

templated_class<int> x;
// templated_class<templated_class<int> > y;

