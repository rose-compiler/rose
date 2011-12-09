// This is part of a bug report that triggered an assertion for a remaining 
// original expression tree.  Chains of original expression trees were not
// previously handled properly.

// This will be unparsed as: "enum fmt {V=768};"
// Which is not what we really want since the original expression tree 
// should ahve been used, but it is not available in the AST.  So this
// should be fixed so that we can better support source-to-source.
enum fmt { V = 0x100 | 0x200 };

void foo(enum fmt f)
   {
     switch (f)
        {
          case V:
             break;
        }
   }

