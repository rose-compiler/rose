// To initialize only the third and fourth members of the temp_address 
// variable, you could use a designated initializer list, as follows:
struct address {
                 int street_no;
                 char *street_name;
                 char *city;
                 char *prov;
                 char *postal_code;
               };
struct address temp_address =
               { .city = "Hamilton", .prov = "Ontario" };

