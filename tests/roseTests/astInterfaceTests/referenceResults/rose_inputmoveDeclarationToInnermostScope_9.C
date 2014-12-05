// Test case with complex for (init_stmt_list, . )

void fallbackSimpleSort(int *fmap,int *eclass,int lo,int hi)
{
  int i;
  unsigned int ec_tmp;
  if (lo == hi) {
    return ;
  }
  if (hi - lo > 3) {
    for ((i = hi - 4 , ec_tmp = 0); i >= lo; i--) {
      int tmp;
      int j;
      tmp = fmap[i];
      ec_tmp = eclass[tmp];
      for (j = i + 4; j <= hi && ec_tmp > eclass[fmap[j]]; j += 4) {
        fmap[j - 4] = fmap[j];
      }
      fmap[j - 4] = tmp;
    }
  }
  for (i = hi - 1; i >= lo; i--) {
    int tmp;
    int j;
    tmp = fmap[i];
    ec_tmp = eclass[tmp];
    for (j = i + 1; j <= hi && ec_tmp > eclass[fmap[j]]; j++) {
      fmap[j - 1] = fmap[j];
    }
    fmap[j - 1] = tmp;
  }
}
