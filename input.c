#include <stdio.h>

int main()
{
    char name[30];
    int age;

    printf("Enter your full name : ");
    fgets(name, sizeof(name), stdin);
    printf("Enter your age : ");
    scanf("%d", &age);
    printf("Hello %s  you are  %d  years old ", name, age);
}