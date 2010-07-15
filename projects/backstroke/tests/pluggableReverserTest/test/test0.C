#include <rctypes.h>

struct model
{
    int i;
};

void event0(model* m)
{
    int i = 10;
    m->i += ++i;
}
