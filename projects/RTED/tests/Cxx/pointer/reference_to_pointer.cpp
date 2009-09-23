int main() {
    int x;
    int y;

    int* p;
    int*& r = p;

    p = &x;
    // unsafe increment from &x to &y
    ++r;

    // error, p is invalidated because we incremented it badly, implicitly
    // depending on a particular memory layout
    int z = *p;

    return 0;
}
