template<typename A, typename B>
class TemplateClass {
 public:
  void temp_mem_func() {
    int i_temp = 0;
    &i_temp;
  }
};

template <class T>
T tempFunc(T& t) {
  int i_temp2 = 0;
  &i_temp2;
  return t;
}

int main() {
  TemplateClass<int, bool> t;
  t.temp_mem_func();

  int i = 1;
  int i2 = tempFunc<int>(i);
}
