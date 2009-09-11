
int main() {

    struct type {
        int x;
    }* p;

    int y;

    p = (struct type*) malloc( sizeof( struct type ));

    // read &p, &(p -> x)
    y = p -> x;

    return 0;
}
