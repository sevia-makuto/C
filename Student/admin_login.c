
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include <libpq-fe.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>

#define PASSWORD_MAX 128
#define SALT_SIZE 16
#define HASH_SIZE 32
#define HASH_TEXT_SIZE 65

int read_password(const char *prompt, char *password, size_t size)
{
    struct termios old_settings, new_settings;

    printf("%s", prompt);
    fflush(stdout);

    if (tcgetattr(STDIN_FILENO, &old_settings) != 0)
        return 0;

    new_settings = old_settings;
    new_settings.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_settings) != 0)
        return 0;

    char *result = fgets(password, size, stdin);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_settings);
    printf("\n");

    if (result == NULL)
        return 0;

    password[strcspn(password, "\n")] = '\0';
    return 1;
}

int hex_to_bytes(const char *hex, unsigned char *output, size_t output_size)
{
    if (strlen(hex) != output_size * 2)
        return 0;

    for (size_t i = 0; i < output_size; i++)
    {
        unsigned int value;

        if (sscanf(hex + i * 2, "%2x", &value) != 1)
            return 0;

        output[i] = (unsigned char)value;
    }

    return 1;
}

void to_hex(const unsigned char *input, size_t length, char *output)
{
    static const char digits[] = "0123456789abcdef";

    for (size_t i = 0; i < length; i++)
    {
        output[i * 2] = digits[input[i] >> 4];
        output[i * 2 + 1] = digits[input[i] & 0x0f];
    }

    output[length * 2] = '\0';
}

int verify_password(const char *password, const char *stored_hash)
{
    char hash_copy[160];

    if (strlen(stored_hash) >= sizeof(hash_copy))
        return 0;

    strcpy(hash_copy, stored_hash);

    char *saveptr;
    char *algorithm = strtok_r(hash_copy, "$", &saveptr);
    char *iterations_text = strtok_r(NULL, "$", &saveptr);
    char *salt_text = strtok_r(NULL, "$", &saveptr);
    char *expected_text = strtok_r(NULL, "$", &saveptr);

    if (!algorithm || !iterations_text || !salt_text || !expected_text)
        return 0;

    if (strcmp(algorithm, "pbkdf2_sha256") != 0)
        return 0;

    char *endptr;
    long iterations = strtol(iterations_text, &endptr, 10);

    if (*iterations_text == '\0' || *endptr != '\0' ||
        iterations < 100000 || iterations > 2000000)
        return 0;

    unsigned char salt[SALT_SIZE];
    unsigned char expected[HASH_SIZE];
    unsigned char actual[HASH_SIZE];

    if (!hex_to_bytes(salt_text, salt, sizeof(salt)) ||
        !hex_to_bytes(expected_text, expected, sizeof(expected)))
        return 0;

    int ok = PKCS5_PBKDF2_HMAC(
        password,
        (int)strlen(password),
        salt,
        sizeof(salt),
        (int)iterations,
        EVP_sha256(),
        sizeof(actual),
        actual
    ) == 1;

    int match = ok &&
        CRYPTO_memcmp(actual, expected, sizeof(expected)) == 0;

    OPENSSL_cleanse(actual, sizeof(actual));
    OPENSSL_cleanse(expected, sizeof(expected));
    OPENSSL_cleanse(salt, sizeof(salt));
    OPENSSL_cleanse(hash_copy, sizeof(hash_copy));

    return match;
}

int main(void)
{
    char username[51];
    char password[PASSWORD_MAX];

    printf("=== Student Management System Login ===\n");
    printf("Username: ");

    if (fgets(username, sizeof(username), stdin) == NULL)
        return 1;

    if (strchr(username, '\n') == NULL && !feof(stdin))
    {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        printf("Username is too long.\n");
        return 1;
    }

    username[strcspn(username, "\n")] = '\0';

    if (username[0] == '\0')
    {
        printf("Username cannot be empty.\n");
        return 1;
    }

    if (!read_password("Password: ", password, sizeof(password)))
    {
        printf("Could not read password.\n");
        return 1;
    }

    PGconn *conn = PQconnectdb(
        "host=localhost "
        "port=5432 "
        "dbname=student_management "
        "user=makuto"
    );

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Database connection failed: %s",
                PQerrorMessage(conn));
        PQfinish(conn);
        OPENSSL_cleanse(password, sizeof(password));
        return 1;
    }

    const char *params[] = {username};

    PGresult *result = PQexecParams(
        conn,
        "SELECT password_hash FROM admins WHERE username = $1",
        1,
        NULL,
        params,
        NULL,
        NULL,
        0
    );

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Login query failed: %s",
                PQresultErrorMessage(result));
        PQclear(result);
        PQfinish(conn);
        OPENSSL_cleanse(password, sizeof(password));
        return 1;
    }

    int logged_in = 0;

    if (PQntuples(result) == 1)
    {
        const char *stored_hash = PQgetvalue(result, 0, 0);
        logged_in = verify_password(password, stored_hash);
    }

    OPENSSL_cleanse(password, sizeof(password));
    PQclear(result);
    PQfinish(conn);

    if (logged_in)
    {
        printf("\nLogin successful! Welcome, %s.\n", username);
        printf("You may now access the student management system.\n");
    }
    else
    {
        printf("\nInvalid username or password.\n");
        return 1;
    }

    return 0;
}
