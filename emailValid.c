
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int isValidEmail(char email[])
{
    int atCount = 0;
    int atPosition = -1;
    int length = strlen(email);

    if (length == 0)
    {
        return 0;
    }

    // Check allowed characters and count @
    for (int i = 0; i < length; i++)
    {
        unsigned char ch = (unsigned char)email[i];

        if (ch == '@')
        {
            atCount++;
            atPosition = i;
        }
        else if (!isalnum(ch) &&
                 ch != '.' && ch != '_' &&
                 ch != '-' && ch != '+')
        {
            return 0;
        }
    }

    // Exactly one @ is required
    if (atCount != 1)
    {
        return 0;
    }

    // Username must not be empty
    if (atPosition == 0)
    {
        return 0;
    }

    // Domain must not be empty
    if (atPosition == length - 1)
    {
        return 0;
    }

    // Check for a dot after @
    int dotFound = 0;

    for (int i = atPosition + 1; i < length; i++)
    {
        if (email[i] == '.')
        {
            dotFound = 1;

            // Dot cannot end the email
            if (i == length - 1)
            {
                return 0;
            }

            // Reject consecutive dots
            if (email[i + 1] == '.')
            {
                return 0;
            }
        }
    }

    if (!dotFound)
    {
        return 0;
    }

    // Reject a dot immediately before @
    if (email[atPosition - 1] == '.')
    {
        return 0;
    }

    return 1;
}

int main(void)
{
    char email[150];

    while (1)
    {
        printf("Enter your email address: ");

        if (fgets(email, sizeof(email), stdin) == NULL)
        {
            return 1;
        }

        // Remove newline
        email[strcspn(email, "\n")] = '\0';

        if (isValidEmail(email))
        {
            printf("Valid email: %s\n", email);
            break;
        }
        else
        {
            printf("Invalid email! Please try again.\n\n");
        }
    }

    return 0;
}
