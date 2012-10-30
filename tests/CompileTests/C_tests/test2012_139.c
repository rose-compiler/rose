
// Two functions provided to support selection between them (as function pointers)
// in the conditional evaluation done in the retrun statement.
extern int valid_ipv4_hostaddr(const char *, int);
extern int valid_ipv6_hostaddr(const char *, int);

const char *valid_mailhost_addr(const char *addr, int gripe)
   {
     const char *bare_addr = 0L;

  // This statement computed a value for "bare_addr" but is not relevant for testing ROSE to compile 
  // the return statement (which is where the bug in ROE is demonstrated).  Basicaly, I had not 
  // anticipated in the testing of the AST that the SgConditionalExp could be a valid basis for a
  // function reference in a function call expression.
  // bare_addr = ((strncasecmp((addr), "IPv6:", (sizeof("IPv6:") - 1)) == 0) ? (addr) + (sizeof("IPv6:") - 1) : (addr));
     return ((bare_addr != addr ? valid_ipv6_hostaddr : valid_ipv4_hostaddr) (bare_addr, gripe) ? bare_addr : 0);
   }
