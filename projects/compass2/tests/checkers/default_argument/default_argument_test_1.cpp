// 4 matches

void fn_1(int x);

// function prototype
void fn_2(int x=3); // match

// redefined function prototype
void fn_3(int x);
void fn_3(int x=6); // match

// function prototype
void fn_4(int x=6); // match
void fn_4(int x);

// function definition
void fn_5(int x);
void fn_5(int x=9) {} // match

int main()
{
}
