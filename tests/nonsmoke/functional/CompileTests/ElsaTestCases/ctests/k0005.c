// error: multiply defined enum `option'

// originally found in package tcl8.4

// ERR-MATCH: multiply defined enum

void foo()
{
    enum option { a=1 };
    if(1) {
        enum option { a=2 };
    }
}
