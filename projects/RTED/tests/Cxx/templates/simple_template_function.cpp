
template< class T > void foo( T* t ) {

    // error, double new, memory leak
    t = new T;
    t = new T;
}


int main() {
	int NULL=0;
    foo( (int*) NULL );

    return 0;
}
