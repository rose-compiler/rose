// t0289.cc
// repeated 'using' declarations

typedef int Int;

namespace N
{
  using ::Int;
  using ::Int;
}
