
// The first parameter must be size_t (but size_t is unsigned int, so this works)
// inline void* operator new(unsigned long, void* __p)  { return __p; }
// inline void* operator new(unsigned long, char* __p)  { return __p; }
// inline void* operator new[](unsigned long, void* __p)  { return __p; }

inline void* operator new(__SIZE_TYPE__, void* __p)  { return __p; }
inline void* operator new(__SIZE_TYPE__, char* __p)  { return __p; }
inline void* operator new[](__SIZE_TYPE__, void* __p)  { return __p; }

inline void  operator delete[](void* __p) {}

// I think this is allowed
inline void  operator delete[](void* __p,unsigned int) {}

