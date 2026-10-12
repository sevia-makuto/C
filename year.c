
#include <stdio.h>

int main()
{
    int year;
    char inputYear[30];

    while (1)
    {
        printf("Enter a year to check if it is a leap year: ");

        if (fgets(inputYear, sizeof(inputYear), stdin) == NULL)
        {
            break;
        }

        if (sscanf(inputYear, "%d", &year) != 1)
        {
            printf("Invalid input. Try again.\n\n");
            continue;
        }

        if ((year % 400 == 0) ||
            (year % 4 == 0 && year % 100 != 0))
        {
            printf("The year %d is a leap year!\n", year);
            break;
        }
        else
        {
            printf("The year %d is NOT a leap year.\n\n", year);
        }
    }

    printf("Program ended.\n");

    return 0;
}
