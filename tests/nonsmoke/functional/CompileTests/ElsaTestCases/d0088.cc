// dsw: initializers of members need to be typechecked on the first
// pass so they can be const-evaled if necessary

struct A {
   static const int a=64;
   int b[a];                    // const eval of 'a' here
};
