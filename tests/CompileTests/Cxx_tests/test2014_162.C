template <typename T>
class HistVarSet 
   {
     public:
          HistVarSet (int x) {}
#if 0
          template <typename T>
          void foo( HistVarSet<bool> flags, T xxx );
#endif
   };

void foobar()
   {
     HistVarSet<bool> log_flags(7);
   }

