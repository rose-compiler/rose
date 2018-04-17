// This is a bug demonstrated in BoxLib (in ROSE using BoxLib).

template <class T>
class BaseFab
   {
     public:

     void dataPtr () 
        {
        }

     void maskLT (BaseFab<int>& mask);

   };

#if 1
template <class T>
void
BaseFab<T>::maskLT (BaseFab<int> & mask)
   {
     mask.dataPtr();
   }
#endif
