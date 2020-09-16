#include<typeinfo>

// This fails for clang++ version 10.0, but passes for GNU (all versions, as best I can tell).
// Original code: typedef std::type_info typeinfo;
// Generated code: typedef class std::type_info typeinfo;
typedef std::type_info typeinfo;

