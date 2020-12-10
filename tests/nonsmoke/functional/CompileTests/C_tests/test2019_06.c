// This is a copy of test1014_57.c

typedef struct
   {
	union 
	{
		struct 
		{
                   int NameIsString;
		};
	};
   } WapiImageResourceDirectoryEntry;

#if 1
void scan_resource_dir ()
   {
     WapiImageResourceDirectoryEntry swapped_entry;
     int is_string;

     is_string = swapped_entry.NameIsString;
   }
#endif

