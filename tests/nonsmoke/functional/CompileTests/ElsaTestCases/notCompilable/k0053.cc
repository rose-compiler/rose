// calling a function with a pointer to an undefined template class

// originally found in package ace_5.4.2.1-1

// a.ii:9:9: error: attempt to instantiate `::S1<int>', but no definition has
// been provided for `::S1<T>'

// ERR-MATCH: attempt to instantiate `.*?', but no definition has been provided

template <typename T> 
struct S1;

void foo(S1<int> *);

void f()
{
    S1<int>* ptr;
    
    // this is ok; if we *could* instantiate 'S1<int>', then we
    // could consider base class conversions, but not being able
    // to instantiate it is not fatal
    foo(ptr);

    return 0;
}

// now suppose we see a defn
template <typename T>
struct S1 {
  T x;
};

// and a legitimate need to instantiate
void g()
{
  S1<int> s1;
  s1.x = 5;
  //ERROR(1): s1.y = 6;    // no such member
}


