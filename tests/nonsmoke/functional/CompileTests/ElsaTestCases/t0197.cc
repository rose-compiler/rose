// t0197.cc
// forward-declared template class, and the template param
// names of the forward and actual decl are different

// relevant cppstd parts:
//   14.1 para 3: "... defines identifier ... in the scope of
//                the template declaration", i.e. and not
//                anywhere else
//   14.1 para 10: compares default arguments in templates to
//                 default arguments in functions, implicitly
//                 suggesting similar rules, particularly w.r.t.
//                 positional matching

// forward, using parameter name "S"
template <class S>
class C;

// create a pending instantiation
C<float> *foo;

// actual, using parameter name "T"
template <class T>
class C {
public:
  T *p;
};

// in fact, I have the same problem even if the first instantiation
// happens later
C<int> *bar;



