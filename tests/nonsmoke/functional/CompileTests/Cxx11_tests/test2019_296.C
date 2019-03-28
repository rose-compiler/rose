
void foobar()
   {
     const char (*pc)[sizeof("abc")] = &"abc";
     (*pc)[1];
     wchar_t const (*pwc)[sizeof(L"abc")/sizeof(L'a')] = &L"abc";
     (*pwc)[2];
   }
