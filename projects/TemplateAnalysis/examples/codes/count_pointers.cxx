
template <typename T>
struct countPointer {
  enum { value = 0 };
};

template <typename T>
struct countPointer<T*> {
  enum { value = countPointer<T>::value + 1 };
};

#ifdef RUNTEST
int main() {
  return countPointer<void *****>::value;
}
#endif

