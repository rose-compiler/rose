extern unsigned long long RamSizeOver4G;

void foobar()
   {
     unsigned long long align_mem; // = pci_region_align(&r64_mem);
     unsigned long long x = (((0x100000000LL + RamSizeOver4G)+((typeof(0x100000000LL + RamSizeOver4G))(align_mem)-1))&~((typeof(0x100000000LL + RamSizeOver4G))(align_mem)-1));
   }
