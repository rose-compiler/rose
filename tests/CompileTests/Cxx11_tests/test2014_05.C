// C++11 example of use of default keyword.
struct atomic_flag
   {
//   atomic_flag() noexcept = default;
//   atomic_flag(const atomic_flag&) = delete;
//   atomic_flag(const atomic_flag&) = default;
#if 1
  // This will be represented in the ROSE AST as a defining 
  // declaration (which is what it implies symantically).
     atomic_flag(const atomic_flag & af) = delete;
#else
     atomic_flag(const atomic_flag&);
#endif
#if 0
    ~atomic_flag() noexcept = default;
     atomic_flag(const atomic_flag&) = delete;
     atomic_flag& operator=(const atomic_flag&) = delete;
     atomic_flag& operator=(const atomic_flag&) volatile = delete;
#endif
   };
