/* nss_ldap-202-5/mp_alloc-WPlt.i */

struct A {
  int x;
};

int main(int argc) {
  struct A *a;
  /* in C mode gcc seems to allow NULL to merge with another type
     without affecting it */
  (argc ? ((void*)0) : a          ) -> x;
  (argc ? a          : ((void*)0) ) -> x;
}
