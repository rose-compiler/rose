namespace Y
   {
   }

void foobar()
   {
  // In order to get the name space global qualification of Y correct, we need to have the namespace alias generate a SgNamespaceSymbo.
     namespace Y = ::Y;
   }

