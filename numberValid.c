
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int main(void)
{
    char input[100];
    long number;
    int valid;

    while (1)
    {
        valid = 1;

        printf("Enter a positive whole number: ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("Input error.\n");
            return 1;
        }

        // Check that the complete input line was read
        if (strchr(input, '\n') == NULL && !feof(stdin))
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                // Discard remaining characters
            }

            valid = 0;
        }

        // Remove newline if present
        input[strcspn(input, "\n")] = '\0';

        // Reject empty input and non-digit characters
        if (input[0] == '\0')
        {
            valid = 0;
        }

        for (int i = 0; input[i] != '\0'; i++)
        {
            if (!isdigit((unsigned char)input[i]))
            {
                valid = 0;
                break;
            }
        }

        if (valid)
        {
            char *end;
            errno = 0;
            number = strtol(input, &end, 10);

            if (errno == ERANGE || *end != '\0' ||
                number < 1 || number > INT_MAX)
            {
                valid = 0;
            }
        }

        if (valid)
        {
            printf("Valid number: %ld\n", number);
            break;
        }
        else
        {
            printf("Invalid input! Enter a positive whole number.\n");
            printf("Please try again.\n\n");
        }
    }

    return 0;
}
