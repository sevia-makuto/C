#include <stdio.h>

int isleapYear(int year)
{
    return ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0));
}

int daysInMonth(int month, int year)
{
    if (month == 2)
    {
        return isleapYear(year) ? 29 : 28;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        return 30;
    }
    else
    {
        return 31;
    }
}

int firstWeekDay(int year)
{
    int n = year - 1;
    int i = n + n / 4 - n / 100 + n / 400 + 1;
    return i % 7;
}

int firstDayOfMonth(int month, int year)
{
    int first = firstWeekDay(year);

    for (int m = 1; m < month; m++)
    {
        first += daysInMonth(m, year);
    }

    return first % 7;
}

int main(void)
{
    int year, month;

    printf("Enter a month (1-12) : ");
    if (scanf("%d", &month) != 1)
    {
        printf("Invalid input .\n ");
        return 1;
    }

    printf("Enter a year : ");
    if (scanf("%d", &year) != 1)
    {
        printf("Invalid input .\n ");
        return 1;
    }

    if (month < 1 || month > 12 || year < 1)
    {
        printf("Invalid month or year \n");
        return 1;
    }

    const char *months[] = {
        "January", "February", "March", "April",
        "May", "June", "July", "August",
        "September", "October", "November", "December"};

    int days = daysInMonth(month, year);
    int start = firstDayOfMonth(month, year);

    printf("             %s %d\n", months[month - 1], year);
    printf("SUN\t MON\t TUE\t WED\t THUR\t FRI\t SAT\n");

    for (int i = 0; i < start; i++)
    {
        printf("\t");
    }

    for (int day = 1; day <= days; day++)
    {
        printf("%3d\t", day);

        if ((start + day) % 7 == 0)
        {
            printf("\n");
        }
    }

    printf("\n");
    return 0;
}