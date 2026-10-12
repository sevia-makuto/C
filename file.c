#include <stdio.h>

int main()
{
    FILE *fptr;
    // fptr = fopen("file.txt", "w");
    // fprintf(fptr, "hello prof");

    fptr = fopen("file.txt", "r");
    char myString[500];
    fgets(myString, 500, fptr);
    printf("%s", myString);

    fclose(fptr);
    return 0;
}