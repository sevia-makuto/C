
#include <stdio.h>

#include <string.h>

typedef struct Student
{
    float entry_points;
    int admissionNumber;
    char name[50];
    char course[50];

} Student;

int main(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("    STUDENT MANAGEMENT SYSTEM\n");
        printf("\n");

        printf("1. Register Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Student Reports\n");
        printf("0. Exit\n");
        printf("\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Enter a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                // Clear invalid input
            }

            continue;
        }

        switch (choice)
        {
        case 1:
            printf("Register Student selected.\n");
            break;

        case 2:
            printf("Display All Students selected.\n");
            break;

        case 3:
            printf("Search Student selected.\n");
            break;

        case 4:
            printf("Update Student selected.\n");
            break;

        case 5:
            printf("Delete Student selected.\n");
            break;

        case 6:
            printf("Student Reports selected.\n");
            break;

        case 0:
            printf("Exiting Student Management System.\n");
            return 0;

        default:
            printf("Invalid choice! Select from 0 to 6.\n");
        }
    }

    if (choice == 0)
    {
        printf("Exiting Student Management System.\n");
        return 0;
    }
    else if (choice == 1)
    {
        printf("Register Student selected.\n");
        }
    else if (choice == 2)
    {
        printf("Display All Students selected.\n");
    }
    else if (choice == 3)
    {
        printf("Search Student selected.\n");
    }
    else if (choice == 4)
    {
        printf("Update Student selected.\n");
    }
    else if (choice == 5)
    {
        printf("Delete Student selected.\n");
    }
    else if (choice == 6)
    {
        printf("Student Reports selected.\n");
    }
    else
    {
        printf("Invalid choice! Select from 0 to 6.\n");
    }

    return 0;
}
