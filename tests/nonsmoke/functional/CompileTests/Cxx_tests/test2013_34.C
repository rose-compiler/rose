
namespace std 
   {
     template <class T> class vector {};
   }

template <class T>
class BaseFab
   {
     public:
          void setVal (T x);
          void maskLT (BaseFab<int>& mask ) const;
   };

template <class T>
void
BaseFab<T>::maskLT (BaseFab<int>& mask) const
   {
     mask.setVal(0);
   }

BaseFab< std::vector<int> > hash;

