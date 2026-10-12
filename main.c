#include <stdio.h>

int main()
{
    int age = 23;
    char grade = 'A';
    float gpa = 4.7;
    double salary = 100000.00;

    printf("Age : %d\n", age);
    printf("Grade : %c\n", grade);
    printf("GPA : %.2f\n", gpa);
    printf("Salary : %.2lf\n", salary);

    // type casting
    int students = 21;
    int Marks = 567;
    float average = (float)Marks / students;

    // sum
    int sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sum += i;
        printf("sum of %d is %d\n", i, sum);
    }

    // int i, j;
    // for (i = 1; i <= 100; i++)
    // {
    //     for (j = 1; j <= 100; j++)
    //     {
    //         printf("%d * %d = %d\n", i, j, i * j);
    //     }
    //     printf("\n");
    // }

    int age[5] = {12, 14, 16, 18, 20};
    for (int i = 0; i < 5; i++)
    {
        printf("Age of student %d is %d\n", i + 1, age[&i]);
    }

    // printf("Average : %.2f\n", average);
    // printf("am back to c in another mode ");
    return 0;
}