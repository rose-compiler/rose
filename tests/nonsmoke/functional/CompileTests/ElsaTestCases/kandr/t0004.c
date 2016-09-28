// t0004.c
// "static" with implicit int in a definition that has
// a K&R parameter list

static /*implicit-int*/ tinfomove(row, col)
  register int row, col;
{
    return(0);
}
