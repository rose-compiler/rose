
static struct bootmem_region 
   {
     unsigned long s, e; /* MFNs @s through @e-1 inclusive are free */
// } *__initdata bootmem_region_list;
   } * bootmem_region_list;

void foobar()
   {
     unsigned long s;
     unsigned long e;

     int i;

     bootmem_region_list[i] = (struct bootmem_region) { s, e };
   }
