#ifndef ISSUE_H
#define ISSUE_H

#include "book.h"
#include "queue.h"

/*
 * Issue and Return Record Management
 *
 * Maintains a linked list of book circulation events:
 * - Student borrowing books
 * - Due date computation (e.g. 14 days default)
 * - Return records with actual return date
 * - Tracking ACTIVE vs RETURNED status
 */

typedef enum {
    ISSUE_ACTIVE = 0,
    ISSUE_RETURNED = 1
} IssueStatus;

typedef struct IssueRecord {
    int issue_id;
    int book_id;
    char book_title[MAX_TITLE_LEN];
    char student_id[MAX_STUDENT_ID_LEN];
    char student_name[MAX_STUDENT_NAME_LEN];
    char issue_date[MAX_DATE_LEN];
    char due_date[MAX_DATE_LEN];
    char return_date[MAX_DATE_LEN];
    IssueStatus status;
} IssueRecord;

typedef struct IssueNode {
    IssueRecord data;
    struct IssueNode *next;
} IssueNode;

// Issue & Return Operations
IssueNode* create_issue_node(const IssueRecord *rec);
int add_issue_record(IssueNode **head, const IssueRecord *rec);
IssueNode* find_active_issue(IssueNode *head, int book_id, const char *student_id);
IssueNode* find_active_issue_by_id(IssueNode *head, int issue_id);
int return_book_record(IssueNode *head, int issue_id, const char *return_date);
void display_active_issues(const IssueNode *head);
void display_all_issues(const IssueNode *head);
int get_next_issue_id(const IssueNode *head);
void free_issue_list(IssueNode **head);

#endif // ISSUE_H
