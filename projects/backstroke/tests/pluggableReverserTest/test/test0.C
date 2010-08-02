
struct model
{
    int i;
    int j;
    //static int k;
};

#if 0
namespace my
{
    int x;
}

int g;
#endif

void event0(model* m)
{
    //int a = 0;
    int t = m->i;
    //m->i = m->j;
    //m->j = t;
    m->i = m->i + t;
    m->i = -m->i + t;
    //int q,w,e,r,t,y;
    //g = 0;
    //model::k = 0;
    //my::x = 0;
    //++a;
    //m->i += a;
    //m->i = 0;
}
