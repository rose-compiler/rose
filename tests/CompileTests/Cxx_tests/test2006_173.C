enum A {
   B = 1
};

#define B 1

void foo () {
  A a = (A)B;
}
