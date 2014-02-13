#include <vector>

struct A
{
    int _val;
    A() { }
    A(int v) : _val(v) { }
};

typedef struct A SA;

int main()
{
    std::vector<SA> savec;
    for(int i = 0; i < savec.size(); ++i) {
        savec[i] = 0;
    }
    return 0;
}
