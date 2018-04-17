
void fallbackSimpleSort(int *fmap,int *eclass,int lo,int hi)
{
  if (lo == hi) 
    return ;
  if (hi - lo > 3) {
    for (int i = hi - 4; i >= lo; i--) {
      int j;
      int tmp;
      unsigned int ec_tmp;
      tmp = fmap[i];
      ec_tmp = eclass[tmp];
      for (j = i + 4; j <= hi && ec_tmp > eclass[fmap[j]]; j += 4) 
        fmap[j - 4] = fmap[j];
      fmap[j - 4] = tmp;
    }
  }
  for (int i = hi - 1; i >= lo; i--) {
    int j;
    int tmp;
    unsigned int ec_tmp;
    tmp = fmap[i];
    ec_tmp = eclass[tmp];
    for (j = i + 1; j <= hi && ec_tmp > eclass[fmap[j]]; j++) 
      fmap[j - 1] = fmap[j];
    fmap[j - 1] = tmp;
  }
}
