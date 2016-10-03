// t0521.cc
// this test reveals a bit about the ordering of old vs. new
// messages in disambiguation error restoration; the behavior
// of Elsa on this input caused me to change 'takeMessages' to
// 'prependMessages' in ~DisambiguationErrorTrapper()

typedef int __cause_xfailure;

int foo()
{
  //x;
  __cause_xfailure(x);
  return x;
}
