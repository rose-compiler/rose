// g0025.cc
// explicit class spec w/o "template <>"
// allowed by gcc-2 bug

template <class T>
class string_char_traits {};

/* should say "template <>" here */
class string_char_traits<char> {};
