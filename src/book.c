#include "book.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * DATA STRUCTURE: SINGLY LINKED LIST FOR BOOK MANAGEMENT
 * ============================================================================
 *
 * Algorithm Explanation:
 * 1. Node Creation: Dynamic heap allocation via malloc().
 * 2. Sorted Insertion: Maintains the list in ascending order of Book ID.
 *    - Traverses nodes until finding node with larger ID or reaching NULL.
 *    - Pointers rewired in O(1) once position is located.
 * 3. Deletion: Traverses with a lagging 'prev' pointer to splice the node out,
 *    backing up its data for the Undo Stack before calling free().
 * ============================================================================
 */

// Allocates and initializes a new BookNode on the heap
BookNode* create_book_node(const Book *b) {
    if (!b) return NULL;
    BookNode *node = (BookNode *)malloc(sizeof(BookNode));
    if (!node) {
        fprintf(stderr, "[!] Error: Memory allocation failed for BookNode.\n");
        return NULL;
    }
    node->data = *b;
    node->next = NULL;
    return node;
}

// Inserts a book into the linked list in ascending order of Book ID
// Returns: 1 on success, 0 if duplicate ID, -1 on allocation failure
int insert_book_list(BookNode **head, const Book *b) {
    if (!head || !b) return 0;

    // Check for duplicate ID
    if (search_book_list_by_id(*head, b->id) != NULL) {
        return 0; // Duplicate detected
    }

    BookNode *new_node = create_book_node(b);
    if (!new_node) return -1;

    // Case 1: Empty list or insert at head (b->id is smaller than head->data.id)
    if (*head == NULL || (*head)->data.id > b->id) {
        new_node->next = *head;
        *head = new_node;
        return 1;
    }

    // Case 2: Traverse to find correct insertion position (maintain sorted order)
    BookNode *curr = *head;
    while (curr->next != NULL && curr->next->data.id < b->id) {
        curr = curr->next;
    }

    new_node->next = curr->next;
    curr->next = new_node;
    return 1;
}

// Linear search through the linked list to find a book by its unique ID
// Time Complexity: O(n)
BookNode* search_book_list_by_id(BookNode *head, int id) {
    BookNode *curr = head;
    while (curr != NULL) {
        if (curr->data.id == id) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Updates an existing book record
// Returns: 1 on success, 0 if not found, -1 if new_total < currently issued copies
int update_book_list(BookNode *head, int id, const char *new_title, const char *new_author,
                     const char *new_category, int new_total) {
    BookNode *node = search_book_list_by_id(head, id);
    if (!node) return 0;

    int currently_issued = node->data.total_copies - node->data.available_copies;
    if (new_total < currently_issued) {
        // Cannot reduce total copies below currently borrowed count
        return -1;
    }

    if (new_title && strlen(new_title) > 0) {
        strncpy(node->data.title, new_title, MAX_TITLE_LEN - 1);
        node->data.title[MAX_TITLE_LEN - 1] = '\0';
    }
    if (new_author && strlen(new_author) > 0) {
        strncpy(node->data.author, new_author, MAX_AUTHOR_LEN - 1);
        node->data.author[MAX_AUTHOR_LEN - 1] = '\0';
    }
    if (new_category && strlen(new_category) > 0) {
        strncpy(node->data.category, new_category, MAX_CATEGORY_LEN - 1);
        node->data.category[MAX_CATEGORY_LEN - 1] = '\0';
    }

    node->data.total_copies = new_total;
    node->data.available_copies = new_total - currently_issued;
    return 1;
}

// Deletes a book from the linked list by ID and saves a backup copy for the undo stack
// Returns: 1 on success, 0 if not found, -2 if active borrowed copies exist
int delete_book_list(BookNode **head, int id, Book *deleted_backup) {
    if (!head || !*head) return 0;

    BookNode *curr = *head;
    BookNode *prev = NULL;

    while (curr != NULL && curr->data.id != id) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        return 0; // Book not found
    }

    // Safety check: Prevent deleting books currently in circulation
    int currently_issued = curr->data.total_copies - curr->data.available_copies;
    if (currently_issued > 0) {
        return -2; // Cannot delete while copies are actively borrowed
    }

    // Backup book data for undo stack
    if (deleted_backup) {
        *deleted_backup = curr->data;
    }

    // Unlink the node
    if (prev == NULL) {
        *head = curr->next; // Deleted node was head
    } else {
        prev->next = curr->next;
    }

    free(curr);
    return 1;
}

// Displays all books in a formatted table
void display_all_books(const BookNode *head) {
    if (!head) {
        printf("\n[i] The library catalog is currently empty.\n");
        return;
    }

    printf("\n+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "---------------", "-----", "-----", "----------");
    printf("|  %-6s |  %-30s |  %-20s |  %-15s |  %-5s |  %-5s |  %-10s |\n",
           "ID", "Title", "Author", "Category", "Total", "Avail", "Status");
    printf("+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "---------------", "-----", "-----", "----------");

    const BookNode *curr = head;
    int count = 0;
    while (curr != NULL) {
        const char *status = (curr->data.available_copies > 0) ? "AVAILABLE" : "OUT OF STOCK";
        printf("|  %-6d |  %-30.30s |  %-20.20s |  %-15.15s |  %-5d |  %-5d |  %-10s |\n",
               curr->data.id,
               curr->data.title,
               curr->data.author,
               curr->data.category,
               curr->data.total_copies,
               curr->data.available_copies,
               status);
        curr = curr->next;
        count++;
    }

    printf("+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "---------------", "-----", "-----", "----------");
    printf("   Total Books in Catalog: %d\n", count);
}

// Counts total books in the linked list
int get_book_count(const BookNode *head) {
    int count = 0;
    const BookNode *curr = head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    return count;
}

// Frees all memory allocated for the linked list
void free_book_list(BookNode **head) {
    if (!head) return;
    BookNode *curr = *head;
    while (curr != NULL) {
        BookNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    *head = NULL;
}
