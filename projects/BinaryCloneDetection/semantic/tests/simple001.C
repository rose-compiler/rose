namespace NAMESPACE {

// Testing no argument returning constant value
static int global_numb_one = 1;

int
numb_one_inline()
{
    return 1;
}

int
numb_one_local_var()
{
    int var1 = 1;
    return var1;
}

int
numb_one_local_vars()
{
    int var1 = 1;
    int var2 = var1;
    return var2;
}

int
numb_one_static_var()
{
    return global_numb_one;
}

int
numb_one_arithmetic()
{
    return 3 - 2;
}

    

// Testing one argument returning argument
static int global_argument_numb_one = 1;

int argument_numb_one_inline(int var_1)
{
    return var_1;
}

int
argument_numb_one_local_var(int var_1)
{
    int loc_var_1 = var_1;
    return loc_var_1;
}

int
argument_numb_one_static_var(int var_1)
{
    global_argument_numb_one = var_1;
    return global_argument_numb_one;
};

// Testing adding two arguments
static int add_argument_static_var_1 = 1;
static int add_argument_static_var_2 = 1;

int
add_numbers_inline(int var_1, int var_2)
{
    return var_1 + var_2;
}

int
add_numbers_local_var(int var_1, int var_2)
{
    int loc_var_1 = var_1;
    int loc_var_2 = var_2;
    return loc_var_1+loc_var_2;
}

int
add_numbers_static_var(int var_1, int var_2)
{
    add_argument_static_var_1 = var_1;
    add_argument_static_var_2 = var_2;
    return add_argument_static_var_1 + add_argument_static_var_2;
}

} // namespace
