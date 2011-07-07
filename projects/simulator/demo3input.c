#include <string.h>
#include <unistd.h>

int main()
{
    write(1, "bye\n", 4);
    return 0;
}

int payload(int seed)
{
    seed += (seed << 3);
    seed ^= (seed >> 11);
    seed += (seed << 15);
    return seed;
}
