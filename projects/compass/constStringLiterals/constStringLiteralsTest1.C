#include <stdlib.h>

void use(const char *);

void noncompliant()
{
    char *a = "foo!";
    a = "bar!";
    use(a);
    mktemp("/tmp/myfileXXXXXX");
}

void compliant()
{
    const char *a = "foo!";
    a = "bar!";
    use(a);
    char tmp[] = "/tmp/myfileXXXXXX";
    mktemp(tmp);
}
