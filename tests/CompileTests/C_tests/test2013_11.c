// C99 example of initialization of a union using a designated initializer.
union {
      char birthday[9];
      int age;
      float weight;
      } people = { .age = 14 };
