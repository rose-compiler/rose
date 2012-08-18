// Control and query object alignment

// C++11 allows variable alignment to be queried and controlled with alignof and alignas.

// The alignof operator takes a type and returns the power of 2 byte boundary on which 
// the type instances must be allocated (as a std::size_t). When given a reference type 
// alignof returns the referenced type's alignment; for arrays it returns the element 
// type's alignment.

// The alignas specifier controls the memory alignment for a variable. The specifier 
// takes a constant or a type; when supplied a type alignas(T) is short hand for 
// alignas(alignof(T)). For example, to specify that a char array should be properly 
// aligned to hold a float:

alignas(float) unsigned char c[sizeof(float)]

