/*
Hi Dan,
when compiling the following code:
enum {
        need_more
};

void deflate_stored()
{
        1 ? need_more : need_more;
}

ROSE segfaults when in C or C99 only mode. PS! This segfault does not show
up in C++ mode.

Andreas
*/


enum {
        need_more
};

void deflate_stored()
{
        1 ? need_more : need_more;
}
