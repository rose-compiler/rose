// this used to cause a failure due to ambiguous syntax and no merge
// action for the Condition nonterminal in the grammar: "WARNING:
// there is no action to merge nonterm Condition"
struct A{};
int main() {
  if (A *a = 0) {
  }
}
