
template <typename T>
class Vect {
  private:
    typedef T v_t;

  public:
    struct It {
      It operator++ ();
      bool operator!= (It &);
      v_t & operator* ();
    };

    It begin() {}
    It end() {}
};

void foo() {
  Vect<int> vect;
  for (auto & i : vect) {}
}

