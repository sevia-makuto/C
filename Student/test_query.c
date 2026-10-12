
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
        fprintf(stderr, "Connection failed: %s",
                PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    PGresult *result = PQexec(
        conn,
        "SELECT current_database(), current_user"
    );

    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Query failed: %s",
                PQerrorMessage(conn));
        PQclear(result);
        PQfinish(conn);
        return 1;
    }

    printf("Database: %s\n",
           PQgetvalue(result, 0, 0));

    printf("Connected user: %s\n",
           PQgetvalue(result, 0, 1));

    PQclear(result);
    PQfinish(conn);

    return 0;
}
