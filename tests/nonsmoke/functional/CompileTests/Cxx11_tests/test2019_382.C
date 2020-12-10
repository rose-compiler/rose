
// This is a failing case in protobuf (L2 regression test application)

template <typename GenericType>
struct GenericTypeHandler {};

template <typename Element>
struct RepeatedPtrField {
  struct TypeHandler;
};

template <typename Element>
struct RepeatedPtrField<Element>::TypeHandler : public GenericTypeHandler<Element> {};
