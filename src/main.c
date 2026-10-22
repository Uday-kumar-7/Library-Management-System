#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "book.h"
#include "bst.h"
#include "stack.h"
#include "queue.h"
#include "issue.h"
#include "file_io.h"
#include "utils.h"

// Global application state
typedef struct LibraryApp {
    BookNode     *book_list;      // Primary storage: Linked List of all books
    BSTNode      *book_bst;       // Index structure: BST for O(log n) searches
    UndoStack     undo_stack;     // Undo structure: Stack (LIFO) for deletions
    RequestQueue  request_queue;  // Waitlist structure: Queue (FIFO) for requests
    IssueNode    *issue_list;     // Circulation records: Linked list of issues/returns
    int           next_request_id;// Counter for sequential request IDs
} LibraryApp;

// Function prototypes for menu actions
static void handle_add_book(LibraryApp *app);
static void handle_display_books(LibraryApp *app);
static void handle_search_book_id(LibraryApp *app);
static void handle_search_book_title(LibraryApp *app);
static void handle_update_book(LibraryApp *app);
static void handle_delete_book(LibraryApp *app);
static void handle_undo_delete(LibraryApp *app);
static void handle_issue_book(LibraryApp *app);
static void handle_return_book(LibraryApp *app);
static void handle_display_active_issues(LibraryApp *app);
static void handle_display_all_issues(LibraryApp *app);
static void handle_display_request_queue(LibraryApp *app);
static void handle_display_undo_stack(LibraryApp *app);
static void handle_display_bst_inorder(LibraryApp *app);
static void save_all_data(LibraryApp *app);

// Initializes the entire application state and loads files
static void init_library_app(LibraryApp *app) {
    memset(app, 0, sizeof(LibraryApp));
    init_undo_stack(&app->undo_stack);
    init_queue(&app->request_queue);
    app->next_request_id = 501;

    printf("[*] Loading catalog data from disk...\n");
    int books_loaded = load_books_from_file(DEFAULT_BOOKS_FILE, &app->book_list, &app->book_bst);
    printf("    -> %d book records loaded.\n", books_loaded);

    printf("[*] Loading circulation records from disk...\n");
    int issues_loaded = load_issues_from_file(DEFAULT_ISSUES_FILE, &app->issue_list);
    printf("    -> %d issue/return records loaded.\n", issues_loaded);

    printf("[*] Loading pending waitlist requests from disk...\n");
    int requests_loaded = load_requests_from_file(DEFAULT_REQUESTS_FILE, &app->request_queue);
    printf("    -> %d waitlist requests loaded.\n", requests_loaded);

    if (requests_loaded > 0) {
        app->next_request_id = 501 + requests_loaded;
    }
}

// Cleans up all dynamic heap allocations and saves data
static void cleanup_library_app(LibraryApp *app) {
    save_all_data(app);
    free_book_list(&app->book_list);
    free_bst(&app->book_bst);
    free_undo_stack(&app->undo_stack);
    free_queue(&app->request_queue);
    free_issue_list(&app->issue_list);
}

// Persists all in-memory data structures to files
static void save_all_data(LibraryApp *app) {
    save_books_to_file(DEFAULT_BOOKS_FILE, app->book_list);
    save_issues_to_file(DEFAULT_ISSUES_FILE, app->issue_list);
    save_requests_to_file(DEFAULT_REQUESTS_FILE, &app->request_queue);
}

// Prints main interactive menu
static void display_menu(void) {
    print_divider('=', 78);
    printf("           LIBRARY MANAGEMENT SYSTEM - DATA STRUCTURES POWERED\n");
    print_divider('=', 78);
    printf("  [ BOOK MANAGEMENT ]\n");
    printf("    1. Add New Book\n");
    printf("    2. Display All Books (Linked List Catalog)\n");
    printf("    3. Search Book by ID (Binary Search Tree - O(log n))\n");
    printf("    4. Search Books by Title (Case-Insensitive)\n");
    printf("    5. Update Book Details\n");
    printf("    6. Delete Book (Pushes to Undo Stack)\n");
    printf("    7. Undo Last Deleted Book (Stack Pop - O(1))\n");
    printf("\n  [ ISSUE & CIRCULATION ]\n");
    printf("    8. Issue Book to Student (Auto-enqueues if out of stock)\n");
    printf("    9. Return Book (Checks Waitlist Queue for auto-reissue)\n");
    printf("   10. View Currently Borrowed Books (Active Issues)\n");
    printf("   11. View Complete Circulation History\n");
    printf("\n  [ DATA STRUCTURE INSPECTORS ]\n");
    printf("   12. View Pending Request Queue (FIFO Waitlist)\n");
    printf("   13. View Undo History Stack (LIFO Deletions)\n");
    printf("   14. Display Books in BST In-Order (Sorted by ID)\n");
    printf("\n   15. Save and Exit\n");
    print_divider('-', 78);
}

int main(void) {
    LibraryApp app;
    init_library_app(&app);

    int choice = 0;
    while (1) {
        display_menu();
        choice = read_int_bounded("Select an option (1-15): ", 1, 15);

        switch (choice) {
            case 1:  handle_add_book(&app); break;
            case 2:  handle_display_books(&app); break;
            case 3:  handle_search_book_id(&app); break;
            case 4:  handle_search_book_title(&app); break;
            case 5:  handle_update_book(&app); break;
            case 6:  handle_delete_book(&app); break;
            case 7:  handle_undo_delete(&app); break;
            case 8:  handle_issue_book(&app); break;
            case 9:  handle_return_book(&app); break;
            case 10: handle_display_active_issues(&app); break;
            case 11: handle_display_all_issues(&app); break;
            case 12: handle_display_request_queue(&app); break;
            case 13: handle_display_undo_stack(&app); break;
            case 14: handle_display_bst_inorder(&app); break;
            case 15:
                printf("\n[*] Saving library data and shutting down...\n");
                cleanup_library_app(&app);
                printf("[+] All data successfully saved. Goodbye!\n");
                return 0;
            default:
                printf("[!] Invalid option. Please select 1-15.\n");
                break;
        }

        pause_screen();
    }

    return 0;
}

// ----------------------------------------------------------------------------
// HANDLERS FOR USER OPERATIONS
// ----------------------------------------------------------------------------

static void handle_add_book(LibraryApp *app) {
    print_header("ADD NEW BOOK RECORD");
    Book b;
    memset(&b, 0, sizeof(Book));

    b.id = read_positive_int("Enter Unique Book ID (e.g. 107): ");

    // Check if ID already exists via BST (fast O(log n))
    if (bst_search_id(app->book_bst, b.id) != NULL) {
        printf("[!] Error: Book ID %d already exists in the catalog!\n", b.id);
        return;
    }

    read_string("Enter Book Title: ", b.title, MAX_TITLE_LEN);
    read_string("Enter Author Name: ", b.author, MAX_AUTHOR_LEN);
    read_string("Enter Category/Genre: ", b.category, MAX_CATEGORY_LEN);
    b.total_copies = read_int_bounded("Enter Total Copies (1-1000): ", 1, 1000);
    b.available_copies = b.total_copies;

    // Insert into Linked List and BST
    insert_book_list(&app->book_list, &b);
    app->book_bst = bst_insert(app->book_bst, &b);
    save_all_data(app);

    printf("\n[+] SUCCESS: Book '%s' (ID: %d) successfully added to catalog!\n", b.title, b.id);
}

static void handle_display_books(LibraryApp *app) {
    print_header("LIBRARY CATALOG (LINKED LIST)");
    display_all_books(app->book_list);
}

static void handle_search_book_id(LibraryApp *app) {
    print_header("SEARCH BOOK BY ID (BINARY SEARCH TREE)");
    int id = read_positive_int("Enter Book ID to search: ");

    // Efficient O(log n) BST search
    BSTNode *node = bst_search_id(app->book_bst, id);

    if (node) {
        printf("\n[+] MATCH FOUND (via BST O(log n) lookup):\n");
        printf("    ID:               %d\n", node->data.id);
        printf("    Title:            %s\n", node->data.title);
        printf("    Author:           %s\n", node->data.author);
        printf("    Category:         %s\n", node->data.category);
        printf("    Total Copies:     %d\n", node->data.total_copies);
        printf("    Available Copies: %d\n", node->data.available_copies);
        printf("    Status:           %s\n", (node->data.available_copies > 0) ? "AVAILABLE" : "OUT OF STOCK");
    } else {
        printf("\n[-] No book found with ID %d in the catalog.\n", id);
    }
}

static void handle_search_book_title(LibraryApp *app) {
    print_header("SEARCH BOOKS BY TITLE");
    char keyword[100];
    read_string("Enter title or keyword to search: ", keyword, sizeof(keyword));

    int found_count = 0;
    bst_search_title(app->book_bst, keyword, &found_count);

    if (found_count > 0) {
        printf("+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+\n",
               "------", "------------------------------", "--------------------", "---------------", "-----", "-----");
        printf("   Found %d matching book(s).\n", found_count);
    } else {
        printf("\n[-] No books found matching keyword '%s'.\n", keyword);
    }
}

static void handle_update_book(LibraryApp *app) {
    print_header("UPDATE BOOK DETAILS");
    int id = read_positive_int("Enter Book ID to update: ");

    BookNode *node = search_book_list_by_id(app->book_list, id);
    if (!node) {
        printf("\n[-] Book ID %d not found in catalog.\n", id);
        return;
    }

    int currently_issued = node->data.total_copies - node->data.available_copies;
    printf("\nCurrent Details:\n");
    printf("  Title: %s | Author: %s | Category: %s\n",
           node->data.title, node->data.author, node->data.category);
    printf("  Total Copies: %d | Available: %d | Currently Issued: %d\n\n",
           node->data.total_copies, node->data.available_copies, currently_issued);

    char new_title[MAX_TITLE_LEN];
    char new_author[MAX_AUTHOR_LEN];
    char new_category[MAX_CATEGORY_LEN];

    read_string_optional("New Title (press Enter to keep current): ", new_title, MAX_TITLE_LEN);
    read_string_optional("New Author (press Enter to keep current): ", new_author, MAX_AUTHOR_LEN);
    read_string_optional("New Category (press Enter to keep current): ", new_category, MAX_CATEGORY_LEN);

    int new_total = read_int_bounded("New Total Copies (must be >= issued copies): ",
                                     currently_issued, 1000);

    const char *title_to_set = (strlen(new_title) > 0) ? new_title : node->data.title;
    const char *author_to_set = (strlen(new_author) > 0) ? new_author : node->data.author;
    const char *cat_to_set = (strlen(new_category) > 0) ? new_category : node->data.category;

    update_book_list(app->book_list, id, title_to_set, author_to_set, cat_to_set, new_total);

    // Sync updated data into BST
    BSTNode *bst_node = bst_search_id(app->book_bst, id);
    if (bst_node) {
        bst_node->data = node->data;
    }

    save_all_data(app);
    printf("\n[+] SUCCESS: Book ID %d successfully updated!\n", id);
}

static void handle_delete_book(LibraryApp *app) {
    print_header("DELETE BOOK RECORD");
    int id = read_positive_int("Enter Book ID to delete: ");

    Book backup;
    memset(&backup, 0, sizeof(Book));

    int res = delete_book_list(&app->book_list, id, &backup);
    if (res == 1) {
        // Remove from BST
        app->book_bst = bst_delete(app->book_bst, id);

        // Push deleted book snapshot onto Undo Stack (LIFO)
        push_undo(&app->undo_stack, &backup);

        save_all_data(app);
        printf("\n[+] SUCCESS: Book '%s' (ID: %d) was deleted.\n", backup.title, backup.id);
        printf("    [TIP] You can undo this deletion at any time via menu option 7.\n");
    } else if (res == -2) {
        printf("\n[!] Operation Denied: Book ID %d cannot be deleted because copies are actively borrowed!\n", id);
    } else {
        printf("\n[-] Book ID %d not found in catalog.\n", id);
    }
}

static void handle_undo_delete(LibraryApp *app) {
    print_header("UNDO LAST BOOK DELETION (STACK LIFO)");

    if (is_undo_stack_empty(&app->undo_stack)) {
        printf("\n[!] Undo stack is empty. There are no recent deletions to restore.\n");
        return;
    }

    Book restored_book;
    if (pop_undo(&app->undo_stack, &restored_book)) {
        // Re-insert into Linked List and BST
        insert_book_list(&app->book_list, &restored_book);
        app->book_bst = bst_insert(app->book_bst, &restored_book);
        save_all_data(app);

        printf("\n[+] SUCCESS: Restored deleted book from Undo Stack:\n");
        printf("    ID:           %d\n", restored_book.id);
        printf("    Title:        %s\n", restored_book.title);
        printf("    Author:       %s\n", restored_book.author);
        printf("    Total Copies: %d\n", restored_book.total_copies);
    }
}

static void handle_issue_book(LibraryApp *app) {
    print_header("ISSUE BOOK TO STUDENT");
    int book_id = read_positive_int("Enter Book ID to issue: ");

    BookNode *book_node = search_book_list_by_id(app->book_list, book_id);
    if (!book_node) {
        printf("\n[-] Book ID %d not found in catalog.\n", book_id);
        return;
    }

    printf("\nBook: '%s' by %s\n", book_node->data.title, book_node->data.author);
    printf("Total Copies: %d | Available on Shelf: %d\n\n",
           book_node->data.total_copies, book_node->data.available_copies);

    // Case 1: Copies are out of stock -> Offer waitlist queue (FIFO)
    if (book_node->data.available_copies <= 0) {
        printf("[!] NOTICE: All copies of this book are currently checked out!\n");
        printf("Would you like to place the student into the Pending Request Queue? (1 = Yes, 0 = No): ");
        int enqueue_choice = read_int_bounded("", 0, 1);
        if (enqueue_choice == 1) {
            Request req;
            memset(&req, 0, sizeof(Request));
            req.request_id = app->next_request_id++;
            req.book_id = book_id;
            read_string("Enter Student ID (e.g. STU101): ", req.student_id, MAX_STUDENT_ID_LEN);
            read_string("Enter Student Name: ", req.student_name, MAX_STUDENT_NAME_LEN);
            get_current_date(req.request_date, MAX_DATE_LEN);

            enqueue(&app->request_queue, &req);
            save_all_data(app);

            printf("\n[+] ENQUEUED: Student %s (ID: %s) added to waitlist queue for Book #%d.\n",
                   req.student_name, req.student_id, book_id);
            printf("    They will be served first (FIFO) when a copy is returned!\n");
        }
        return;
    }

    // Case 2: Copy is available -> Issue book
    IssueRecord rec;
    memset(&rec, 0, sizeof(IssueRecord));

    rec.issue_id = get_next_issue_id(app->issue_list);
    rec.book_id = book_id;
    strncpy(rec.book_title, book_node->data.title, MAX_TITLE_LEN - 1);

    read_string("Enter Student ID (e.g. STU101): ", rec.student_id, MAX_STUDENT_ID_LEN);

    // Prevent same student from borrowing multiple copies of the same book simultaneously
    if (find_active_issue(app->issue_list, book_id, rec.student_id) != NULL) {
        printf("\n[!] Notice: Student %s already has an active issue of this book!\n", rec.student_id);
        return;
    }

    read_string("Enter Student Name: ", rec.student_name, MAX_STUDENT_NAME_LEN);
    get_current_date(rec.issue_date, MAX_DATE_LEN);

    int loan_days = read_int_bounded("Enter Loan Period in days (default 14): ", 1, 90);
    calculate_due_date(rec.issue_date, loan_days, rec.due_date, MAX_DATE_LEN);
    rec.status = ISSUE_ACTIVE;

    // Decrement available copies
    book_node->data.available_copies--;

    // Sync with BST
    BSTNode *bst_node = bst_search_id(app->book_bst, book_id);
    if (bst_node) {
        bst_node->data.available_copies = book_node->data.available_copies;
    }

    // Add to issue history list
    add_issue_record(&app->issue_list, &rec);
    save_all_data(app);

    printf("\n[+] SUCCESS: Book issued successfully!\n");
    printf("    Issue ID:       %d\n", rec.issue_id);
    printf("    Book:           %s\n", rec.book_title);
    printf("    Student:        %s (%s)\n", rec.student_name, rec.student_id);
    printf("    Issue Date:     %s\n", rec.issue_date);
    printf("    Due Date:       %s\n", rec.due_date);
    printf("    Copies Left:    %d\n", book_node->data.available_copies);
}

static void handle_return_book(LibraryApp *app) {
    print_header("RETURN BORROWED BOOK");

    // Display active issues so user can easily reference the Issue ID
    display_active_issues(app->issue_list);

    int issue_id = read_positive_int("\nEnter Issue ID to return: ");
    IssueNode *issue_node = find_active_issue_by_id(app->issue_list, issue_id);

    if (!issue_node) {
        printf("\n[-] Active issue record with ID %d not found.\n", issue_id);
        return;
    }

    char ret_date[MAX_DATE_LEN];
    get_current_date(ret_date, MAX_DATE_LEN);
    return_book_record(app->issue_list, issue_id, ret_date);

    int book_id = issue_node->data.book_id;
    BookNode *book_node = search_book_list_by_id(app->book_list, book_id);

    if (book_node) {
        book_node->data.available_copies++;
        BSTNode *bst_node = bst_search_id(app->book_bst, book_id);
        if (bst_node) {
            bst_node->data.available_copies = book_node->data.available_copies;
        }
    }

    printf("\n[+] SUCCESS: Issue #%d marked as RETURNED on %s.\n", issue_id, ret_date);

    // Check if students are waiting in the queue for this book!
    if (has_request_for_book(&app->request_queue, book_id)) {
        printf("\n=======================================================\n");
        printf("[!] ALERT: Students are waiting in the Queue for Book #%d!\n", book_id);
        printf("Would you like to fulfill the request for the next waiting student now? (1 = Yes, 0 = No): ");
        int fulfill_choice = read_int_bounded("", 0, 1);

        if (fulfill_choice == 1) {
            Request next_req;
            if (dequeue_for_book(&app->request_queue, book_id, &next_req)) {
                // Issue to the waiting student
                IssueRecord new_rec;
                memset(&new_rec, 0, sizeof(IssueRecord));
                new_rec.issue_id = get_next_issue_id(app->issue_list);
                new_rec.book_id = book_id;
                if (book_node) {
                    strncpy(new_rec.book_title, book_node->data.title, MAX_TITLE_LEN - 1);
                }
                strncpy(new_rec.student_id, next_req.student_id, MAX_STUDENT_ID_LEN - 1);
                strncpy(new_rec.student_name, next_req.student_name, MAX_STUDENT_NAME_LEN - 1);
                get_current_date(new_rec.issue_date, MAX_DATE_LEN);
                calculate_due_date(new_rec.issue_date, 14, new_rec.due_date, MAX_DATE_LEN);
                new_rec.status = ISSUE_ACTIVE;

                if (book_node) {
                    book_node->data.available_copies--;
                    BSTNode *bst_node = bst_search_id(app->book_bst, book_id);
                    if (bst_node) {
                        bst_node->data.available_copies = book_node->data.available_copies;
                    }
                }

                add_issue_record(&app->issue_list, &new_rec);
                printf("\n[+] AUTO-ISSUED to waiting student %s (ID: %s)!\n",
                       new_rec.student_name, new_rec.student_id);
                printf("    New Issue ID: %d | Due Date: %s\n", new_rec.issue_id, new_rec.due_date);
            }
        }
    }

    save_all_data(app);
}

static void handle_display_active_issues(LibraryApp *app) {
    print_header("CURRENTLY BORROWED BOOKS (ACTIVE ISSUES)");
    display_active_issues(app->issue_list);
}

static void handle_display_all_issues(LibraryApp *app) {
    print_header("COMPLETE CIRCULATION HISTORY");
    display_all_issues(app->issue_list);
}

static void handle_display_request_queue(LibraryApp *app) {
    print_header("PENDING WAITLIST REQUEST QUEUE (FIFO)");
    display_queue(&app->request_queue);
}

static void handle_display_undo_stack(LibraryApp *app) {
    print_header("UNDO DELETIONS HISTORY (STACK LIFO)");
    display_undo_stack(&app->undo_stack);
}

static void handle_display_bst_inorder(LibraryApp *app) {
    print_header("BOOKS IN BINARY SEARCH TREE (IN-ORDER TRAVERSAL)");
    if (!app->book_bst) {
        printf("\n[i] Binary Search Tree is currently empty.\n");
        return;
    }

    printf("\n+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "---------------", "-----", "-----", "----------");
    printf("|  %-6s |  %-30s |  %-20s |  %-15s |  %-5s |  %-5s |  %-10s |\n",
           "ID", "Title", "Author", "Category", "Total", "Avail", "Status");
    printf("+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "---------------", "-----", "-----", "----------");

    bst_inorder_display(app->book_bst);

    printf("+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "---------------", "-----", "-----", "----------");
    printf("   Notice: In-Order Traversal (Left-Root-Right) naturally visits nodes in sorted ID order!\n");
}
