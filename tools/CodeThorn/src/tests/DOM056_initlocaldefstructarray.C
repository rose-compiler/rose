#include <cstdio>

int main() {
  static char *realm, *opaque, *nonce, *qop;

  static struct X {
    const char *name;
    char **variable;
  } options[] = {
    { "realm", &realm },
    { "opaque", &opaque },
    { "nonce", &nonce },
    { "qop", &qop }
  };

  realm = opaque = nonce = qop = NULL;

  printf("%s\n",options[0].name);
  printf("%s\n",options[1].name);
  printf("%s\n",options[2].name);
  return 0;
}
