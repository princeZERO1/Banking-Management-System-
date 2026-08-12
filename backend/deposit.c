#include "database.c"

int main() {
    char* acc = get_session_cookie();
    send_json_headers();

    if (strlen(acc) == 0) {
        printf("{\"status\": \"error\", \"message\": \"Not logged in.\"}");
        return 0;
    }

    parse_post_data();
    char* amount_str = get_param("depositAmount");
    char* remarks = get_param("remarks");
    
    double amount = atof(amount_str);
    if (amount <= 0) {
        printf("{\"status\": \"error\", \"message\": \"Invalid amount.\"}");
        return 0;
    }

    Customer c;
    if (get_customer(acc, &c)) {
        if (c.status == 0) {
            printf("{\"status\": \"error\", \"message\": \"Account is frozen.\"}");
            return 0;
        }

        c.balance += amount;
        
        if (update_customer(&c)) {
            Transaction t;
            generate_transaction_id(t.transaction_id);
            strcpy(t.account_number, c.account_number);
            strcpy(t.type, "Deposit");
            t.amount = amount;
            strcpy(t.sender_account, "Self");
            strcpy(t.receiver_account, "Self");
            get_current_datetime(t.date, t.time);
            t.updated_balance = c.balance;
            if (strlen(remarks) > 0) {
                strcpy(t.description, remarks);
            } else {
                strcpy(t.description, "Cash Deposit");
            }

            save_transaction(&t);

            printf("{\"status\": \"success\", \"message\": \"Deposit successful.\", \"new_balance\": %.2f}", c.balance);
        } else {
            printf("{\"status\": \"error\", \"message\": \"Failed to update account.\"}");
        }
    } else {
        printf("{\"status\": \"error\", \"message\": \"Account not found.\"}");
    }

    return 0;
}
