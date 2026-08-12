#include "database.c"

int main() {
    parse_post_data();
    
    char* acc_num = get_param("account_number");
    char* password = get_param("password");

    Customer c;
    if (get_customer(acc_num, &c)) {
        if (strcmp(c.password, password) == 0) {
            if (c.status == 0) {
                send_json_headers();
                printf("{\"status\": \"error\", \"message\": \"Account is frozen. Please contact admin.\"}");
                return 0;
            }
            // Success - Set session cookie
            printf("Set-Cookie: session_acc=%s; Path=/;\n", c.account_number);
            send_json_headers();
            printf("{\"status\": \"success\", \"message\": \"Login successful.\"}");
        } else {
            send_json_headers();
            printf("{\"status\": \"error\", \"message\": \"Invalid password.\"}");
        }
    } else {
        send_json_headers();
        printf("{\"status\": \"error\", \"message\": \"Account not found.\"}");
    }

    return 0;
}
