
// Protobuf failing example.

template <typename GenericType>
struct GenericTypeHandler {};

template <typename Element>
struct RepeatedPtrField 
   {
     struct TypeHandler;
   };

template <typename Element>
struct RepeatedPtrField<Element>::TypeHandler : public GenericTypeHandler<Element> {};
