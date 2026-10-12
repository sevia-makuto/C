
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

typedef struct Student
{
    float entry_points;
    int admissionNumber;
    int age;
    char name[50];
    char course[50];

} Student;

// Read input safely, including text containing spaces
int readLine(const char *prompt, char input[], size_t size)
{
    while (1)
    {
        printf("%s", prompt);

        if (fgets(input, size, stdin) == NULL)
            return 0;

        if (strchr(input, '\n') == NULL)
        {
            int ch = getchar();

            if (ch != EOF)
            {
                while (ch != '\n' && ch != EOF)
                    ch = getchar();

                printf("Input too long. Please try again.\n");
                continue;
            }
        }

        input[strcspn(input, "\n")] = '\0';
        return 1;
    }
}

// Check whether a string contains digits only
int isDigitsOnly(const char input[])
{
    if (input[0] == '\0')
        return 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)input[i]))
            return 0;
    }

    return 1;
}

// Read a positive integer
int readPositiveInteger(const char *prompt, int *result)
{
    char input[100];

    while (1)
    {
        if (!readLine(prompt, input, sizeof(input)))
            return 0;

        if (!isDigitsOnly(input))
        {
            printf("Invalid input! Enter digits only.\n");
            continue;
        }

        errno = 0;
        char *end;
        long value = strtol(input, &end, 10);

        if (errno == ERANGE || *end != '\0' ||
            value < 1 || value > INT_MAX)
        {
            printf("Invalid number. Please try again.\n");
            continue;
        }

        *result = (int)value;
        return 1;
    }
}

// Validate names and courses
// Only letters and spaces are accepted
int isValidText(const char input[])
{
    int hasLetter = 0;

    if (input[0] == '\0')
        return 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        unsigned char ch = (unsigned char)input[i];

        if (isalpha(ch))
        {
            hasLetter = 1;
        }
        else if (ch != ' ')
        {
            return 0;
        }
    }

    return hasLetter;
}

// Read a valid name or course
int readValidText(const char *prompt,
                  char output[], size_t size)
{
    while (1)
    {
        if (!readLine(prompt, output, size))
            return 0;

        if (isValidText(output))
            return 1;

        printf("Invalid input! Use letters and spaces only.\n");
    }
}

// Read and validate entry points between 0 and 100
int readEntryPoints(float *result)
{
    char input[100];

    while (1)
    {
        if (!readLine("Enter entry points (0-100): ",
                      input, sizeof(input)))
            return 0;

        errno = 0;
        char *end;
        float value = strtof(input, &end);

        if (input[0] == '\0' || end == input ||
            *end != '\0' || errno == ERANGE ||
            !isfinite(value) || value < 0 || value > 100)
        {
            printf("Invalid entry points! Enter a number from 0 to 100.\n");
            continue;
        }

        *result = value;
        return 1;
    }
}

// Register a student
int registerStudent(Student *student)
{
    printf("\n===== REGISTER STUDENT =====\n");

    if (!readPositiveInteger("Enter admission number: ",
                             &student->admissionNumber))
        return 0;

    if (!readValidText("Enter student name: ",
                       student->name, sizeof(student->name)))
        return 0;

    if (!readValidText("Enter course: ",
                       student->course, sizeof(student->course)))
        return 0;

    while (1)
    {
        if (!readPositiveInteger("Enter age (16-100): ",
                                 &student->age))
            return 0;

        if (student->age >= 16 && student->age <= 100)
            break;

        printf("Invalid age! Age must be between 16 and 100.\n");
    }

    if (!readEntryPoints(&student->entry_points))
        return 0;

    printf("\nStudent registered successfully!\n");
    printf("Admission Number: %d\n", student->admissionNumber);
    printf("Name: %s\n", student->name);
    printf("Course: %s\n", student->course);
    printf("Age: %d\n", student->age);
    printf("Entry Points: %.2f\n", student->entry_points);

    return 1;
}

// Display a registered student
void displayStudent(const Student *student)
{
    printf("\n========== STUDENT DETAILS ==========\n");
    printf("Admission Number : %d\n", student->admissionNumber);
    printf("Name             : %s\n", student->name);
    printf("Course           : %s\n", student->course);
    printf("Age              : %d\n", student->age);
    printf("Entry Points     : %.2f\n", student->entry_points);
    printf("=====================================\n");
}

int main(void)
{
    Student student1;
    int registered = 0;
    int choice;
    char menuInput[100];

    while (1)
    {
        printf("\n");
        printf("=====================================\n");
        printf("       STUDENT MANAGEMENT SYSTEM\n");
        printf("=====================================\n");
        printf("1. Register Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Student Reports\n");
        printf("0. Exit\n");
        printf("=====================================\n");

        if (!readLine("Enter your choice: ",
                      menuInput, sizeof(menuInput)))
            return 0;

        if (!isDigitsOnly(menuInput))
        {
            printf("Invalid input! Enter a menu number.\n");
            continue;
        }

        char *end;
        long value = strtol(menuInput, &end, 10);

        if (*end != '\0' || value > 6)
        {
            printf("Invalid choice! Select from 0 to 6.\n");
            continue;
        }

        choice = (int)value;

        switch (choice)
        {
        case 1:
            if (registered)
            {
                printf("A student is already registered in this version.\n");
            }
            else if (registerStudent(&student1))
            {
                registered = 1;
            }
            break;

        case 2:
            if (registered)
                displayStudent(&student1);
            else
                printf("No students registered yet.\n");
            break;

        case 3:
            printf("Search will be implemented in a later lesson.\n");
            break;

        case 4:
            printf("Update will be implemented in a later lesson.\n");
            break;

        case 5:
            printf("Delete will be implemented in a later lesson.\n");
            break;

        case 6:
            printf("Reports will be implemented in a later lesson.\n");
            break;

        case 0:
            printf("Exiting Student Management System.\n");
            return 0;
        }
    }
}
