struct bootmem_region 
   {
     unsigned long s;
   } * bootmem_region_list;

void foobar()
   {
     bootmem_region_list[42] = (struct bootmem_region) { 7 };
   }
