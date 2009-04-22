
# define st_mtime st_mtim.tv_sec

struct StatA
{

  int tv_sec;
};

struct StatB
{
  struct StatA st_mtim;
};

int main()
{

  struct StatB statbuf;
  &(statbuf.st_mtime);
  statbuf.st_mtime;
};
