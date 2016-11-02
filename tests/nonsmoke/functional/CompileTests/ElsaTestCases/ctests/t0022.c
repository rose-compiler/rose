// t0022.c
// differing prototypes for main()

//ERROR(1): int main(int);
//ERROR(2): int main;

int main(int, char**, char **);
int main(int, char**);
int main(int, char*[]);
//ERROR(3): int main(int, char**[]);
int main(int, char**)
  { return 0; }          //ERRORIFMISSING(4): avoid duplicate error

//ERROR(4): ; int main;

//int main();
//int main(int);
