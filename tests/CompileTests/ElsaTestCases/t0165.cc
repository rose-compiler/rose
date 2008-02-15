// t0165.cc
// specialization of nsCOMPtr
        
template <class T>
class nsCOMPtr {
  // nsCOMPtr === nsCOMPtr<T>
};
      
// specialization
template <>
class nsCOMPtr<int> {
  // nsCOMPtr === nsCOMPtr<int>
  nsCOMPtr(nsCOMPtr<int> &blah);
};
