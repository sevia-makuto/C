
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(void)
{
    char name[100];
    int valid;

    while (1)
    {
        valid = 1;

        printf("Enter your full name: ");

        if (fgets(name, sizeof(name), stdin) == NULL)
        {
            printf("Input error.\n");
            return 1;
        }

        // Remove the newline
        name[strcspn(name, "\n")] = '\0';

        // Check for an empty name
        if (name[0] == '\0')
        {
            valid = 0;
        }

        int hasLetter = 0;

        // Validate each character
        for (int i = 0; name[i] != '\0'; i++)
        {
            unsigned char ch = (unsigned char)name[i];

            if (isalpha(ch))
            {
                hasLetter = 1;
            }
            else if (ch != ' ')
            {
                valid = 0;
                break;
            }
        }

        // Reject names containing only spaces
        if (!hasLetter)
        {
            valid = 0;
        }

        if (valid)
        {
            printf("Name accepted: %s\n", name);
            break;
        }
        else
        {
            printf("Invalid name! Use letters and spaces only. Try again.\n\n");
        }
    }

    return 0;
}
