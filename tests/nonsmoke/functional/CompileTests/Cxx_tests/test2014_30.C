#if 0
class Box
   {
     friend void minBox (Box & a, Box & b);
   };
#endif

void minBox (int a);

class IntVectSet
   {
     public:
          IntVectSet& operator|=(const IntVectSet& ivs);

          int minBox() const;
};

IntVectSet& IntVectSet::operator|=(const IntVectSet& ivs)
   {
     ::minBox(42);

     return *this;
   }
