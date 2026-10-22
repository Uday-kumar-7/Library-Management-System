#include "file_io.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

/*
 * ============================================================================
 * FILE HANDLING AND PERSISTENCE ENGINE
 * ============================================================================
 *
 * Algorithm Explanation:
 * 1. Delimited Text Format: Uses pipe ('|') delimiters to cleanly separate fields
 *    while allowing titles and author names to contain spaces and commas.
 * 2. Dedicated Safe Field Extractor (next_field):
 *    Unlike standard strtok(), which collapses consecutive delimiters (skipping
 *    empty fields like "||" in unreturned items), next_field() strictly preserves
 *    empty fields so null or blank entries are reliably parsed without desyncing.
 * 3. Automatic Seed Initialization: If the book data file does not exist on
 *    first run, the system populates initial canonical books so users can
 *    immediately interact without manual setup.
 * 4. Dual-Index Population: Reading books simultaneously populates the
 *    Linked List (for catalog operations) and the Binary Search Tree
 *    (for fast O(log n) lookups).
 * ============================================================================
 */

// Safely extracts the next field from a delimited string, correctly handling empty fields
static char* next_field(char **cursor, char delimiter) {
    if (!cursor || !*cursor) return NULL;
    char *start = *cursor;
    char *end = strchr(start, delimiter);
    if (end) {
        *end = '\0';
        *cursor = end + 1;
    } else {
        *cursor = NULL; // Reached last token
    }
    return start;
}

// Creates the data/ directory if it doesn't already exist
void ensure_data_directory(void) {
    MKDIR("data");
}

// Seeds default canonical books into the catalog on first launch
static void seed_default_books(BookNode **head, BSTNode **root) {
    Book defaults[] = {
        {101, "The C Programming Language", "Brian Kernighan & Dennis Ritchie", "Computer Science", 5, 5},
        {102, "Introduction to Algorithms", "Thomas Cormen & Ronald Rivest", "Algorithms", 4, 4},
        {103, "Clean Code", "Robert C. Martin", "Software Engineering", 6, 6},
        {104, "Operating System Concepts", "Abraham Silberschatz", "Operating Systems", 3, 3},
        {105, "Design Patterns", "Erich Gamma et al.", "Software Architecture", 4, 4},
        {106, "Structure and Interpretation", "Harold Abelson & Gerald Sussman", "Computer Science", 2, 2}
    };

    int count = sizeof(defaults) / sizeof(defaults[0]);
    for (int i = 0; i < count; i++) {
        insert_book_list(head, &defaults[i]);
        *root = bst_insert(*root, &defaults[i]);
    }
}

// Loads book records from disk file into Linked List and BST
// Returns total books loaded
int load_books_from_file(const char *filename, BookNode **head, BSTNode **root) {
    ensure_data_directory();
    FILE *fp = fopen(filename, "r");

    if (!fp) {
        // File does not exist yet; populate initial seed catalog
        seed_default_books(head, root);
        save_books_to_file(filename, *head);
        return get_book_count(*head);
    }

    char line[512];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        // Skip comment lines and blank lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        line[strcspn(line, "\r\n")] = '\0';

        char *cursor = line;
        Book b;
        memset(&b, 0, sizeof(Book));

        // Format: ID|Title|Author|Category|TotalCopies|AvailableCopies
        char *token = next_field(&cursor, '|');
        if (!token) continue;
        b.id = atoi(token);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(b.title, token, MAX_TITLE_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(b.author, token, MAX_AUTHOR_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(b.category, token, MAX_CATEGORY_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        b.total_copies = atoi(token);

        token = next_field(&cursor, '|');
        if (!token) continue;
        b.available_copies = atoi(token);

        // Sanity checks
        if (b.available_copies > b.total_copies) {
            b.available_copies = b.total_copies;
        }

        insert_book_list(head, &b);
        *root = bst_insert(*root, &b);
        loaded++;
    }

    fclose(fp);
    return loaded;
}

// Saves all book records from the Linked List to disk file
int save_books_to_file(const char *filename, const BookNode *head) {
    ensure_data_directory();
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "[!] Error: Failed to open '%s' for writing.\n", filename);
        return 0;
    }

    fprintf(fp, "# ID|Title|Author|Category|TotalCopies|AvailableCopies\n");

    const BookNode *curr = head;
    int saved = 0;
    while (curr != NULL) {
        fprintf(fp, "%d|%s|%s|%s|%d|%d\n",
                curr->data.id,
                curr->data.title,
                curr->data.author,
                curr->data.category,
                curr->data.total_copies,
                curr->data.available_copies);
        curr = curr->next;
        saved++;
    }

    fclose(fp);
    return saved;
}

// Loads issue and circulation records from disk file
int load_issues_from_file(const char *filename, IssueNode **head) {
    ensure_data_directory();
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0; // No prior issue file exists
    }

    char line[512];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        line[strcspn(line, "\r\n")] = '\0';

        char *cursor = line;
        IssueRecord rec;
        memset(&rec, 0, sizeof(IssueRecord));

        // Format: IssueID|BookID|BookTitle|StudentID|StudentName|IssueDate|DueDate|ReturnDate|Status
        char *token = next_field(&cursor, '|');
        if (!token) continue;
        rec.issue_id = atoi(token);

        token = next_field(&cursor, '|');
        if (!token) continue;
        rec.book_id = atoi(token);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(rec.book_title, token, MAX_TITLE_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(rec.student_id, token, MAX_STUDENT_ID_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(rec.student_name, token, MAX_STUDENT_NAME_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(rec.issue_date, token, MAX_DATE_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(rec.due_date, token, MAX_DATE_LEN - 1);

        token = next_field(&cursor, '|');
        if (token) {
            strncpy(rec.return_date, token, MAX_DATE_LEN - 1);
        }

        token = next_field(&cursor, '|');
        if (token) {
            rec.status = (atoi(token) == 1) ? ISSUE_RETURNED : ISSUE_ACTIVE;
        }

        add_issue_record(head, &rec);
        loaded++;
    }

    fclose(fp);
    return loaded;
}

// Saves all issue records to disk file
int save_issues_to_file(const char *filename, const IssueNode *head) {
    ensure_data_directory();
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "[!] Error: Failed to open '%s' for writing.\n", filename);
        return 0;
    }

    fprintf(fp, "# IssueID|BookID|BookTitle|StudentID|StudentName|IssueDate|DueDate|ReturnDate|Status(0=Active,1=Returned)\n");

    const IssueNode *curr = head;
    int saved = 0;
    while (curr != NULL) {
        fprintf(fp, "%d|%d|%s|%s|%s|%s|%s|%s|%d\n",
                curr->data.issue_id,
                curr->data.book_id,
                curr->data.book_title,
                curr->data.student_id,
                curr->data.student_name,
                curr->data.issue_date,
                curr->data.due_date,
                curr->data.return_date,
                (int)curr->data.status);
        curr = curr->next;
        saved++;
    }

    fclose(fp);
    return saved;
}

// Loads pending waitlist requests from disk file into Queue
int load_requests_from_file(const char *filename, RequestQueue *q) {
    ensure_data_directory();
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0;
    }

    char line[512];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        line[strcspn(line, "\r\n")] = '\0';

        char *cursor = line;
        Request r;
        memset(&r, 0, sizeof(Request));

        // Format: RequestID|BookID|StudentID|StudentName|RequestDate
        char *token = next_field(&cursor, '|');
        if (!token) continue;
        r.request_id = atoi(token);

        token = next_field(&cursor, '|');
        if (!token) continue;
        r.book_id = atoi(token);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(r.student_id, token, MAX_STUDENT_ID_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(r.student_name, token, MAX_STUDENT_NAME_LEN - 1);

        token = next_field(&cursor, '|');
        if (!token) continue;
        strncpy(r.request_date, token, MAX_DATE_LEN - 1);

        enqueue(q, &r);
        loaded++;
    }

    fclose(fp);
    return loaded;
}

// Saves pending waitlist requests from Queue to disk file
int save_requests_to_file(const char *filename, const RequestQueue *q) {
    ensure_data_directory();
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "[!] Error: Failed to open '%s' for writing.\n", filename);
        return 0;
    }

    fprintf(fp, "# RequestID|BookID|StudentID|StudentName|RequestDate\n");

    if (q) {
        const RequestNode *curr = q->front;
        int saved = 0;
        while (curr != NULL) {
            fprintf(fp, "%d|%d|%s|%s|%s\n",
                    curr->data.request_id,
                    curr->data.book_id,
                    curr->data.student_id,
                    curr->data.student_name,
                    curr->data.request_date);
            curr = curr->next;
            saved++;
        }
        fclose(fp);
        return saved;
    }

    fclose(fp);
    return 0;
}
