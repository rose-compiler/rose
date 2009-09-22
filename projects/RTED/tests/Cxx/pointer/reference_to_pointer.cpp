int main() {
    int x;
    int y;

    int* p;
    int*& r = p;

    p = &x;
    // error, we incremented from x to y, implicitly depending on a particular
    // memory layout
    ++r;

    return 0;
}
