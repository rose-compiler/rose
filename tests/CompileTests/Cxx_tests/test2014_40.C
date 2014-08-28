class IntVect
   {
     public:
          friend void coarsen(IntVect& output);
   };

class DisjointBoxLayout
   {
     public:
          friend void coarsen(DisjointBoxLayout& output);
   };

void viewLevelNoFine()
   {
     DisjointBoxLayout layoutFineCoarsened;
     coarsen(layoutFineCoarsened);
   }

