// t0256.cc
// explicit instantiation of template function


// template primary decl
template <class T>
int foo(T t)
{
  return sizeof(T);
}

// explicit instantiation request
template
int foo(int t);

// another, with template arguments supplied
template
int foo<float>(float t);

// mismatch between inferred and supplied arguments
//ERROR(1): template
//ERROR(1): int foo<double>(char t);

