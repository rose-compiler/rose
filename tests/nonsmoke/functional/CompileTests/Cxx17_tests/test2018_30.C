// [[nodiscard]] attribute

struct [[nodiscard]] error_info { /*...*/ };
error_info enable_missile_safety_mode();
void launch_missiles();
void test_missiles() {
  enable_missile_safety_mode(); // warning encouraged launch_missiles();
}
error_info &foo();
void f() { foo(); } // reference type, warning not encouraged

