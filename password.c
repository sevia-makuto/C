
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int isValidPassword(char password[])
{
    int length = strlen(password);

    // Password length must not exceed 11
    if (length > 11 || length == 0)
    {
        return 0;
    }

    // Must start with an uppercase letter
    if (!isupper((unsigned char)password[0]))
    {
        return 0;
    }

    int hasDigit = 0;
    int hasSpecial = 0;

    for (int i = 0; password[i] != '\0'; i++)
    {
        unsigned char ch = (unsigned char)password[i];

        if (isdigit(ch))
        {
            hasDigit = 1;
        }
        else if (ch == '@' || ch == '#' ||
                 ch == '$' || ch == '&')
        {
            hasSpecial = 1;
        }
        else if (isalpha(ch) || ch == '.')
        {
            // Letters and optional dots are allowed
        }
        else
        {
            // Reject spaces and unsupported characters
            return 0;
        }
    }

    // Must contain at least one digit and special character
    if (hasDigit && hasSpecial)
    {
        return 1;
    }

    return 0;
}

int main(void)
{
    char password[100];

    while (1)
    {
        printf("Enter your password: ");

        if (fgets(password, sizeof(password), stdin) == NULL)
        {
            return 1;
        }

        // Remove newline
        password[strcspn(password, "\n")] = '\0';

        if (isValidPassword(password))
        {
            printf("Password accepted!\n");
            break;
        }
        else
        {
            printf("\nInvalid password!\n");
            printf("Requirements:\n");
            printf("- Must start with an uppercase letter\n");
            printf("- Must contain at least one digit\n");
            printf("- Must contain @, #, $ or &\n");
            printf("- Maximum length is 11 characters\n");
            printf("- Dots are optional; spaces are not allowed\n\n");
        }
    }

    return 0;
}
