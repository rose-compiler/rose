/* 
NameQuery::querySubTree has problems parsing a function declaration with a
variable number of arguments.  In particular, printf.  But also:

int find_the_sum(int count, ...)
{
  return 0;
}

The call I'm attempting is:

list<string> testList = NameQuery::querySubTree(funcDec,
        NameQuery::ArgumentNames);

For the time being I can avoid; I'll remove such offending functions from
my example.

The problem in querySubTree occurs at line 79 of nameQuery.c:

          string sageArgument (elementNode.get_name ().str ());

on the 2nd iteration of the loop (2nd arg "...", as you might expect)
corresponding to the above function find_the_sum.  The problem is that
elementNode.get_name().str() is NULL.

By the way, there are 2 elements in the list returned from querySubTree.
The first corresponds to 'count' and the 2nd to '...'. As expected I
suppose; somehow that 2nd element should be treated differently to
indicate it is a vararg.

*/

int main()
   {
     return 0;
   }
