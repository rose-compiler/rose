struct Struct_2 {
  template<class T>
  class Class_1;
};

template<typename Value>
struct Struct_1;

template<typename Value>
struct Struct_1<typename Struct_2::Class_1<Value> >;
