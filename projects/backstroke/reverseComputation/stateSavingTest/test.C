#include <math.h>

struct point
{
    int x, y, z;
    mutable int counter;

    void operator = (const point& p)
    {
        x = p.x, y = p.y, z = p.z;
    }

    void reverse()
    {
        x = -x, y = -y, z = -z;
    }

    void reset() 
    {
        x = y = z = 0;
    }

    int distance(point& p) const
    {
        return sqrt(
                pow(x - p.x, 2) +
                pow(y - p.y, 2) +
                pow(z - p.z, 2));
    }

    bool isOrigin() const
    {
        ++counter;
        return x == 0 && y == 0 && z == 0;
    }
}

struct model
{
    point p;
    vector<int> v;
}

void event0(model* m)
{
    m->reset();
    m->v.resize(100);
    event1(m);
}

void event1(model* m)
{
    m->isOrigin();
    int i = m->v[0];
}

int main()
{
}
