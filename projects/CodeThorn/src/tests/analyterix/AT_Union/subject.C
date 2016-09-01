union U3 {
  int a3;
  bool b3;
};

union U4 {
  int a4;
  bool b4;
};

void func(U3& u3) {
  &(u3.a3);
}

int main() {
  // nested unions share the same memory location
  union U {
    int a;
    bool b;
    union {
      int c1;
      bool c2;
      union {
        int c31;
        union {
          int c321;
          bool c222;
        } c32;
        bool c33;
        U4 c34;
      } c3;
    } c;
  } u;
  int* ip = &(u.a); // address-taking of a AND b

  union U2 {
    int a2;
    bool b2;
  };
  U2 u2;

  auto lambda = [&](){
    &(u2.b2);
  };
}
