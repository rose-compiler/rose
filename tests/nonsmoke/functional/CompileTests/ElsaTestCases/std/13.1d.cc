// 13.1d.cc

int f(char*);
int f(char[]);            // same as f(char*)
int f(char[7]);           // same as f(char*)
int f(char[9]);           // same as f(char*)

int g(char(*)[10]);       
int g(char[5][10]);       // same as g(char(*)[10]);
int g(char[7][10]);       // same as g(char(*)[10]);
int g(char(*)[20]);       // different from as g(char(*)[10]);
