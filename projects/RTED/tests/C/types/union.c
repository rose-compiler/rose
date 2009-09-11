
int main() {

    union {
        int*    p;
        int     x;
    } u;

    u.p = malloc( sizeof( int ));

    // error, we have a memory leak now that we've overwritten p
    u.x = 1943;

    free( u.p );

    return 0;
}
