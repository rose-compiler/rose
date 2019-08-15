// Dynamic memory allocation for over-aligned data

#include <new>

namespace std {
	enum class align_val_t: size_t;
};

void *operator new(std::size_t, std::align_val_t);	// new overload

