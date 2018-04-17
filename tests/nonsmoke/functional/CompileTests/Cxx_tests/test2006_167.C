// Example code from Thomas demonstrating a typedef without a name (but it appears that it 
// is given a name (the name of the struct).

// Normal struct
struct testStructtype
{
  int testStruct1;
} ;

// This is a typedef without a name (but it is not an error!)
// I wonder what the name is in this typedef?
typedef struct structname1
{
  int testStructtypedef1;
  int testStructtypedef2;
};

// This is a more common form of a typedef (with a name)
typedef struct structname2
{
  int testStructtypedef3;
} struct2type;

// normal typedef (but the struct is un-named)
typedef struct
{
  int structnoname1;
} structnonametype;


// A simple union
union testUnion
{
   int testUnion1;
}; 
