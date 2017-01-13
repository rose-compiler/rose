// g0004.cc
// call some GNU __builtins, as gcc's cstream header does

void f()
{
  __builtin_strchr("foo", 'f');
  __builtin_strpbrk("foo", "fo");
  __builtin_strrchr("foo", 'o');
  __builtin_strstr("this is the haystack", "hay");
}
