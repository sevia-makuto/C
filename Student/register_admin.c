
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include <libpq-fe.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define SALT_SIZE 16
#define HASH_SIZE 32
#define ITERATIONS 600000
#define PASSWORD_MAX 128

/* Read a password without displaying it on screen. */
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

/* Convert bytes to hexadecimal text. */
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

/* Hash a password using PBKDF2-HMAC-SHA256. */
int hash_password(const char *password, char *stored_hash, size_t output_size)
{
    unsigned char salt[SALT_SIZE];
    unsigned char hash[HASH_SIZE];

    if (RAND_bytes(salt, sizeof(salt)) != 1)
        return 0;

    if (PKCS5_PBKDF2_HMAC(
            password,
            (int)strlen(password),
            salt,
            sizeof(salt),
            ITERATIONS,
            EVP_sha256(),
            sizeof(hash),
            hash) != 1)
        return 0;

    char salt_hex[SALT_SIZE * 2 + 1];
    char hash_hex[HASH_SIZE * 2 + 1];

    to_hex(salt, sizeof(salt), salt_hex);
    to_hex(hash, sizeof(hash), hash_hex);

    int written = snprintf(
        stored_hash,
        output_size,
        "pbkdf2_sha256$%d$%s$%s",
        ITERATIONS,
        salt_hex,
        hash_hex);

    OPENSSL_cleanse(hash, sizeof(hash));
    OPENSSL_cleanse(salt, sizeof(salt));

    return written > 0 && (size_t)written < output_size;
}

int main(void)
{
    char username[51];
    char password[PASSWORD_MAX];
    char confirmation[PASSWORD_MAX];
    char stored_hash[160];

    printf("=== First Administrator Registration ===\n");

    printf("Choose an administrator username: ");

    if (fgets(username, sizeof(username), stdin) == NULL)
        return 1;

    username[strcspn(username, "\n")] = '\0';

    if (strlen(username) == 0 ||
        strlen(username) > 50 ||
        strpbrk(username, " \t\r\n") != NULL)
    {
        fprintf(stderr, "Username must be 1-50 characters without spaces.\n");
        return 1;
    }

    if (!read_password("Choose a password (minimum 12 characters): ",
                       password, sizeof(password)))
    {
        fprintf(stderr, "Could not read password.\n");
        return 1;
    }

    if (strlen(password) < 12)
    {
        fprintf(stderr, "Password must contain at least 12 characters.\n");
        OPENSSL_cleanse(password, sizeof(password));
        return 1;
    }

    if (!read_password("Confirm password: ",
                       confirmation, sizeof(confirmation)))
    {
        fprintf(stderr, "Could not read confirmation.\n");
        OPENSSL_cleanse(password, sizeof(password));
        return 1;
    }

    if (strcmp(password, confirmation) != 0)
    {
        fprintf(stderr, "Passwords do not match.\n");
        OPENSSL_cleanse(password, sizeof(password));
        OPENSSL_cleanse(confirmation, sizeof(confirmation));
        return 1;
    }

    if (!hash_password(password, stored_hash, sizeof(stored_hash)))
    {
        fprintf(stderr, "Password hashing failed.\n");
        OPENSSL_cleanse(password, sizeof(password));
        OPENSSL_cleanse(confirmation, sizeof(confirmation));
        return 1;
    }

    OPENSSL_cleanse(password, sizeof(password));
    OPENSSL_cleanse(confirmation, sizeof(confirmation));

    PGconn *conn = PQconnectdb(
        "host=localhost "
        "port=5432 "
        "dbname=student_management "
        "user=makuto");

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Database connection failed: %s",
                PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    /*
       Insert only if no administrator exists.
       The unique username constraint prevents duplicate usernames.
    */
    PGresult *result = PQexecParams(
        conn,
        "INSERT INTO admins (username, password_hash) "
        "SELECT $1, $2 "
        "WHERE NOT EXISTS (SELECT 1 FROM admins) "
        "RETURNING id",
        2,
        NULL,
        (const char *const[]){username, stored_hash},
        NULL,
        NULL,
        0);

    int success = PQresultStatus(result) == PGRES_TUPLES_OK &&
                  PQntuples(result) == 1;

    if (success)
    {
        printf("\nAdministrator registered successfully!\n");
        printf("Administrator ID: %s\n", PQgetvalue(result, 0, 0));
    }
    else if (PQresultStatus(result) == PGRES_TUPLES_OK)
    {
        printf("\nRegistration refused: an administrator may already exist.\n");
    }
    else
    {
        fprintf(stderr, "Registration failed: %s",
                PQresultErrorMessage(result));
    }

    PQclear(result);
    PQfinish(conn);

    OPENSSL_cleanse(stored_hash, sizeof(stored_hash));

    return success ? 0 : 1;
}
