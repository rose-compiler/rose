#include <stdio.h>
#include <stddef.h>
#include <wchar.h>
#include <locale.h>

int main(void) {
  const wchar_t wc = L'\x3b1';
  const wchar_t *pu_umlaut = L"\u00FC";

  const char c = '\x3b1';
  const char *cc = "\u00FC";
  const char *ccc = "00FC";

  setlocale(LC_ALL, "en_US.UTF-8");
  wprintf(L"%lc\n", wc);
  wprintf(L"%ls\n", pu_umlaut);
 
  return 0;
}

