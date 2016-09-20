// t0547.cc
// multiple using-directives naively conflict

typedef int ptrdiff_t;
namespace std {
  using ::ptrdiff_t;
  using std::ptrdiff_t;
}
