#ifndef FILE_IO_H
#define FILE_IO_H

#include "book.h"
#include "bst.h"
#include "issue.h"
#include "queue.h"

/*
 * File Persistence Management
 *
 * Saves and loads records to/from persistent disk storage
 * so data survives application restarts.
 *
 * Files handled:
 * - data/books.txt     (Book catalog records)
 * - data/issues.txt    (Circulation / Issue records)
 * - data/requests.txt  (Pending waitlist requests)
 */

#define DEFAULT_BOOKS_FILE    "data/books.txt"
#define DEFAULT_ISSUES_FILE   "data/issues.txt"
#define DEFAULT_REQUESTS_FILE "data/requests.txt"

// Book file persistence
int load_books_from_file(const char *filename, BookNode **head, BSTNode **root);
int save_books_to_file(const char *filename, const BookNode *head);

// Issue record file persistence
int load_issues_from_file(const char *filename, IssueNode **head);
int save_issues_to_file(const char *filename, const IssueNode *head);

// Waitlist request file persistence
int load_requests_from_file(const char *filename, RequestQueue *q);
int save_requests_to_file(const char *filename, const RequestQueue *q);

// Ensure data directory exists
void ensure_data_directory(void);

#endif // FILE_IO_H
