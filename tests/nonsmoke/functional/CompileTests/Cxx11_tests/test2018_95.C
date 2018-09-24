template <typename allocator_t>

class MemPool

{

public:

  static inline MemPool<allocator_t>& getInstance()

  {

    static MemPool<allocator_t> pool{};

    return pool;

  }

};

