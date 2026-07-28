
#include <stdio.h>

int isleapYear(int year)
{
    return (year % 400 == 0) ||
           (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int month, int year)
{
    if (month == 2)
    {
        return isleapYear(year) ? 29 : 28;
    }

    if (month == 4 || month == 6 ||
        month == 9 || month == 11)
    {
        return 30;
    }

    return 31;
}

/* Returns the weekday of January 1.
   Sunday = 0, Monday = 1, ..., Saturday = 6. */
int firstWeekDay(int year)
{
    int n = year - 1;
    int i = n + n / 4 - n / 100 + n / 400 + 1;

    return i % 7;
}

// Returns the weekday of the first day of a month.
int firstDayOfMonth(int month, int year)
{
    int first = firstWeekDay(year);

    for (int m = 1; m < month; m++)
    {
        first += daysInMonth(m, year);
    }

    return first % 7;
}

// Displays one month's calendar.
void displayMonth(int month, int year)
{
    const char *months[] = {
        "January", "February", "March", "April",
        "May", "June", "July", "August",
        "September", "October", "November", "December"};

    int days = daysInMonth(month, year);
    int start = firstDayOfMonth(month, year);

    printf("\n\n          %s %d\n", months[month - 1], year);
    printf(" Sun Mon Tue Wed Thu Fri Sat\n");

    // Print spaces before the first day.
    for (int i = 0; i < start; i++)
    {
        printf("    ");
    }

    // Print dates from 1 to the last day.
    for (int day = 1; day <= days; day++)
    {
        printf("%4d", day);

        if ((start + day) % 7 == 0)
        {
            printf("\n");
        }
    }

    printf("\n");
}

int main(void)
{
    int year;

    printf("Enter year: ");

    if (scanf("%d", &year) != 1 || year < 1)
    {
        printf("Invalid year. Please enter a positive whole number.\n");
        return 1;
    }

    printf("\n");
    printf("             YEAR: %d\n", year);
    printf("\n");

    // Display January through December
    for (int month = 1; month <= 12; month++)
    {
        displayMonth(month, year);
    }

    return 0;
}
