// error: no template parameter list supplied for `S'

// explicit instantiation of template member function

// originally found in package aspell

// ERR-MATCH: no template parameter list supplied

template <typename T> struct S {
    void foo() {  
      // the following line of code contains an error that
      // is only found if S<int>::foo is instantiated; this
      // ensures the instantiation request below is not simply
      // being ignored
      //ERROR(1): typename T::foo x;
    }
};
template void S<int>::foo();
