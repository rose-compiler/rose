// a partially initialized structure
struct address {
                 int street_no;
                 char *street_name;
                 char *city;
                 char *prov;
                 char *postal_code;
               };
struct address temp_address =
               { 44, "Knyvet Ave.", "Hamilton", "Ontario" };

