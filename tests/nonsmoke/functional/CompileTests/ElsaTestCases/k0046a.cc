// anonymous struct fields in a union

// originally found in package libselinux

// k0046a.cc:14:24: error: there is no member called `foo2' in union U1

// ERR-MATCH: there is no member called.*in union

union U1 {
    struct {
        int foo1;
        int foo2;
    };
};

void f()
{
  U1 u;
  u.foo1;
}

int x =  ((int) &((U1 *)0)->foo2);
