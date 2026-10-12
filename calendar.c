#include <stdio.h>

int main()
{
    printf("MON\tTUE\tWED\tTHUR\tFRI\tSAT\tSUN\n");
    int date = 1 - 5;
    int d, w;
    for (w = 0; w <= 6; w++)
    {
        for (d = 0; d < 7; d++)
        {
            if (date <= 0 || date > 31)
                printf(" \t");
            else
                printf("%d\t", date);
            date++;
        }
        printf("\n");
    }

    return 0;
}