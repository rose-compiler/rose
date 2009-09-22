
int foo( int& x ) {
    x = 24601;
}


int main() {

    int x;
    // not a read of x, because the param is pass-by-reference
    foo( x );


    if( true ) {
        int* y = new int;
    // memory leak at scope exit
    }

    return 0;
}
