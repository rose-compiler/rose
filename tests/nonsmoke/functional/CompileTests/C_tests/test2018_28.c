// #include <wchar.h>
typedef int wchar_t;

int main(int argc, char **argv) {
    const char *astr = "a";
    const wchar_t wide = L'a'; // L'\u00FC';
    const wchar_t *pu_umlaut = L"\u00FC";
}
