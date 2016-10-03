// t0208.cc
// default type arg that demonstrates need to do bindings
// before/during use of default args

template <class S, class T = S*>
class C {
public:
  T t_inst;
};

int f()
{
  // default argument ought to be 'int*'
  C<int> x;
  
  int arr[ sizeof(x.t_inst) ];
  
  return 6;
}



