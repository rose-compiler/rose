#if 0
Hi Dan and Leo,

There seems to be a bug when unparsing an array as a parameter -
attached is a small piece of C code:

void parse_args (char **, char[81], char[81])

when parsed and then unparsed directly from the IR, we get this:

extern void parse_args(char **,char ()[81UL],char [81UL]);

The second parameter has an additional "()" which triggers a
compilation error. Note that the third parameter is correct. After
tweaking with parameters, I found out that this is caused when there
is a parameter which is a pointer of a pointer (**). When I remove the
first parameter,  this bug does not manifest.

Thanks,
Gabriel.

#endif

void parse_args (char **, char[81], char[81]);

void parse_args (char **argv, char inPath[81], char outPath[81])
   {
   }

