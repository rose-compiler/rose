// deleting pointers of template-type

// error: can only delete pointers, not `T'

// originally found in package qt-x11-free

// ERR-MATCH: can only delete pointers

template <class T> struct S1 {
    static void foo() {
        void * x = 0;
        delete (T)x;
    }
};

void f()
{
  S1<int*>::foo();
}

