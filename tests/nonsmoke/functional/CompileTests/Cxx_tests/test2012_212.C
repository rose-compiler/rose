// This is the primary template (we will use partial specialization using this primary template below)
template<typename _Tp> struct __is_pointer{};

// This is a Partial Specialization (for pointer to _Tp).
template<typename _Tp> struct __is_pointer<_Tp*>{};

