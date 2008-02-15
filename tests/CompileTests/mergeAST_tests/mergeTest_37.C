// We need a way of knowing when these are different!
// They are both marked as defining declarations!
// Maybe we need the template string in the unique name???
#if 0
// forward template declaration
template<typename T>
class templated_class;

template<typename T>
class templated_class;
#endif

#if 0
template<typename T>
class templated_class_declaration;
#endif

#if 1
template<typename T>
class templated_class_declaration;
#endif

#if 1
// template declaration (definition)
template<typename T>
class templated_class_declaration
   {
     public:
         T t;
   };
#endif

#if 0
template<typename T>
class templated_class_declaration;
#endif
