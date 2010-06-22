//#include <vector>


struct Point
{
    int x, y;
};

struct Model
{
    int i, j, k;
    int a[10];
    //std::vector<int> vec;
    Point p;
};


void event0(Model* m)
{
    m->i = m->j = 0;
    ++m->i;
    m->a[1] = m->i;
    *m;
    //m->vec[0] = 0;
    //m->p.x = 0;
}
