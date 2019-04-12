void foobar()
   {
     wchar_t const (*pwc)[sizeof(L"abc")/sizeof(L'a')] = &L"abc";
   }
