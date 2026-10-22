#include "issue.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * ISSUE & RETURN MANAGEMENT
 * ============================================================================
 *
 * Algorithm Explanation:
 * 1. Tracks every borrowing transaction using a linked list of IssueRecords.
 * 2. Active records represent currently borrowed books (status == ISSUE_ACTIVE).
 * 3. Returning a book transitions the record to ISSUE_RETURNED and timestamps it.
 * 4. Issue ID generation guarantees unique serial identifiers.
 * ============================================================================
 */

// Allocates and initializes a new IssueNode
IssueNode* create_issue_node(const IssueRecord *rec) {
    if (!rec) return NULL;
    IssueNode *node = (IssueNode *)malloc(sizeof(IssueNode));
    if (!node) {
        fprintf(stderr, "[!] Error: Memory allocation failed for IssueNode.\n");
        return NULL;
    }
    node->data = *rec;
    node->next = NULL;
    return node;
}

// Appends an issue transaction record to the linked list
int add_issue_record(IssueNode **head, const IssueRecord *rec) {
    if (!head || !rec) return 0;

    IssueNode *new_node = create_issue_node(rec);
    if (!new_node) return 0;

    if (*head == NULL) {
        *head = new_node;
        return 1;
    }

    IssueNode *curr = *head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
    return 1;
}

// Searches for an active issue record by book_id and student_id
IssueNode* find_active_issue(IssueNode *head, int book_id, const char *student_id) {
    IssueNode *curr = head;
    while (curr != NULL) {
        if (curr->data.status == ISSUE_ACTIVE &&
            curr->data.book_id == book_id &&
            strcmp(curr->data.student_id, student_id) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Searches for an active issue record by unique issue_id
IssueNode* find_active_issue_by_id(IssueNode *head, int issue_id) {
    IssueNode *curr = head;
    while (curr != NULL) {
        if (curr->data.status == ISSUE_ACTIVE && curr->data.issue_id == issue_id) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Marks an active issue record as RETURNED with the specified return date
int return_book_record(IssueNode *head, int issue_id, const char *return_date) {
    IssueNode *node = find_active_issue_by_id(head, issue_id);
    if (!node) {
        return 0; // Not found or already returned
    }

    node->data.status = ISSUE_RETURNED;
    if (return_date && strlen(return_date) > 0) {
        strncpy(node->data.return_date, return_date, MAX_DATE_LEN - 1);
        node->data.return_date[MAX_DATE_LEN - 1] = '\0';
    } else {
        get_current_date(node->data.return_date, MAX_DATE_LEN);
    }
    return 1;
}

// Displays only currently active (unreturned) issues
void display_active_issues(const IssueNode *head) {
    int count = 0;
    const IssueNode *curr = head;

    // Check if there are any active records
    while (curr != NULL) {
        if (curr->data.status == ISSUE_ACTIVE) count++;
        curr = curr->next;
    }

    if (count == 0) {
        printf("\n[i] No active book issues found. All books are accounted for on shelves.\n");
        return;
    }

    printf("\n+- %-6s -+- %-6s -+- %-25s -+- %-12s -+- %-18s -+- %-11s -+- %-11s -+\n",
           "------", "------", "-------------------------", "------------", "------------------", "-----------", "-----------");
    printf("|  %-6s |  %-6s |  %-25s |  %-12s |  %-18s |  %-11s |  %-11s |\n",
           "Iss ID", "Bk ID", "Book Title", "Student ID", "Student Name", "Issue Date", "Due Date");
    printf("+- %-6s -+- %-6s -+- %-25s -+- %-12s -+- %-18s -+- %-11s -+- %-11s -+\n",
           "------", "------", "-------------------------", "------------", "------------------", "-----------", "-----------");

    curr = head;
    while (curr != NULL) {
        if (curr->data.status == ISSUE_ACTIVE) {
            printf("|  %-6d |  %-6d |  %-25.25s |  %-12.12s |  %-18.18s |  %-11.11s |  %-11.11s |\n",
                   curr->data.issue_id,
                   curr->data.book_id,
                   curr->data.book_title,
                   curr->data.student_id,
                   curr->data.student_name,
                   curr->data.issue_date,
                   curr->data.due_date);
        }
        curr = curr->next;
    }

    printf("+- %-6s -+- %-6s -+- %-25s -+- %-12s -+- %-18s -+- %-11s -+- %-11s -+\n",
           "------", "------", "-------------------------", "------------", "------------------", "-----------", "-----------");
    printf("   Total Active Book Borrowers: %d\n", count);
}

// Displays complete circulation history (both active and returned)
void display_all_issues(const IssueNode *head) {
    if (!head) {
        printf("\n[i] No circulation records found.\n");
        return;
    }

    printf("\n+- %-6s -+- %-6s -+- %-22s -+- %-12s -+- %-11s -+- %-11s -+- %-11s -+- %-10s -+\n",
           "------", "------", "----------------------", "------------", "-----------", "-----------", "-----------", "----------");
    printf("|  %-6s |  %-6s |  %-22s |  %-12s |  %-11s |  %-11s |  %-11s |  %-10s |\n",
           "Iss ID", "Bk ID", "Book Title", "Student ID", "Issue Date", "Due Date", "Return Date", "Status");
    printf("+- %-6s -+- %-6s -+- %-22s -+- %-12s -+- %-11s -+- %-11s -+- %-11s -+- %-10s -+\n",
           "------", "------", "----------------------", "------------", "-----------", "-----------", "-----------", "----------");

    const IssueNode *curr = head;
    int count = 0;
    while (curr != NULL) {
        const char *status_str = (curr->data.status == ISSUE_ACTIVE) ? "ACTIVE" : "RETURNED";
        const char *ret_str = (curr->data.status == ISSUE_ACTIVE) ? "--" : curr->data.return_date;

        printf("|  %-6d |  %-6d |  %-22.22s |  %-12.12s |  %-11.11s |  %-11.11s |  %-11.11s |  %-10s |\n",
               curr->data.issue_id,
               curr->data.book_id,
               curr->data.book_title,
               curr->data.student_id,
               curr->data.issue_date,
               curr->data.due_date,
               ret_str,
               status_str);
        curr = curr->next;
        count++;
    }

    printf("+- %-6s -+- %-6s -+- %-22s -+- %-12s -+- %-11s -+- %-11s -+- %-11s -+- %-10s -+\n",
           "------", "------", "----------------------", "------------", "-----------", "-----------", "-----------", "----------");
    printf("   Total Circulation History Records: %d\n", count);
}

// Determines the next auto-increment issue ID
int get_next_issue_id(const IssueNode *head) {
    int max_id = 1000;
    const IssueNode *curr = head;
    while (curr != NULL) {
        if (curr->data.issue_id > max_id) {
            max_id = curr->data.issue_id;
        }
        curr = curr->next;
    }
    return max_id + 1;
}

// Frees all memory allocated for issue records
void free_issue_list(IssueNode **head) {
    if (!head) return;
    IssueNode *curr = *head;
    while (curr != NULL) {
        IssueNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    *head = NULL;
}
