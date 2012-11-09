
enum def_tuple
   {
     never,
     all
   };

struct sudo_defs_types 
   {
     union
        {
          enum def_tuple tuple;
        } sd_un;
   };

extern struct sudo_defs_types sudo_defs_table[];

int
XXX_sudo_file_lookup(int pwflag)
   {
     if (pwflag)
        {
          enum def_tuple pwcheck;

       // References to both "never" AND "sudo_defs_table[pwflag].sd_un.tuple" are important.
          pwcheck = pwflag ? never : sudo_defs_table[pwflag].sd_un.tuple;
        }
   }


