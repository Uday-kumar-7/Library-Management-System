#ifndef QUEUE_H
#define QUEUE_H

/*
 * Data Structure: Queue (FIFO - First In, First Out) for Pending Book Requests
 *
 * When all copies of a book are currently issued (available_copies == 0),
 * students requesting the book are placed into a waitlist queue.
 *
 * When a copy is returned to the library, the system checks the queue
 * to notify and optionally auto-fulfill the request for the student
 * who has been waiting the longest (FIFO order).
 *
 * Time Complexity:
 * - Enqueue: O(1)
 * - Dequeue: O(1)
 * - Peek:    O(1)
 */

#define MAX_STUDENT_ID_LEN   30
#define MAX_STUDENT_NAME_LEN 60
#define MAX_DATE_LEN         20

typedef struct Request {
    int request_id;
    int book_id;
    char student_id[MAX_STUDENT_ID_LEN];
    char student_name[MAX_STUDENT_NAME_LEN];
    char request_date[MAX_DATE_LEN];
} Request;

typedef struct RequestNode {
    Request data;
    struct RequestNode *next;
} RequestNode;

typedef struct RequestQueue {
    RequestNode *front;
    RequestNode *rear;
    int count;
} RequestQueue;

// Queue Operations
void init_queue(RequestQueue *q);
int enqueue(RequestQueue *q, const Request *r);
int dequeue(RequestQueue *q, Request *r);
int peek_queue(const RequestQueue *q, Request *r);
int dequeue_for_book(RequestQueue *q, int book_id, Request *r);
int has_request_for_book(const RequestQueue *q, int book_id);
void display_queue(const RequestQueue *q);
int is_queue_empty(const RequestQueue *q);
void free_queue(RequestQueue *q);

#endif // QUEUE_H
