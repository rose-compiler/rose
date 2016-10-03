
void foo()
   {
#if 1
  // This is the default, which is the same as if "u8" were used explicitly.
     "I'm a default (UTF-8) string.";
#endif
#if 1
  // This is the same as the default, so there is nothing to specify the use of "u8" explicitly.
     u8"I'm a UTF-8 string.";
#endif
#if 1
     u"This is a UTF-16 string.";
#endif
#if 1
     L"This is a wchar string.";
#endif
#if 1
     U"This is a UTF-32 string.";
#endif
   }
