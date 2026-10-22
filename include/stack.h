#ifndef STACK_H
#define STACK_H

#include "book.h"

/*
 * Data Structure: Stack (LIFO - Last In, First Out) for Undo Deletion
 *
 * When a book is deleted from the library system, its complete snapshot
 * is pushed onto the undo stack.
 *
 * If the librarian chooses to undo the deletion, the most recently
 * deleted book is popped from the stack and restored to both the
 * Linked List and the Binary Search Tree.
 *
 * Time Complexity:
 * - Push: O(1)
 * - Pop:  O(1)
 * - Peek: O(1)
 */

typedef struct UndoNode {
    Book book;
    struct UndoNode *next;
} UndoNode;

typedef struct UndoStack {
    UndoNode *top;
    int count;
} UndoStack;

// Undo Stack Operations
void init_undo_stack(UndoStack *stack);
int push_undo(UndoStack *stack, const Book *b);
int pop_undo(UndoStack *stack, Book *b);
int peek_undo(const UndoStack *stack, Book *b);
int is_undo_stack_empty(const UndoStack *stack);
void display_undo_stack(const UndoStack *stack);
void free_undo_stack(UndoStack *stack);

#endif // STACK_H
