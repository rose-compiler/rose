
// The first parameter must be size_t (but size_t is unsigned int, so this works)
inline void* operator new(unsigned int, void* __p)  { return __p; }
inline void* operator new(unsigned int, char* __p)  { return __p; }

inline void* operator new[](unsigned int, void* __p)  { return __p; }
inline void  operator delete[](void* __p) {}

// I think this is allowed
inline void  operator delete[](void* __p,unsigned int) {}

