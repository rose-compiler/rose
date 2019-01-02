// guaranteed copy elision through simplified value categories

struct X {};
X &&a = X(); // binds directly
int &&b = int(); // used to not bind directly, now does

