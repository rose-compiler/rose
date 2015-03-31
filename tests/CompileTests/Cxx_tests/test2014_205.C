namespace std
   {
     typedef unsigned long size_t;
     typedef unsigned long ptrdiff_t;
   }

namespace 
   {
     template<std::size_t Alignment>
     struct AlignedAllocator 
        {
          typedef std::size_t size_type;
          typedef std::ptrdiff_t difference_type;

          static void free(char * ptr) {}
        };

  // This part requires boost...
  // typedef boost::pool<AlignedAllocator<32> > Pool;

  // Pool pool(0,8);

   } // unnamed namespace

