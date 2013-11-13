// void rose_ATinit(int argc, char *argv[], rose_ATerm *bottomOfStack);
// rose_ATbool rose_ATmatch(ATerm t, const char *pattern, ...);

#if 0
// This is the ATERM library implementation of the ATerm type.
// It can't be implemented as a SgNode* in the ROSE version of the ATerm API, 
// I think we have to reproduce a simple data structures as in the ATerm library.
struct __ATerm
{
  header_type   header;
  union _ATerm *next;
};

typedef union _ATerm
{
  header_type     header;
  struct __ATerm  aterm;
  union _ATerm*   subaterm[1];
  MachineWord     word[1];
} *ATerm;
#endif

// This is a structure similar to the ATerm Library, but specific to ROSE.
// Note that we can't just use something as simple as SgNode* as a ATerm
// equivalent since we can't make it work the same (does not permit assignemnt
// "ls = b;" in function: "vector<ATerm> ATerm_Graph::getAtermList(ATerm ls);"
// of file: "termGraph_using_RoseAtermAPI.C".

struct __rose_ATerm
   {
  // I expect that we can hide a SgNode* pointer in this data structure (as required).
  // We might need a pointer to a STL container for when we have ATerms that will be 
  // vectors of SgNode*.

     header_type   header;
     union _rose_ATerm *next;
   };

typedef union _rose_ATerm
   {
     header_type     header;
     struct __rose_ATerm  aterm;
     union _rose_ATerm*   subaterm[1];
     MachineWord     word[1];
   } *rose_ATerm;


// void rose_ATinit(int argc, char *argv[], rose_ATerm *bottomOfStack);
void rose_ATinit(int argc, char *argv[], void *bottomOfStack);

// rose_ATbool rose_ATmatch(rose_ATerm t, const char *pattern, ...);
// rose_ATbool rose_ATmatch(ATerm t, const char *pattern, ...);
bool rose_ATmatch(ATerm t, const char *pattern, ...);
