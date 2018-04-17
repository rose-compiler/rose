// Support for this unparsing uses the tests for in:
//     unparseLanguageIndependentConstructs.C
//     unparseCxx_expressions.C
// and 
//     unparseCxx_statements.C
//
// Modifications are associated with handling of "__anonymous_0x"
// strings and dated (1/22/2014).

// DQ (1/22/2014): Look ahead to see if this is a SgVarRefExp of a variable with a generated name.
// We can't support this approach.  We have to look from the SgDotExp down to see if their is a 
// variable reference to a variables named "__anonymous_0x" so that we can know to not output the
// SgDotExp operator name, and then always in the SgVarRef supress the name when we detect the
// "__anonymous_0x" named variables.

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

void scan_resource_dir ()
   {
     WapiImageResourceDirectoryEntry swapped_entry;
     int is_string;

     is_string = swapped_entry.NameIsString;
   }

