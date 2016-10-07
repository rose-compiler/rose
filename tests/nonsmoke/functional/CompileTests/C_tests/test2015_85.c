struct xen_foreign_memory_map
   {
     int domid;
     struct X
        {
          int nr_entries;
        } map;
   };

void foobar()
   {
     int nr_entries;
     int domid;

  // struct xen_foreign_memory_map fmap = { .domid = domid, .map = { .nr_entries = nr_entries } };
     struct xen_foreign_memory_map fmap = 
        {
          .domid = domid,
          .map = { .nr_entries = nr_entries }
        };
   }


