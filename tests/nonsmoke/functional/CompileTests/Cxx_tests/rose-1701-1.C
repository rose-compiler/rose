
namespace N {
  template <int v>
  struct X {
    static const int value = v;
  };
}

template <typename T, int s>
struct Y {
  T array[ N::X<s>::value ];
};

static_assert(sizeof(Y<int, 5>) == 5 * sizeof(int));

