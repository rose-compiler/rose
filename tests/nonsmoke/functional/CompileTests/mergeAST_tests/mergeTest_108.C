// Deomonstrates multiple declarations in the same scope (non-unique declarations in global scope)
template <typename T> class X;
template <typename T> class X;

template <typename T> class X { class FIND_ME_IN_TEMPALTE_DECLARATION {}; };

template <typename T> class X;

template <> class X<int>;
template <> class X<int>;
template <> class X<int> { class FIND_ME_IN_TEMPLATE_INSTANTIATION_DECLARATION {}; };

X<int> x_int;


