#define ETH_ALEN 42

typedef unsigned char u_int8_t;

struct ether_addr
   {
     u_int8_t ether_addr_octet[ETH_ALEN];
   } __attribute__ ((__packed__));

void foo()
   {
  // This causes the message: 
  // --- NOTE: In set_alignment_attributes(a_variable_ptr,SgInitializedName*): Packing in set_alignment_attributes( a_variable_ptr variable, SgInitializedName* initializedName ): not implemented!
  // Not yet clear if this is something we have to fix.
     struct ether_addr x;
   }
