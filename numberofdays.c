#include <stdio.h>

int main()
{
    int month, year, days;
    int first_weekday = 0;

    printf("Enter month(1-12) : ");
    scanf("%d", &month);

    printf("Enter year : ");
    scanf("%d", &year);

    if (month < 1 || month > 12 || year < 1)
    {
        printf("Invalid month of the year");
        return 1;
    }
    if (month == 2)
    {
        if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
        {
            days = 29;
        }
        else
        {
            days = 28;
        }
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        days = 30;
    }
    else
    {
        days = 31;
    }
    printf("Month is %d\nYear is %d\n", month, year);
    printf("Name of the days %d\n ", days);
    return 0;
}