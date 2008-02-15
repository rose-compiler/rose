

AstNodePtr AstInterface::GetFunctionDefinition( const AstNodePtr &n, std::string* name)
{
  AstNodePtr r=n;
  while (r != AST_NULL && !IsFunctionDefinition(r, name)) {
     r = GetParent(r);
  }
  return r;
}

