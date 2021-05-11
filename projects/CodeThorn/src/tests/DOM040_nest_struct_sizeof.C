//#include <cstddef>

struct S {
  // from bits/types/struct_FILE.h
  //  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (long)];
};

int main() {
  S s;
}
