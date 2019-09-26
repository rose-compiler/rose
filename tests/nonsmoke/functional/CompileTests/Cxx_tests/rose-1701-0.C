
namespace N {
  template <int v>
  struct X {
    static const int value = 5;
  };
}

template <typename T, int s>
struct Y {
  T array[ N::X<s>::value ];
};

