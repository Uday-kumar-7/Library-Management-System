#ifndef BOOK_H
#define BOOK_H

/*
 * Data Structure: Linked List for Book Records
 *
 * A singly linked list allows dynamic allocation of book records
 * during runtime without pre-allocating fixed array sizes.
 * It provides O(1) insertion at the head or O(n) sorted insertion,
 * and efficient deletion without memory shifting.
 */

#define MAX_TITLE_LEN    100
#define MAX_AUTHOR_LEN    80
#define MAX_CATEGORY_LEN  40

typedef struct Book {
    int id;                                 // Unique book identifier
    char title[MAX_TITLE_LEN];              // Title of the book
    char author[MAX_AUTHOR_LEN];            // Author name
    char category[MAX_CATEGORY_LEN];        // Genre / Subject category
    int total_copies;                       // Total copies owned by the library
    int available_copies;                   // Current copies available on shelf
} Book;

typedef struct BookNode {
    Book data;
    struct BookNode *next;
} BookNode;

// Linked List Function Prototypes
BookNode* create_book_node(const Book *b);
int insert_book_list(BookNode **head, const Book *b);
BookNode* search_book_list_by_id(BookNode *head, int id);
int update_book_list(BookNode *head, int id, const char *new_title, const char *new_author, const char *new_category, int new_total);
int delete_book_list(BookNode **head, int id, Book *deleted_backup);
void display_all_books(const BookNode *head);
int get_book_count(const BookNode *head);
void free_book_list(BookNode **head);

#endif // BOOK_H
