template<typename A, typename B>
class TemplateClass {
 public:
  void temp_mem_func() {
    int i_temp = 0;
    &i_temp;
  }
};

template <class T>
T tempFunc(T& t_ref) {
  int i_temp2 = 0;
  &i_temp2;
  return t_ref;
}

int main() {
  TemplateClass<int, bool> t;
  t.temp_mem_func();

  int i = 1;
  int i2 = tempFunc<int>(i);

  int (*fp)(int&);
  fp = tempFunc<int>;
}
