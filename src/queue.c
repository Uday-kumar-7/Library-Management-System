#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * DATA STRUCTURE: QUEUE (FIFO) FOR PENDING BOOK REQUESTS / WAITLIST
 * ============================================================================
 *
 * Algorithm Explanation:
 * 1. FIFO (First-In, First-Out) Principle:
 *    - The student who requested an out-of-stock book first is served first
 *      when that book is returned.
 * 2. Enqueue Operation (O(1)):
 *    - Appends a new RequestNode at the rear of the queue.
 *    - Updates rear pointer to the new node.
 * 3. Dequeue Operation (O(1)):
 *    - Removes the node at the front of the queue.
 *    - Updates front pointer to the next node and frees memory.
 * 4. Priority Dequeue by Book ID:
 *    - Scans the queue from front to rear to identify the earliest request
 *      specifically matched with a newly returned book ID.
 * ============================================================================
 */

// Initializes an empty waitlist queue
void init_queue(RequestQueue *q) {
    if (q) {
        q->front = NULL;
        q->rear = NULL;
        q->count = 0;
    }
}

// Adds a student's book request to the rear of the queue
// Time Complexity: O(1)
int enqueue(RequestQueue *q, const Request *r) {
    if (!q || !r) return 0;

    RequestNode *node = (RequestNode *)malloc(sizeof(RequestNode));
    if (!node) {
        fprintf(stderr, "[!] Error: Memory allocation failed for RequestNode.\n");
        return 0;
    }

    node->data = *r;
    node->next = NULL;

    if (q->rear == NULL) {
        // Queue was empty
        q->front = node;
        q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }

    q->count++;
    return 1;
}

// Removes and returns the oldest request from the front of the queue
// Time Complexity: O(1)
int dequeue(RequestQueue *q, Request *r) {
    if (!q || q->front == NULL) {
        return 0; // Queue underflow
    }

    RequestNode *temp = q->front;
    if (r) {
        *r = temp->data;
    }

    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->count--;
    return 1;
}

// Peeks at the front request without removing it
// Time Complexity: O(1)
int peek_queue(const RequestQueue *q, Request *r) {
    if (!q || q->front == NULL) {
        return 0;
    }
    if (r) {
        *r = q->front->data;
    }
    return 1;
}

// Finds and dequeues the earliest pending request for a specific Book ID
// Time Complexity: O(k) where k is the number of queued requests
// Returns: 1 if a request was found and dequeued, 0 otherwise
int dequeue_for_book(RequestQueue *q, int book_id, Request *r) {
    if (!q || q->front == NULL) {
        return 0;
    }

    RequestNode *curr = q->front;
    RequestNode *prev = NULL;

    while (curr != NULL && curr->data.book_id != book_id) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        return 0; // No pending request for this specific book
    }

    // Copy request data
    if (r) {
        *r = curr->data;
    }

    // Splice node out of the queue
    if (prev == NULL) {
        // Matched node was at the front
        q->front = curr->next;
        if (q->front == NULL) {
            q->rear = NULL;
        }
    } else {
        prev->next = curr->next;
        if (curr == q->rear) {
            // Matched node was at the rear
            q->rear = prev;
        }
    }

    free(curr);
    q->count--;
    return 1;
}

// Checks if any request exists in the queue for a specific book ID
int has_request_for_book(const RequestQueue *q, int book_id) {
    if (!q) return 0;
    const RequestNode *curr = q->front;
    while (curr != NULL) {
        if (curr->data.book_id == book_id) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

// Displays all pending waitlist requests in FIFO sequence
void display_queue(const RequestQueue *q) {
    if (is_queue_empty(q)) {
        printf("\n[i] The pending book request queue is currently empty.\n");
        return;
    }

    printf("\n+- %-6s -+- %-8s -+- %-15s -+- %-25s -+- %-12s -+\n",
           "------", "--------", "---------------", "-------------------------", "------------");
    printf("|  %-6s |  %-8s |  %-15s |  %-25s |  %-12s |\n",
           "Req ID", "Book ID", "Student ID", "Student Name", "Req Date");
    printf("+- %-6s -+- %-8s -+- %-15s -+- %-25s -+- %-12s -+\n",
           "------", "--------", "---------------", "-------------------------", "------------");

    const RequestNode *curr = q->front;
    while (curr != NULL) {
        printf("|  %-6d |  %-8d |  %-15.15s |  %-25.25s |  %-12.12s |\n",
               curr->data.request_id,
               curr->data.book_id,
               curr->data.student_id,
               curr->data.student_name,
               curr->data.request_date);
        curr = curr->next;
    }

    printf("+- %-6s -+- %-8s -+- %-15s -+- %-25s -+- %-12s -+\n",
           "------", "--------", "---------------", "-------------------------", "------------");
    printf("   Total Pending Requests in Queue: %d\n", q->count);
}

// Checks if the request queue is empty
int is_queue_empty(const RequestQueue *q) {
    return (q == NULL || q->front == NULL);
}

// Frees all nodes in the request queue
void free_queue(RequestQueue *q) {
    if (!q) return;

    RequestNode *curr = q->front;
    while (curr != NULL) {
        RequestNode *temp = curr;
        curr = curr->next;
        free(temp);
    }

    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}
