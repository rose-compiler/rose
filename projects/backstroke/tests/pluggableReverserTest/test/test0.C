
struct model
{
    int i;
    static int k;
};

namespace my
{
    int x;
}

int g;

void event0(model* m)
{
    int a = 0;
    int b = 0;
    g = 0;
    //model::k = 0;
    my::x = 0;
    //++a;
    //m->i += a;
    //m->i = 0;
}
