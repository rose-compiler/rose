/* The innermost scope (the if statement) in this toy function will be
 * reported by the DeepNestingChecker if maximumNestedScopes is set to 2. */
void matrix_abs(int n, int m, int **matrix)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (matrix[i][j] < 0)
            {
                matrix[i][j] = -matrix[i][j];
            }
        }
    }
}

/* The nesting in each function is not greater than 2; the if statement has
 * been pulled out into its own function. */
void abs_if_necessary(int *p)
{
    if (*p < 0)
    {
        *p = -*p;
    }
}

void matrix_abs2(int n, int m, int **matrix)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            abs_if_necessary(&matrix[i][j]);
        }
    }
}
