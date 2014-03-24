#define MY_VALUE 42

// int abc;

#ifdef MY_VALUE
   int variable_we_expect_to_see;
#else
   int variable_that_would_be_hidden_without_token_unparsing;
#endif

// int xyz;


void foo()
   {
     int x = MY_VALUE;
   }

int main()
   {
   }

int x = MY_VALUE;
int y = 42;
