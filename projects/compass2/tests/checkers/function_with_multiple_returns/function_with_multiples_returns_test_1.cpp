// 2 matches

int multiple_returns()
{
    if (0)
      return 0;
    else
      return 1;
}

int multiple_returns_2()
{
    switch(1)
    {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        default: return -1;
    }
}

// Single return
int main()
{
  return 0;
}
