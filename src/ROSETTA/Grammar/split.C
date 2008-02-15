vector<string> split (const string & s )
   {
  // Split function using strings for replacement of similar function using char*
  // Copied from Accelerated C++ Book (Makrus showed this to me)

     vector<string> ret;
     typedef string::size_type string_type;
     string_size i = 0;
  // invariant: we have processed characters[original value ofi,i)
     while (i != size())
        {
       // ignore blanks
       // invariant: characters in range [original i, current i] are all spaces
          while (i != s.size() && isspace(s[i]))
               ++i;
       // find end of next word
          string_size j = i;
       // invariant: none of the characters in range [original j, current j] is a space
          while (j != s.size() && !isspace(s[j]))
               ++j;

      // if we found some nonwhite space characters
         if (i != j)
            {
           // copy from s starting at i and taking j-1 chars
              ret.push_back(s.substr(i,j-1));
              i = j;
            }
        }

     return ret;      
   }
