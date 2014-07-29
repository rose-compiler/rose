// C++11 example of use of default keyword.
struct atomic_flag
   {
//   atomic_flag() noexcept = default;
//   atomic_flag(const atomic_flag&) = delete;
//   atomic_flag(const atomic_flag&) = default;
     atomic_flag(const atomic_flag&) = delete;

#if 0
    ~atomic_flag() noexcept = default;
     atomic_flag(const atomic_flag&) = delete;
     atomic_flag& operator=(const atomic_flag&) = delete;
     atomic_flag& operator=(const atomic_flag&) volatile = delete;
#endif
   };
