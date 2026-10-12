
#include <stdio.h>
#include <libpq-fe.h>

int main(void)
{
    PGconn *conn = PQconnectdb(
        "host=localhost "
        "port=5432 "
        "dbname=student_management "
        "user=makuto"
    );

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection failed: %s\n",
                PQerrorMessage(conn));

        PQfinish(conn);
        return 1;
    }

    printf("Successfully connected to PostgreSQL!\n");

    printf("Database: %s\n", PQdb(conn));
    printf("User: %s\n", PQuser(conn));

    PQfinish(conn);

    return 0;
}
