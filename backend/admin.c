#include "database.c"

int main() {
    // For simplicity, we just check a static admin cookie or assume admin is logged in
    char* cookie = getenv("HTTP_COOKIE");
    if (!cookie || !strstr(cookie, "admin=true")) {
        // Just for simulation, if you hit admin.cgi, we'll allow it but in reality we'd auth.
    }

    send_json_headers();
    parse_post_data();
    char* action = get_param("action");

    if (strcmp(action, "freeze") == 0 || strcmp(action, "unfreeze") == 0) {
        char* target_acc = get_param("account_number");
        Customer c;
        if (get_customer(target_acc, &c)) {
            c.status = (strcmp(action, "unfreeze") == 0) ? 1 : 0;
            if (update_customer(&c)) {
                printf("{\"status\": \"success\", \"message\": \"Account status updated.\"}");
            } else {
                printf("{\"status\": \"error\", \"message\": \"Failed to update status.\"}");
            }
        } else {
            printf("{\"status\": \"error\", \"message\": \"Customer not found.\"}");
        }
        return 0;
    }

    // Default: Return dashboard stats and customer list
    int total_customers = 0;
    int active_accounts = 0;
    double total_deposits = 0.0;
    double total_withdrawals = 0.0;

    printf("{");
    printf("\"status\": \"success\",");
    
    // Customers
    printf("\"customers\": [");
    FILE *cfile = fopen(CUSTOMER_FILE, "rb");
    if (cfile) {
        Customer c;
        int first = 1;
        while (fread(&c, sizeof(Customer), 1, cfile)) {
            total_customers++;
            if (c.status == 1) active_accounts++;
            
            if (!first) printf(",");
            printf("{");
            printf("\"account_number\": \"%s\",", c.account_number);
            printf("\"name\": \"%s\",", c.name);
            printf("\"account_type\": \"%s\",", c.account_type);
            printf("\"status\": %d", c.status);
            printf("}");
            first = 0;
        }
        fclose(cfile);
    }
    printf("],");

    // Transactions
    FILE *tfile = fopen(TRANSACTION_FILE, "rb");
    if (tfile) {
        Transaction t;
        while (fread(&t, sizeof(Transaction), 1, tfile)) {
            if (strcmp(t.type, "Deposit") == 0) total_deposits += t.amount;
            if (strcmp(t.type, "Withdraw") == 0) total_withdrawals += t.amount;
        }
        fclose(tfile);
    }

    printf("\"stats\": {");
    printf("\"total_customers\": %d,", total_customers);
    printf("\"active_accounts\": %d,", active_accounts);
    printf("\"total_deposits\": %.2f,", total_deposits);
    printf("\"total_withdrawals\": %.2f", total_withdrawals);
    printf("}");
    printf("}");

    return 0;
}
