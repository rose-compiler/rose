class IntVect
   {
     public:
          friend void coarsen(IntVect& output,const IntVect& input);
   };

// #include "BoxLayoutData.H"
// #include "DisjointBoxLayout.H"

class DisjointBoxLayout //: public BoxLayout
   {
     public:
          friend void coarsen(DisjointBoxLayout& output,const DisjointBoxLayout& input,int refinement);
   };

template<class T>
class BoxLayoutData // : public LayoutData<T>
   {
     public:
   };

class FArrayBox
   {
     public:
   };

template<class T> class LevelData : public BoxLayoutData<T>
   {
     public:
          const DisjointBoxLayout& disjointBoxLayout() const;

   };

void
viewLevelNoFine(const LevelData<FArrayBox>* a_dataPtr,
                const LevelData<FArrayBox>* a_dataFinePtr,
                int a_refRatio)
   {
     const DisjointBoxLayout& layoutOrig = a_dataPtr->disjointBoxLayout();
     const DisjointBoxLayout& layoutFine = a_dataFinePtr->disjointBoxLayout();
     DisjointBoxLayout layoutFineCoarsened;
     coarsen(layoutFineCoarsened, layoutFine, a_refRatio);
   }

