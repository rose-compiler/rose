
char* foo (char * input) {
   char* p, *q, *r;
   char** pp; 
  char m;
  q = &m;
  r = q;
   pp = &p;
   *pp = r;
  char * p1 = *pp + 2;
   return input;
}


void goo (char* f(char*))
{
  char* content = "dklafj";
  f(content);
}

int main() {
   char* content = "adlfkj";
   char* content2 = new char[4];
   char* output = foo(content);
   goo(foo);
}
