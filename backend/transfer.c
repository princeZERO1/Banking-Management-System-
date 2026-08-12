#include "database.c"

int main() {
    char* acc = get_session_cookie();
    send_json_headers();

    if (strlen(acc) == 0) {
        printf("{\"status\": \"error\", \"message\": \"Not logged in.\"}");
        return 0;
    }

    parse_post_data();
    char* receiver = get_param("receiverAccount");
    char* receiver_name = get_param("receiverName"); // We could validate this against DB
    char* amount_str = get_param("transferAmount");
    char* purpose = get_param("purpose");
    
    double amount = atof(amount_str);
    if (amount <= 0) {
        printf("{\"status\": \"error\", \"message\": \"Invalid amount.\"}");
        return 0;
    }

    if (strcmp(acc, receiver) == 0) {
        printf("{\"status\": \"error\", \"message\": \"Cannot transfer to your own account.\"}");
        return 0;
    }

    Customer sender;
    Customer rec;

    if (get_customer(acc, &sender) && get_customer(receiver, &rec)) {
        if (sender.status == 0) {
            printf("{\"status\": \"error\", \"message\": \"Your account is frozen.\"}");
            return 0;
        }
        if (rec.status == 0) {
            printf("{\"status\": \"error\", \"message\": \"Receiver account is frozen.\"}");
            return 0;
        }
        if (sender.balance < amount) {
            printf("{\"status\": \"error\", \"message\": \"Insufficient balance.\"}");
            return 0;
        }

        // Perform Transfer
        sender.balance -= amount;
        rec.balance += amount;

        if (update_customer(&sender) && update_customer(&rec)) {
            char date[15], time_s[15];
            get_current_datetime(date, time_s);
            
            // Sender Transaction
            Transaction ts;
            generate_transaction_id(ts.transaction_id);
            strcpy(ts.account_number, sender.account_number);
            strcpy(ts.type, "Transfer");
            ts.amount = amount;
            strcpy(ts.sender_account, sender.account_number);
            strcpy(ts.receiver_account, rec.account_number);
            strcpy(ts.date, date);
            strcpy(ts.time, time_s);
            ts.updated_balance = sender.balance;
            sprintf(ts.description, "Transfer to %s (%s)", rec.name, purpose);
            save_transaction(&ts);

            // Receiver Transaction
            Transaction tr;
            strcpy(tr.transaction_id, ts.transaction_id); // Same ID for tracking
            strcpy(tr.account_number, rec.account_number);
            strcpy(tr.type, "Deposit"); // It's a deposit for them
            tr.amount = amount;
            strcpy(tr.sender_account, sender.account_number);
            strcpy(tr.receiver_account, rec.account_number);
            strcpy(tr.date, date);
            strcpy(tr.time, time_s);
            tr.updated_balance = rec.balance;
            sprintf(tr.description, "Transfer from %s (%s)", sender.name, purpose);
            save_transaction(&tr);

            printf("{\"status\": \"success\", \"message\": \"Transfer successful.\", \"new_balance\": %.2f}", sender.balance);
        } else {
            printf("{\"status\": \"error\", \"message\": \"Transfer failed during update.\"}");
        }
    } else {
        printf("{\"status\": \"error\", \"message\": \"Invalid receiver account.\"}");
    }

    return 0;
}
