#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * ============================================================================
 * DATA STRUCTURE: STACK (LIFO) FOR UNDOING BOOK DELETIONS
 * ============================================================================
 *
 * Algorithm Explanation:
 * 1. LIFO (Last-In, First-Out) Principle:
 *    - The most recently deleted book is always at the top of the stack.
 * 2. Push Operation (O(1)):
 *    - Allocates an UndoNode containing the deleted book's snapshot.
 *    - Inserts node at the head (top) of the stack.
 * 3. Pop Operation (O(1)):
 *    - Extracts the top node data to restore into catalog.
 *    - Updates top pointer to the next element and frees popped node.
 * ============================================================================
 */

// Initializes an empty undo stack
void init_undo_stack(UndoStack *stack) {
    if (stack) {
        stack->top = NULL;
        stack->count = 0;
    }
}

// Pushes a deleted book onto the undo stack
// Time Complexity: O(1)
int push_undo(UndoStack *stack, const Book *b) {
    if (!stack || !b) return 0;

    UndoNode *node = (UndoNode *)malloc(sizeof(UndoNode));
    if (!node) {
        fprintf(stderr, "[!] Error: Memory allocation failed for UndoNode.\n");
        return 0;
    }

    node->book = *b;
    node->next = stack->top;
    stack->top = node;
    stack->count++;
    return 1;
}

// Pops the most recently deleted book from the undo stack
// Time Complexity: O(1)
// Returns: 1 if pop succeeded, 0 if stack is empty
int pop_undo(UndoStack *stack, Book *b) {
    if (!stack || stack->top == NULL) {
        return 0; // Stack underflow
    }

    UndoNode *temp = stack->top;
    if (b) {
        *b = temp->book;
    }
    stack->top = temp->next;
    free(temp);
    stack->count--;
    return 1;
}

// Inspects the top book of the stack without removing it
// Time Complexity: O(1)
int peek_undo(const UndoStack *stack, Book *b) {
    if (!stack || stack->top == NULL) {
        return 0;
    }
    if (b) {
        *b = stack->top->book;
    }
    return 1;
}

// Checks if the undo stack is empty
int is_undo_stack_empty(const UndoStack *stack) {
    return (stack == NULL || stack->top == NULL);
}

// Displays all books currently in the undo stack
void display_undo_stack(const UndoStack *stack) {
    if (is_undo_stack_empty(stack)) {
        printf("\n[i] Undo stack is empty. No recent deletions to undo.\n");
        return;
    }

    printf("\n+- %-6s -+- %-30s -+- %-20s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "----------");
    printf("|  %-6s |  %-30s |  %-20s |  %-10s |\n",
           "ID", "Title", "Author", "Copies");
    printf("+- %-6s -+- %-30s -+- %-20s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "----------");

    const UndoNode *curr = stack->top;
    while (curr != NULL) {
        printf("|  %-6d |  %-30.30s |  %-20.20s |  %-10d |\n",
               curr->book.id,
               curr->book.title,
               curr->book.author,
               curr->book.total_copies);
        curr = curr->next;
    }
    printf("+- %-6s -+- %-30s -+- %-20s -+- %-10s -+\n",
           "------", "------------------------------", "--------------------", "----------");
    printf("   Total Undoable Deletions in Stack: %d\n", stack->count);
}

// Frees all remaining nodes in the undo stack
void free_undo_stack(UndoStack *stack) {
    if (!stack) return;

    UndoNode *curr = stack->top;
    while (curr != NULL) {
        UndoNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    stack->top = NULL;
    stack->count = 0;
}
