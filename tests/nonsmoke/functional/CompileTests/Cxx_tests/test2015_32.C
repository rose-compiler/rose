// TEST: default constructor and get/set functions

class RunTimeSystem {
public:
  void** avpush(void** p) { return p; }
  int* avpush(int* p) { return p; }
} rts;

template <typename T>
class A {
public:
  A();
  void set(T val);
  T get();
private:
  T _val;
};

template<typename T>
A<T>::A(){
}

template<typename T>
void A<T>::set(T val) {
  _val=val;
}

template<typename T>
T A<T>::get() {
  return _val;
}

int main() {
  A<int> a;
  int x1=1;
  a.set(x1);
  int x2=a.get();
  return !(x1==x2);
}
