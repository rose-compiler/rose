#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

static char *month[] = {"", "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                            "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

static FILE *output;

static void print_header(void)
{
    fprintf(output, "\f\n"
            "%-10s%-10s%-10s%-12s%-12s%-12s%-12s\n"
            "%-10s%-10s%-10s%-12s%-12s%-12s%-12s\n\n",
            "MONTH","MONTH INT","ACCUM INT","  YEAR INT",
            "MONTH PRINC","REM. PRINC","YEAR PRINC",
            "=====","===== ===","===== ===","  ==== ===",
            "===== =====","==== =====","==== =====");
}

int main(int argc, char *argv[])
{
    char table,
         ballooning = 0;
 
    char temp_str[256],
         *output_file;

    double p,
           ep = 0,
           yr,
           r,
           n,
           balloon,
           power,
           remainder,
           amount,
           monthly_interest,
           monthly_principal,
           previous_interest_accumulation,
           previous_principal_accumulation,
           yia,
           ypa,
           ai,
           ap;

    int i,
        l,
        m,
        fm,
        year_no,
        page_count = 0;

    if (argc > 1 && argv[1][0] != '?')
        output_file = argv[1];
    else
    {
        temp_str[0] = '\0';
        output_file = temp_str;

        fprintf(stderr,
                "This program will calculate the monthly payment that\n"
                "a borrower has to make on a mortgage, and generate\n"
                "an amortization table.  It takes as input, the\n"
                "mortgage amount, the interest rate, and the length of\n"
                "the loan.  It may be invoked as follows: \n"
                "\n"
                "%35s\n%35s\n%35s\n"
                "\n"
                "Using the first format, the user can specify the\n"
                "name of the output file. When the second\n"
                "format is used (with or without the question mark),\n"
                "this message will appear on the screen, and standard\n"
                "output is assumed.\n\n",
                "MORTGAGE filename",
                "or         ",
                "MORTGAGE [?]     ");
        fflush(stderr);
    }

    output = (output_file[0] == '\0' ? stdout : fopen(output_file, "w"));
    if (output == NULL)
    {
        fprintf(stderr,
                "Unable to open output file. Assuming standard output.\n");
        output_file[0] = '\0';
        output = stdout;    
    }

    fprintf(stderr, "Enter mortgage amount: "); fflush(stderr);
    scanf("%lf", &p);

    fprintf(stderr, "Enter interest rate: "); fflush(stderr);
    scanf("%lf", &yr);

    fprintf(stderr, "Enter number of years: "); fflush(stderr);
    scanf("%lf", &n);
    n *= 12.0;
    balloon = n;

    fprintf(stderr, "Do you want the amortization table: "); fflush(stderr);
    scanf("%s", temp_str);
    {
        char *p;
        for (p = temp_str; *p == ' '; p++)
            ;
        table = (*p == 'y' || *p == 'Y');

        if (table)
        {
            fprintf(stderr, "First month of payment(1 or 2 digits): ");
            fflush(stderr);
            scanf("%d", &fm);
            while (fm < 1 || fm > 12)
            {
                fprintf(stderr, "\aInvalid value for month, try again [1..12]: ");
                fflush(stderr);
                scanf("%d", &fm);
            }

            fprintf(stderr, "First year of payment(4 digits): "); fflush(stderr);
            scanf("%d", &year_no);
        }
    }

    fprintf(stderr, "Do you want to balloon? "); fflush(stderr);
    scanf("%s", temp_str);
    {
        char *p;
        for (p = temp_str; *p == ' '; p++)
            ;
        if (*p == 'y' || *p == 'Y')
        {
            fprintf(stderr, "After how many years? "); fflush(stderr);
            scanf("%lf", &balloon);
            balloon *= 12.0;
            balloon = (balloon > n ? n : balloon);
        }
    }
    fprintf(stderr, "\n\n");

    r = yr / 1200.0;
    power = exp(n * log(1.0 + r)); /* same as: (1.0 + r) ** n */
    amount = p * ((r * power) / (power - 1.0));

    fprintf(output, "\n");
    fprintf(output, "Total amount of loan: $%.2lf\n", p);
    fprintf(output, "Yearly interest rate: %.4lf%%\n", yr);
    fprintf(output, "Length of loan: %.2lf Year(s)\n", n / 12);
    fprintf(output, "Fixed monthly amount: $%.2lf\n", amount);
    if (table)
        fprintf(output, "First month of payment: %s %d\n",month[fm], year_no);
    fprintf(output, "Interest is compounded monthly !\n");

    if (output_file[0] != '\0')
    {
        fprintf(stderr, "\n\n");
        fprintf(stderr, "Total amount of loan: $%.2lf\n", p);
        fprintf(stderr, "Yearly interest rate: %.4lf%%\n", yr);
        fprintf(stderr, "Length of loan: %.2lf Year(s)\n", n / 12);
        fprintf(stderr, "Fixed monthly amount: $%.2lf\n", amount);
        if (table)
            fprintf(stderr, "First month of payment: %s %4d\n",
                            month[fm], year_no);
        fprintf(stderr, "Interest is compounded monthly !\n");
    }
    fprintf(stderr, "\n\n");
    fflush(stderr);

    ballooning = (balloon < n);
    
    if (table || ballooning)
    {
        fprintf(stderr, "Extra monthly payment: "); fflush(stderr);
        scanf("%lf", &ep);
        if (output_file[0] != '\0')
        {
            fprintf(output, "Extra monthly payment: $%.2lf\n", ep);
            fprintf(output, "Total monthly payment: $%.2lf\n", amount + ep);
        }

        print_header();

        remainder = p;
        ai = 0.0; /* accumulated interest  */
        ap = 0.0; /* accumulated principal */
        previous_interest_accumulation = 0.0;
        previous_principal_accumulation = 0.0;
        m = fm;
        for (i = 1; i <= ceil(balloon) && remainder > 0.0; i++)
        {
            monthly_interest = remainder * r;
            monthly_principal = (amount - monthly_interest + ep);
            if (monthly_principal >= remainder)
                monthly_principal = remainder;
            ai += monthly_interest;
            ap += monthly_principal;
            remainder -= monthly_principal;
            if (table)
            {
                fprintf(output, "%3s %4d  %9.2lf %9.2lf              %10.2lf"
                                "  %9.2lf",
                                month[m], year_no, monthly_interest,
                                ai, monthly_principal, remainder);
                if (m == 12) /* December? calculate year-end stuff. */
                {
                    yia = ai - previous_interest_accumulation;
                    ypa = ap - previous_principal_accumulation;
                    previous_interest_accumulation = ai;
                    previous_principal_accumulation = ap;
                    for (l = 0; l < 1 /* increase this number to make this line darker */ ; l++)
                    {
                        fprintf(output, "\r"
                                "%3s %4d  %9.2lf %9.2lf %10.2lf   %10.2lf"
                                "  %9.2lf  %9.2lf",
                                month[m], year_no, monthly_interest,
                                ai, yia, monthly_principal,
                                remainder, ypa);
                    }
                    fprintf(output, "\n");
                    // if (++page_count > 55 && i != (int) balloon)
                    // {
                    //     print_header();
                    //     page_count = 0;
                    // }
                    year_no = year_no + 1;
                    m = 1;
                }
                else
                    m++;
                if (++page_count > 55 && i != (int) balloon)
                {
                     print_header();
                     page_count = 0;
                }
                if (i >= balloon || remainder > 0.0)
                    fprintf(output, "\n");
            }
        }

        if (m != 1) /* not terminated exactly on december. */
        {
            yia = ai - previous_interest_accumulation;
            ypa = ap - previous_principal_accumulation;
            m = m - 1;
            for (l = 1; l <= 3; l++)
                fprintf(output, "\r"
                                "%3s %4d  %9.2lf %9.2lf %10.2lf   %10.2lf"
                                "  %9.2lf  %9.2lf",
                                month[m], year_no, monthly_interest,
                                ai, yia, monthly_principal,
                                remainder, ypa);
            fprintf(output, "\n");
        }

        if (balloon != n)
        {
            fprintf(output,
                    "\n\nAfter %5d month(s), you have to pay $%.2lf\n",
                    (int) balloon, remainder);
        }
    }

    fclose(output);
    return 0;
}
