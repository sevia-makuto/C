#include <stdio.h>
#include <ctype.h>

int main()
{
    char ch;
    printf("Enter a character: ");
    scanf("%c", &ch);

    if (isalpha((unsigned char)ch))
    {
        printf("%c is an alphabet.\n", ch);
    }
    else if (isdigit((unsigned char)ch))
    {
        printf("%c is a digit.\n", ch);
    }
    else if (isspace((unsigned char)ch))
    {
        printf("%c is a whitespace character.\n", ch);
    }
    else
    {
        printf("%c is neither an alphabet nor a digit.\n", ch);
    }

    return 0;
}