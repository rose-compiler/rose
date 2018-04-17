// from the kernel
struct qstr {
        const unsigned char * name;
        unsigned int len;
        unsigned int hash;
};
int main() {
  // in the kernel it is used as a call to a function, not assigned to
  // a pointer as it is here; I didn't want the extra function call
  // layer when reading the AST
  const struct qstr *x =
    &(const struct qstr) { "bdev:", 5, 0 };
}
