#include <vector>
#include <list>
#include <assert.h>

struct point
{
    int x, y;

    point() : x(0), y(0) {}
};

bool operator == (const point& p1, const point& p2)
{
    return (p1.x == p2.x) && (p1.y == p2.y);
}

bool operator != (const point& p1, const point& p2)
{
    return !(p1 == p2);
}

struct model
{
    int i, j, k;
    int a[10];
    std::vector<int> vec;
    std::list<point> point_set;
    point p;
};

void event0(model* m)
{
    m->i = m->j = 10;
    ++m->i;
    m->a[1] = m->i;
    if (!m->vec.empty())
        m->vec.front() = 10;
    m->point_set.clear();
    m->p.x += m->p.y;
}
