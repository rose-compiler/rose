
// The bug in ROSE is that this will be unparsed as:
//    void foobar(TYPE x);
// but this is a mistake since "TYPE" had not been defined yet.
void foobar(/* can't list the parameters since TYPE is not defined yet */);

typedef int TYPE;

void foobar(TYPE x);

void foobar(TYPE x) {}



