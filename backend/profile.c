#include "database.c"

int main() {
    char* acc = get_session_cookie();
    send_json_headers();

    if (strlen(acc) == 0) {
        printf("{\"status\": \"error\", \"message\": \"Not logged in.\"}");
        return 0;
    }

    Customer c;
    if (!get_customer(acc, &c)) {
        printf("{\"status\": \"error\", \"message\": \"Account not found.\"}");
        return 0;
    }

    parse_post_data();
    char* action = get_param("action");

    if (strcmp(action, "update") == 0) {
        char* name = get_param("name");
        char* email = get_param("email");
        char* phone = get_param("phone");
        char* address = get_param("address");
        
        // Prevent duplicate email/phone if changed
        if (strcmp(c.email, email) != 0 && check_email_exists(email)) {
            printf("{\"status\": \"error\", \"message\": \"Email already in use.\"}");
            return 0;
        }
        if (strcmp(c.phone, phone) != 0 && check_phone_exists(phone)) {
            printf("{\"status\": \"error\", \"message\": \"Phone already in use.\"}");
            return 0;
        }

        if (strlen(name) > 0) strcpy(c.name, name);
        if (strlen(email) > 0) strcpy(c.email, email);
        if (strlen(phone) > 0) strcpy(c.phone, phone);
        if (strlen(address) > 0) strcpy(c.address, address);

        char* old_pass = get_param("old_password");
        char* new_pass = get_param("new_password");

        if (strlen(new_pass) > 0) {
            if (strcmp(c.password, old_pass) == 0) {
                strcpy(c.password, new_pass);
            } else {
                printf("{\"status\": \"error\", \"message\": \"Incorrect current password.\"}");
                return 0;
            }
        }

        if (update_customer(&c)) {
            printf("{\"status\": \"success\", \"message\": \"Profile updated successfully.\"}");
        } else {
            printf("{\"status\": \"error\", \"message\": \"Failed to update profile.\"}");
        }
    } else {
        // Just fetch profile
        printf("{");
        printf("\"status\": \"success\",");
        printf("\"customer\": {");
        printf("\"name\": \"%s\",", c.name);
        printf("\"email\": \"%s\",", c.email);
        printf("\"phone\": \"%s\",", c.phone);
        printf("\"address\": \"%s\",", c.address);
        printf("\"account_number\": \"%s\",", c.account_number);
        printf("\"aadhaar\": \"%s\",", c.aadhaar);
        printf("\"pan\": \"%s\",", c.pan);
        printf("\"status\": %d", c.status);
        printf("}");
        printf("}");
    }

    return 0;
}
