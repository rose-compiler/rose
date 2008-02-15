
// Build a templated function
template<typename T> void foobar (T x){ }

// Explicit instantiation of templated function
// This is unparsed as: "template void foobar < int > (int x,int x);"
// template void foobar<int> (int x);

int main()
   {
     foobar<int>(0);
     return 0;
   }

