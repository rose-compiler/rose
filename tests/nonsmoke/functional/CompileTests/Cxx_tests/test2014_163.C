template <typename T>
class HistVarSet 
   {
     public:
#if 0
       // HistVarSet (int x) {}
          HistVarSet (int x);
#endif
#if 1
          void foo( HistVarSet<bool> flags );
#endif
   };

#if 1
void foobar( HistVarSet<bool> flags );
#endif

#if 0
HistVarSet<bool> bool_flags;
HistVarSet<float> float_flags;
#endif

