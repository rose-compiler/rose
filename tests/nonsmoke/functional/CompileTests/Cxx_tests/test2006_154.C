
#if __cplusplus

// C++ specific version of problem showing friend with fully qualified name.
class mpi2cppList
   {
     public:
          class iter;
          class Link
             {
            // friend class iter;
               friend class mpi2cppList::iter;
               Link *next;
             };

          class iter
             {
               private:
                    Link* node;
               public:
                    iter& operator++() { node = node->next; return *this; }
                    iter operator++(int) { iter tmp = *this; ++(*this); return tmp; }
             };
   };

#else

// C specific version that demonstrates similar issue.
struct mpi2cppList
   {
     struct iter;
     struct Link
        {
          struct iter;
          struct Link *next;
        };

     struct iter
        {
          struct Link* node;
        };
   };

#endif
