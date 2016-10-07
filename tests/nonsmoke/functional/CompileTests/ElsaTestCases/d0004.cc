template < class T > struct already_AddRefed {};
inline const int do_QueryInterface (int *aRawPtr, int *error = 0);
template < class T > inline void do_QueryInterface (already_AddRefed < T > &);
template < class T > class nsCOMPtr {
  void Assert_NoQueryNeeded () {
    T* mRawPtr;                 // added, since there was no declaration for mRawPtr
    do_QueryInterface (mRawPtr);
  }
};
