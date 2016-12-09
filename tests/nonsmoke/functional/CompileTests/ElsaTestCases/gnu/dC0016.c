// from the kernel
int __const_udelay_Reae3dfd6(int);
int __udelay_R9e7d6bd0(int);
int __bad_udelay();
int main() {
  (
   (__builtin_constant_p(1) && (1)<=5)
   ? (__builtin_constant_p((1)*1000)
      ? (((1)*1000) > 20000
         ? __bad_udelay()
         : __const_udelay_Reae3dfd6(((1)*1000) * 0x10c6ul))
      : __udelay_R9e7d6bd0((1)*1000))
   : ({
     unsigned long msec=(1);
     while (msec--)
       (__builtin_constant_p(1000)
        ? ((1000) > 20000
           ? __bad_udelay()
           : __const_udelay_Reae3dfd6((1000) * 0x10c6ul))
        : __udelay_R9e7d6bd0(1000));
   })
   );
}
