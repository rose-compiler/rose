// ? matches

// fn_prototype_1 is a forward declaration with definition=0
// fn_prototype_2 is a forward declaration with definition=0
// fn_declaration_1 is NOT a forward declaration with definition=0x2aea54871010
// fn_prototype_2 is NOT a forward declaration with definition=0x2aea54871138
// main is NOT a forward declaration with definition=0x2aea54871260

void fn_prototype_1();
void fn_prototype_2();

void fn_declaration_1() {} // Can't distinguish this

void fn_prototype_2() {}

int main()
{
  return 0;
}
