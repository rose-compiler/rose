struct X
   {
     enum private_mode
        {
//        a,b,c
//        _S_app = 1L << 0
          _S_app = 1
        };

     typedef private_mode public_mode;

   };


// void foo (public_mode i);
void foo (X::private_mode i);

void foobar()
   {
  // foo((private_mode)0);
     foo(X::public_mode(0));
   }


