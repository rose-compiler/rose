// Dynamic memory allocation for over-aligned data

#include <new>

// This is how Intel supported this feature.
// #include <aligned_new>

    namespace std {
        class bad_alloc;
        class bad_array_new_length;
        enum class align_val_t: size_t {};
     // struct nothrow_t {};
        extern const nothrow_t nothrow;
        typedef void (*new_handler)();
        new_handler get_new_handler() noexcept;
        new_handler set_new_handler(new_handler new_p) noexcept;

     // 18.6.4, hardware interference size
     // static constexpr size_t hardware_destructive_interference_size  = implementation-defined;
     // static constexpr size_t hardware_constructive_interference_size = implementation-defined;

     // DQ (1/2/2019): Make up values for implementation-defined behavior.
        static constexpr size_t hardware_destructive_interference_size  = 64;
        static constexpr size_t hardware_constructive_interference_size = 64;
    };

    void* operator new(std::size_t size);
    void* operator new(std::size_t size, std::align_val_t alignment);
    void* operator new(std::size_t size, const std::nothrow_t&) noexcept;
    void* operator new(std::size_t size, std::align_val_t alignment,
    			const std::nothrow_t&) noexcept;
    void operator delete(void* ptr) noexcept;
    void operator delete(void* ptr, std::size_t size) noexcept;
    void operator delete(void* ptr, std::align_val_t alignment) noexcept;
    void operator delete(void* ptr, std::size_t size,
    			std::align_val_t alignment) noexcept;
    void operator delete(void* ptr, const std::nothrow_t&) noexcept;
    void operator delete(void* ptr, std::align_val_t alignment,
    			const std::nothrow_t&) noexcept;
    void* operator new[](std::size_t size);
    void* operator new[](std::size_t size, std::align_val_t alignment);
    void* operator new[](std::size_t size, const std::nothrow_t&) noexcept;
    void* operator new[](std::size_t size, std::align_val_t alignment,
    			const std::nothrow_t&) noexcept;
    void operator delete[](void* ptr) noexcept;
    void operator delete[](void* ptr, std::size_t size) noexcept;
    void operator delete[](void* ptr, std::align_val_t alignment) noexcept;
    void operator delete[](void* ptr, std::size_t size,
    			std::align_val_t alignment) noexcept;
    void operator delete[](void* ptr, const std::nothrow_t&) noexcept;
    void operator delete[](void* ptr, std::align_val_t alignment,
    			const std::nothrow_t&) noexcept;

    void* operator new  (std::size_t size, void* ptr) noexcept;
    void* operator new[](std::size_t size, void* ptr) noexcept;
    void operator delete  (void* ptr, void*) noexcept;
    void operator delete[](void* ptr, void*) noexcept;

