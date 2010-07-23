
struct model
{
    int i;
};

void event0(model* m)
{
    int a = 0;
    //++a;
    m->i += a;
    m->i = 0;
}
