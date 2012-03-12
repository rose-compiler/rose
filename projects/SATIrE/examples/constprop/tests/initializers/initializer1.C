int main() {

  // multi-dim array init wcet_bench: ns.c

  // http://publications.gbdirect.co.uk/c_book/chapter6/initialization.html
  short month_days[] =
    {31,28,31,30,31,30,31,31,30,31,30,31};

  char *mnames[] ={
    "January", "February",
    "March", "April",
    "May", "June",
    "July", "August",
    "September", "October",
    "November", "December"
  };
  char str1[] = {'h', 'e', 'l', 'l', 'o', 0};
  
  /* no room for the null */
  //char str2[5] = "hello";

  /* room for the null */
  char str3[6] = "hello";

  /* room for the null and some more room */
  char str4[10] = "hello";

  float y[4][3] = {
    {1, 3, 5},      /* y[0][0], y[0][1], y[0][2] */
    {2, 4, 6},      /* y[1][0], y[1][1], y[1][2] */
    {3, 5, 7}       /* y[2][0], y[2][1], y[2][2] */
  };

  return 0;
}
