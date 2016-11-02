/*
The AST generated for the two string arguments to memcpy: snippet and story, is a cast expression with two subtrees instead of one.
Note that if the declarations of story and snippet are moved inside the main method then the correct AST is generated. In both cases,
the correct AST is generated for the string arguments to strcpy.

Can you tell me what causes this problem and how to test for for this condition?

--thanks,
--philippe
*/

// Added memcpy function prototype
typedef unsigned long size_t;
void *memcpy(void *dest, const void *src, size_t n);
char *strcpy(char *dest, const char *src);

char story[20],snippet[10];

int
main(int argc, char *argv[10])
   {
     strcpy(story, "Once upon a time...");
     memcpy(snippet, story, 5);
   }
