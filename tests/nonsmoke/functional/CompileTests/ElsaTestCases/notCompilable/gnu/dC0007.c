// from the kernel
static int sstfb_decode_var()
{
  int x;
  switch (x) {
    case 0 ... 16 :
      break;

    // gcc-2 allows this      
    case 17 ... 31 :
      /* nothing */
  }
}
