#include "database.c"

int main() {
    char* acc = get_session_cookie();
    send_json_headers();

    if (strlen(acc) == 0) {
        printf("{\"status\": \"error\", \"message\": \"Not logged in.\"}");
        return 0;
    }

    printf("{\"status\": \"success\", \"transactions\": [");
    
    FILE *file = fopen(TRANSACTION_FILE, "rb");
    if (file) {
        Transaction t;
        int first = 1;
        while (fread(&t, sizeof(Transaction), 1, file)) {
            if (strcmp(t.account_number, acc) == 0) {
                if (!first) printf(",");
                printf("{");
                printf("\"id\": \"%s\",", t.transaction_id);
                printf("\"type\": \"%s\",", t.type);
                printf("\"amount\": %.2f,", t.amount);
                printf("\"balance\": %.2f,", t.updated_balance);
                printf("\"date\": \"%s\",", t.date);
                printf("\"description\": \"%s\"", t.description);
                printf("}");
                first = 0;
            }
        }
        fclose(file);
    }
    printf("]}");

    return 0;
}
