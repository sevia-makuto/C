
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <libpq-fe.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>

#define USERNAME_SIZE 51
#define PASSWORD_SIZE 256
#define ADMISSION_SIZE 31
#define NAME_SIZE 101
#define COURSE_SIZE 101

/* ---------- INPUT FUNCTIONS ---------- */

int read_line(const char *prompt, char *buffer, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, size, stdin) == NULL)
        return 0;

    char *newline = strchr(buffer, '\n');

    if (newline != NULL)
    {
        *newline = '\0';
        return 1;
    }

    int ch = getchar();

    if (ch != '\n' && ch != EOF)
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }

        printf("Input is too long. Try again.\n");
        return 0;
    }

    return 1;
}

int read_integer(const char *prompt, int minimum,
                 int maximum, int *value)
{
    char buffer[100];
    char *end;

    if (!read_line(prompt, buffer, sizeof(buffer)))
        return 0;

    errno = 0;
    long number = strtol(buffer, &end, 10);

    if (buffer[0] == '\0' || end == buffer ||
        *end != '\0' || errno == ERANGE ||
        number < minimum || number > maximum)
    {
        printf("Enter a whole number between %d and %d.\n",
               minimum, maximum);
        return 0;
    }

    *value = (int)number;
    return 1;
}

int read_points(double *points)
{
    char buffer[100];
    char *end;

    if (!read_line("Entry points (0-100): ",
                   buffer, sizeof(buffer)))
        return 0;

    errno = 0;
    double number = strtod(buffer, &end);

    if (buffer[0] == '\0' || end == buffer ||
        *end != '\0' || errno == ERANGE ||
        number < 0.0 || number > 100.0)
    {
        printf("Enter a valid number between 0 and 100.\n");
        return 0;
    }

    *points = number;
    return 1;
}

int read_password(char *password, size_t size)
{
    struct termios old_settings;
    struct termios new_settings;

    if (tcgetattr(STDIN_FILENO, &old_settings) != 0)
    {
        printf("Cannot configure password input.\n");
        return 0;
    }

    new_settings = old_settings;
    new_settings.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH,
                  &new_settings) != 0)
    {
        printf("Cannot configure password input.\n");
        return 0;
    }

    int success = read_line("Password: ", password, size);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_settings);
    printf("\n");

    if (!success)
    {
        OPENSSL_cleanse(password, size);
        return 0;
    }

    return 1;
}

/* ---------- PASSWORD VERIFICATION ---------- */

int hex_to_bytes(const char *hex, unsigned char *output,
                 size_t capacity, size_t *output_length)
{
    size_t length = strlen(hex);

    if (length == 0 || length % 2 != 0 ||
        length / 2 > capacity)
        return 0;

    for (size_t i = 0; i < length; i += 2)
    {
        unsigned int byte;
        char pair[3] = {hex[i], hex[i + 1], '\0'};

        if (strspn(pair, "0123456789abcdefABCDEF") != 2 ||
            sscanf(pair, "%2x", &byte) != 1)
            return 0;

        output[i / 2] = (unsigned char)byte;
    }

    *output_length = length / 2;
    return 1;
}

int verify_password(const char *password, const char *stored_hash)
{
    char copy[512];
    unsigned char salt[64] = {0};
    unsigned char expected[64] = {0};
    unsigned char calculated[64] = {0};

    size_t salt_length = 0;
    size_t expected_length = 0;
    int authenticated = 0;

    if (strlen(stored_hash) >= sizeof(copy))
        return 0;

    strcpy(copy, stored_hash);

    char *saveptr = NULL;
    char *algorithm = strtok_r(copy, "$", &saveptr);
    char *iterations_text = strtok_r(NULL, "$", &saveptr);
    char *salt_text = strtok_r(NULL, "$", &saveptr);
    char *hash_text = strtok_r(NULL, "$", &saveptr);

    if (algorithm == NULL || iterations_text == NULL ||
        salt_text == NULL || hash_text == NULL ||
        strtok_r(NULL, "$", &saveptr) != NULL)
        goto cleanup;

    if (strcmp(algorithm, "pbkdf2_sha256") != 0)
        goto cleanup;

    char *end;
    errno = 0;
    long iterations = strtol(iterations_text, &end, 10);

    if (errno != 0 || end == iterations_text ||
        *end != '\0' || iterations < 1 ||
        iterations > 10000000)
        goto cleanup;

    if (!hex_to_bytes(salt_text, salt, sizeof(salt),
                      &salt_length) ||
        !hex_to_bytes(hash_text, expected, sizeof(expected),
                      &expected_length))
        goto cleanup;

    if (PKCS5_PBKDF2_HMAC(
            password, (int)strlen(password),
            salt, (int)salt_length,
            (int)iterations, EVP_sha256(),
            (int)expected_length, calculated) != 1)
        goto cleanup;

    authenticated =
        CRYPTO_memcmp(calculated, expected, expected_length) == 0;

cleanup:
    OPENSSL_cleanse(copy, sizeof(copy));
    OPENSSL_cleanse(salt, sizeof(salt));
    OPENSSL_cleanse(expected, sizeof(expected));
    OPENSSL_cleanse(calculated, sizeof(calculated));

    return authenticated;
}

/* ---------- DATABASE CONNECTION ---------- */

PGconn *connect_database(void)
{
    PGconn *conn = PQconnectdb(
        "host=localhost port=5432 "
        "dbname=student_management user=makuto");

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Database connection failed: %s\n",
                PQerrorMessage(conn));

        PQfinish(conn);
        return NULL;
    }

    printf("Connected to PostgreSQL successfully.\n");
    return conn;
}

/* ---------- ADMINISTRATOR LOGIN ---------- */

int admin_login(PGconn *conn, char *username_out,
                size_t username_size)
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];

    printf("\n=== Student Management System Login ===\n");

    if (!read_line("Username: ", username, sizeof(username)) ||
        username[0] == '\0')
    {
        printf("Invalid username.\n");
        return 0;
    }

    if (!read_password(password, sizeof(password)))
        return 0;

    const char *values[] = {username};

    PGresult *result = PQexecParams(
        conn,
        "SELECT password_hash FROM admins WHERE username = $1",
        1, NULL, values, NULL, NULL, 0);

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        printf("Login query failed: %s\n", PQresultErrorMessage(result));
        PQclear(result);
        OPENSSL_cleanse(password, sizeof(password));
        return 0;
    }

    int authenticated = 0;

    if (PQntuples(result) == 1)
    {
        authenticated = verify_password(
            password, PQgetvalue(result, 0, 0));
    }

    PQclear(result);
    OPENSSL_cleanse(password, sizeof(password));

    if (!authenticated)
    {
        printf("Invalid username or password.\n");
        return 0;
    }

    snprintf(username_out, username_size, "%s", username);

    printf("\nLogin successful! Welcome, %s.\n", username);
    return 1;
}

/* ---------- REGISTER STUDENT ---------- */

void register_student(PGconn *conn)
{
    char admission[ADMISSION_SIZE];
    char name[NAME_SIZE];
    char course[COURSE_SIZE];
    char age_text[12];
    char points_text[32];

    int age;
    double points;

    printf("\n=== Register Student ===\n");

    if (!read_line("Admission number: ", admission,
                   sizeof(admission)) ||
        admission[0] == '\0')
    {
        printf("Invalid admission number.\n");
        return;
    }

    if (!read_line("Student full name: ", name,
                   sizeof(name)) ||
        name[0] == '\0')
    {
        printf("Invalid student name.\n");
        return;
    }

    if (!read_line("Course: ", course,
                   sizeof(course)) ||
        course[0] == '\0')
    {
        printf("Invalid course.\n");
        return;
    }

    if (!read_integer("Age (16-100): ", 16, 100, &age))
        return;

    if (!read_points(&points))
        return;

    snprintf(age_text, sizeof(age_text), "%d", age);
    snprintf(points_text, sizeof(points_text), "%.2f", points);

    const char *values[] = {
        admission, name, course, age_text, points_text};

    PGresult *result = PQexecParams(
        conn,
        "INSERT INTO students "
        "(admission_number, name, course, age, entry_points) "
        "VALUES ($1, $2, $3, $4, $5) RETURNING id",
        5, NULL, values, NULL, NULL, 0);

    if (PQresultStatus(result) == PGRES_TUPLES_OK)
    {
        printf("\nStudent registered successfully!\n");
        printf("Student ID: %s\n", PQgetvalue(result, 0, 0));
    }
    else if (PQresultErrorField(result, PG_DIAG_SQLSTATE) != NULL &&
             strcmp(PQresultErrorField(
                        result, PG_DIAG_SQLSTATE),
                    "23505") == 0)
    {
        printf("Registration failed: admission number already exists.\n");
    }
    else
    {
        printf("Registration failed: %s\n", PQresultErrorMessage(result));
    }

    PQclear(result);
}

/* ---------- VIEW ALL STUDENTS ---------- */

void view_students(PGconn *conn)
{
    PGresult *result = PQexec(
        conn,
        "SELECT id, admission_number, name, course, age, entry_points "
        "FROM students ORDER BY id");

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        printf("Could not retrieve students: %s\n",
               PQresultErrorMessage(result));
        PQclear(result);
        return;
    }

    int rows = PQntuples(result);

    printf("\n%-5s %-16s %-24s %-23s %-5s %-10s\n",
           "ID", "ADMISSION", "NAME", "COURSE", "AGE", "POINTS");
    printf("--------------------------------------------------------------------------------------\n");

    for (int i = 0; i < rows; i++)
    {
        printf("%-5s %-16.16s %-24.24s %-23.23s %-5s %-10s\n",
               PQgetvalue(result, i, 0),
               PQgetvalue(result, i, 1),
               PQgetvalue(result, i, 2),
               PQgetvalue(result, i, 3),
               PQgetvalue(result, i, 4),
               PQgetvalue(result, i, 5));
    }

    printf("\nTotal students: %d\n", rows);
    PQclear(result);
}

/* ---------- SEARCH STUDENT ---------- */

void search_student(PGconn *conn)
{
    char admission[ADMISSION_SIZE];

    printf("\n=== Search Student ===\n");

    if (!read_line("Enter admission number: ",
                   admission, sizeof(admission)) ||
        admission[0] == '\0')
    {
        printf("Invalid admission number.\n");
        return;
    }

    const char *values[] = {admission};

    PGresult *result = PQexecParams(
        conn,
        "SELECT id, admission_number, name, course, age, entry_points "
        "FROM students WHERE admission_number = $1",
        1, NULL, values, NULL, NULL, 0);

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        printf("Search failed: %s\n", PQresultErrorMessage(result));
        PQclear(result);
        return;
    }

    if (PQntuples(result) == 0)
    {
        printf("No student found with admission number '%s'.\n",
               admission);
        PQclear(result);
        return;
    }

    printf("\nStudent found!\n");
    printf("ID:               %s\n", PQgetvalue(result, 0, 0));
    printf("Admission number: %s\n", PQgetvalue(result, 0, 1));
    printf("Full name:        %s\n", PQgetvalue(result, 0, 2));
    printf("Course:           %s\n", PQgetvalue(result, 0, 3));
    printf("Age:              %s\n", PQgetvalue(result, 0, 4));
    printf("Entry points:     %s\n", PQgetvalue(result, 0, 5));

    PQclear(result);
}

/* ---------- UPDATE STUDENT ---------- */

void update_student(PGconn *conn)
{
    char admission[ADMISSION_SIZE];
    char name[NAME_SIZE];
    char course[COURSE_SIZE];
    char age_text[12];
    char points_text[32];

    int age;
    double points;

    printf("\n=== Update Student Information ===\n");

    if (!read_line("Admission number of student to update: ",
                   admission, sizeof(admission)) ||
        admission[0] == '\0')
    {
        printf("Invalid admission number.\n");
        return;
    }

    /* First, check whether this student exists */
    const char *search_values[] = {admission};

    PGresult *check = PQexecParams(
        conn,
        "SELECT id, name, course, age, entry_points "
        "FROM students WHERE admission_number = $1",
        1, NULL, search_values, NULL, NULL, 0);

    if (PQresultStatus(check) != PGRES_TUPLES_OK)
    {
        printf("Database error: %s\n", PQresultErrorMessage(check));
        PQclear(check);
        return;
    }

    if (PQntuples(check) == 0)
    {
        printf("No student found with admission number '%s'.\n",
               admission);
        PQclear(check);
        return;
    }

    printf("\nCurrent student details:\n");
    printf("Name:         %s\n", PQgetvalue(check, 0, 1));
    printf("Course:       %s\n", PQgetvalue(check, 0, 2));
    printf("Age:          %s\n", PQgetvalue(check, 0, 3));
    printf("Entry points: %s\n", PQgetvalue(check, 0, 4));

    PQclear(check);

    printf("\nEnter the new student details.\n");

    if (!read_line("New full name: ", name, sizeof(name)) ||
        name[0] == '\0')
    {
        printf("Invalid student name. Update cancelled.\n");
        return;
    }

    if (!read_line("New course: ", course, sizeof(course)) ||
        course[0] == '\0')
    {
        printf("Invalid course. Update cancelled.\n");
        return;
    }

    if (!read_integer("New age (16-100): ", 16, 100, &age))
    {
        printf("Update cancelled.\n");
        return;
    }

    if (!read_points(&points))
    {
        printf("Update cancelled.\n");
        return;
    }

    snprintf(age_text, sizeof(age_text), "%d", age);
    snprintf(points_text, sizeof(points_text), "%.2f", points);

    const char *values[] = {
        name, course, age_text, points_text, admission};

    PGresult *result = PQexecParams(
        conn,
        "UPDATE students "
        "SET name = $1, course = $2, age = $3, entry_points = $4 "
        "WHERE admission_number = $5 "
        "RETURNING id",
        5, NULL, values, NULL, NULL, 0);

    if (PQresultStatus(result) == PGRES_TUPLES_OK &&
        PQntuples(result) == 1)
    {
        printf("\nStudent information updated successfully!\n");
        printf("Student ID: %s\n", PQgetvalue(result, 0, 0));
    }
    else
    {
        printf("Update failed: %s\n", PQresultErrorMessage(result));
    }

    PQclear(result);
}

/* ---------- MAIN PROGRAM ---------- */

int main(void)
{
    PGconn *conn = connect_database();

    if (conn == NULL)
        return EXIT_FAILURE;

    char username[USERNAME_SIZE];

    if (!admin_login(conn, username, sizeof(username)))
    {
        printf("Access denied. Exiting program.\n");
        PQfinish(conn);
        return EXIT_FAILURE;
    }

    int choice;
    int running = 1;

    while (running)
    {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Register student\n");
        printf("2. View all students\n");
        printf("3. Search for a student\n");
        printf("4. Update student information\n");
        printf("5. Logout and exit\n");
        printf("===============================\n");

        if (!read_integer("Choose an option: ", 1, 5, &choice))
            continue;

        switch (choice)
        {
        case 1:
            register_student(conn);
            break;

        case 2:
            view_students(conn);
            break;

        case 3:
            search_student(conn);
            break;

        case 4:
            update_student(conn);
            break;

        case 5:
            printf("Logging out. Goodbye, %s!\n", username);
            running = 0;
            break;
        }
    }

    PQfinish(conn);
    return EXIT_SUCCESS;
}
