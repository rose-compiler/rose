// namespace std 
// {
//   template <class T> class vector {};
     template <class T> class vector;
// }

template <class T>
class BaseFab
   {
     public:
          void setVal (T x);
          void maskLT (BaseFab<int>& mask );
   };

template <class T>
void
BaseFab<T>::maskLT (BaseFab<int>& mask)
   {
     mask.setVal(0);
   }

BaseFab< vector<int> > hash;
// BaseFab< int > hash;

