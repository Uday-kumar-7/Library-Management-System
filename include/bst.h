#ifndef BST_H
#define BST_H

#include "book.h"

/*
 * Data Structure: Binary Search Tree (BST) for Fast Book Searching
 *
 * Each node in the BST stores a book record.
 * Keys are ordered by Book ID:
 * - All nodes in the left subtree have IDs strictly less than the node's ID.
 * - All nodes in the right subtree have IDs strictly greater than the node's ID.
 *
 * Time Complexity:
 * - Search: O(log n) average, O(n) worst-case (degenerate)
 * - Insert: O(log n) average, O(n) worst-case
 * - Delete: O(log n) average, O(n) worst-case
 * - In-Order Traversal: O(n) yields books strictly sorted by ID
 */

typedef struct BSTNode {
    Book data;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

// BST Operations
BSTNode* bst_insert(BSTNode *root, const Book *b);
BSTNode* bst_search_id(BSTNode *root, int id);
void bst_search_title(const BSTNode *root, const char *keyword, int *found_count);
BSTNode* bst_find_min(BSTNode *root);
BSTNode* bst_delete(BSTNode *root, int id);
void bst_inorder_display(const BSTNode *root);
void free_bst(BSTNode **root);

// Helper to rebuild BST from a linked list
BSTNode* build_bst_from_list(const BookNode *head);

#endif // BST_H
