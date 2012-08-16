// 2 matches

// (1) No forward declaration
void fn_declaration_1() {}

// Only forward declaration; no definition
void fn_prototype_1();

// Function definition + declaration
void fn_prototype_2();
void fn_prototype_2() {}

// (2) No forward declaration
int main()
{
  return 0;
}
