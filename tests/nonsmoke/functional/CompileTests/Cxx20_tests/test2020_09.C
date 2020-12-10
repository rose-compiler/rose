char a[] = "abc";
// equivalent to char a[4] = {'a', 'b', 'c', '\0'};
 
//  unsigned char b[3] = "abc"; // Error: initializer string too long
unsigned char b[5]{"abc"};
// equivalent to unsigned char b[5] = {'a', 'b', 'c', '\0', '\0'};
 
wchar_t c[] = {L"кошка"}; // optional braces
// equivalent to wchar_t c[6] = {L'к', L'о', L'ш', L'к', L'а', L'\0'};

