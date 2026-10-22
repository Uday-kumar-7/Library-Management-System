#include "utils.h"

// Clear remaining characters in standard input buffer
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // discard
    }
}

// Safely reads an integer within [min_val, max_val] with complete validation
int read_int_bounded(const char *prompt, int min_val, int max_val) {
    char line[128];
    int value;
    char extra;

    while (1) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) {
            if (feof(stdin)) {
                return min_val;
            }
            clearerr(stdin);
            continue;
        }

        // Parse integer and ensure no trailing garbage except whitespace
        if (sscanf(line, "%d %c", &value, &extra) == 1) {
            if (value >= min_val && value <= max_val) {
                return value;
            }
            printf("[!] Value must be between %d and %d. Please try again.\n", min_val, max_val);
        } else {
            printf("[!] Invalid input. Please enter a valid integer.\n");
        }
    }
}

// Safely reads a strictly positive integer (> 0)
int read_positive_int(const char *prompt) {
    return read_int_bounded(prompt, 1, 1000000000);
}

// Safely reads a non-empty string, trimming trailing newlines
void read_string(const char *prompt, char *buffer, int max_len) {
    char line[512];

    while (1) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) {
            if (feof(stdin)) {
                strncpy(buffer, "N/A", max_len - 1);
                buffer[max_len - 1] = '\0';
                return;
            }
            clearerr(stdin);
            continue;
        }

        // Remove newline character if present
        line[strcspn(line, "\r\n")] = '\0';

        // Trim leading spaces
        char *start = line;
        while (isspace((unsigned char)*start)) {
            start++;
        }

        // Check if string is empty
        if (*start == '\0') {
            printf("[!] Input cannot be empty. Please enter a value.\n");
            continue;
        }

        // Copy up to max_len - 1 characters
        strncpy(buffer, start, max_len - 1);
        buffer[max_len - 1] = '\0';
        break;
    }
}

// Reads a string that can be left blank (returns empty if user presses Enter)
void read_string_optional(const char *prompt, char *buffer, int max_len) {
    char line[512];

    printf("%s", prompt);
    if (!fgets(line, sizeof(line), stdin)) {
        clearerr(stdin);
        buffer[0] = '\0';
        return;
    }

    line[strcspn(line, "\r\n")] = '\0';

    char *start = line;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        buffer[0] = '\0';
        return;
    }

    strncpy(buffer, start, max_len - 1);
    buffer[max_len - 1] = '\0';
}

// Formats today's date as YYYY-MM-DD
void get_current_date(char *date_buf, int max_len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t) {
        strftime(date_buf, max_len, "%Y-%m-%d", t);
    } else {
        snprintf(date_buf, max_len, "2026-01-01");
    }
}

// Adds `days` to `issue_date` (in YYYY-MM-DD format) and stores result in `due_buf`
void calculate_due_date(const char *issue_date, int days, char *due_buf, int max_len) {
    int year = 2026, month = 1, day = 1;
    if (sscanf(issue_date, "%d-%d-%d", &year, &month, &day) == 3) {
        struct tm t = {0};
        t.tm_year = year - 1900;
        t.tm_mon  = month - 1;
        t.tm_mday = day + days; // mktime normalizes overflow in days
        mktime(&t);
        strftime(due_buf, max_len, "%Y-%m-%d", &t);
    } else {
        snprintf(due_buf, max_len, "2026-01-15");
    }
}

// Case-insensitive substring search: returns 1 if needle is inside haystack, 0 otherwise
int case_insensitive_contains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    if (*needle == '\0') return 1;

    size_t h_len = strlen(haystack);
    size_t n_len = strlen(needle);
    if (n_len > h_len) return 0;

    for (size_t i = 0; i <= h_len - n_len; i++) {
        size_t j;
        for (j = 0; j < n_len; j++) {
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) {
                break;
            }
        }
        if (j == n_len) return 1;
    }
    return 0;
}

// Prints a horizontal separator line
void print_divider(char ch, int length) {
    for (int i = 0; i < length; i++) {
        putchar(ch);
    }
    putchar('\n');
}

// Prints an aligned stylized section header
void print_header(const char *title) {
    printf("\n");
    print_divider('=', 78);
    printf("   %s\n", title);
    print_divider('=', 78);
}

// Pauses and waits for user to press ENTER before continuing
void pause_screen(void) {
    printf("\nPress [ENTER] to return to the menu...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // discard
    }
}
