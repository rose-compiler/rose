class Box
   {
     friend void minBox (int a);
   };

void minBox (int a);

class IntVectSet
   {
     public:
          IntVectSet& operator|=(const IntVectSet& ivs);

          void minBox() const;
};

IntVectSet& IntVectSet::operator|=(const IntVectSet& ivs)
   {
     ::minBox(42);

     return *this;
   }
