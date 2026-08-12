#include "database.c"

int main() {
    char* acc = get_session_cookie();
    send_json_headers();

    if (strlen(acc) == 0) {
        printf("{\"status\": \"error\", \"message\": \"Not logged in.\"}");
        return 0;
    }

    Customer c;
    if (get_customer(acc, &c)) {
        // Find recent transactions
        FILE *file = fopen(TRANSACTION_FILE, "rb");
        Transaction t;
        int txn_count = 0;
        
        printf("{");
        printf("\"status\": \"success\",");
        printf("\"customer\": {");
        printf("\"name\": \"%s\",", c.name);
        printf("\"account_number\": \"%s\",", c.account_number);
        printf("\"account_type\": \"%s\",", c.account_type);
        printf("\"balance\": %.2f,", c.balance);
        printf("\"email\": \"%s\",", c.email);
        printf("\"phone\": \"%s\",", c.phone);
        printf("\"address\": \"%s\",", c.address);
        printf("\"aadhaar\": \"%s\",", c.aadhaar);
        printf("\"pan\": \"%s\",", c.pan);
        printf("\"status\": %d", c.status);
        printf("},");
        
        printf("\"transactions\": [");
        if (file) {
            // Read backwards or read all and take last 5. We will read all into memory.
            // For simplicity, we just print them all or limit. Let's print all matching for now.
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
                    txn_count++;
                }
            }
            fclose(file);
        }
        printf("]");
        printf("}");

    } else {
        printf("{\"status\": \"error\", \"message\": \"Account not found.\"}");
    }

    return 0;
}
