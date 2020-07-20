extern thread_local constinit int x;
int f() { return x; } // no check of a guard variable needed

