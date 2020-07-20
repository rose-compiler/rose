char* p = 0;
template<class T> T g(T x = &p) { return x; }
template int g<int>(int);   // OK even though &p isn’t an int.

