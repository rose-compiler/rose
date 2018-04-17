// Example of multiple declarations (one of which contains default arguments)
void foo( int x, int y);
void foo( int x, int y);
int defaultValue = 0;
void foo( int x, int y = defaultValue);
void foo( int x, int y);
