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
        char date[15], time_s[15];
        get_current_datetime(date, time_s);
        
        printf("{");
        printf("\"status\": \"success\",");
        printf("\"balance\": %.2f,", c.balance);
        printf("\"name\": \"%s\",", c.name);
        printf("\"account_type\": \"%s\",", c.account_type);
        printf("\"last_updated\": \"%s %s\"", date, time_s);
        printf("}");
    } else {
        printf("{\"status\": \"error\", \"message\": \"Account not found.\"}");
    }

    return 0;
}
