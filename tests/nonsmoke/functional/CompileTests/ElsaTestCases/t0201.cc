// t0201.cc
// specialize one parameter completely, leave behind one template parameter

// primary: two parameters
template <class S, class T>
struct C
{};

// specialization: one (the first!) parameter
template <class U>
struct C<int, U> 
{};

// make use of the primary
C<float,float> *p;

// and also use the specialization
C<int,float> *q;
