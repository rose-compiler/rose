typedef int uint32_t;

struct xen_foreign_memory_map
   {
     uint32_t domid;
     struct 
        {
          uint32_t nr_entries;
        } map;
   };
    
int foobar()
{
    int rc;
    uint32_t nr_entries;
    uint32_t domid;

    struct xen_foreign_memory_map fmap = {
        .domid = domid,
        .map = { .nr_entries = nr_entries }
    };

    return rc;
}


