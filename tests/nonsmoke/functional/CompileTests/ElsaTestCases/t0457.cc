// t0457.cc
// differing prototypes for main()

int main(int, char**, char **);
//int main(int, char**);
//int main(int);
//int main() { return 0; }

// main() cannot be overloaded
//ERROR(1): int main(float,float,float);

