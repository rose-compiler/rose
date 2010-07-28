
struct model
{
    int i;
    int j;
};

void event0(model* m)
{
#if 0
    int i = 10;
    m->i += ++i;
    if (int i = 10) ++m->i, ++i;
    //++m->i; 
#endif
    //int i = 1;
    m->i = 10;
    m->i += m->j;
    //m->j += i;
}
