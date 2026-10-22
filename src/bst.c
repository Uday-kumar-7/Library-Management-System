#include "bst.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * DATA STRUCTURE: BINARY SEARCH TREE (BST) FOR BOOK SEARCHING & INDEXING
 * ============================================================================
 *
 * Algorithm Explanation:
 * 1. BST Invariant: For any node N with key K:
 *    - All keys in left subtree of N are < K.
 *    - All keys in right subtree of N are > K.
 * 2. Search Algorithm:
 *    - Compares target ID with current node ID.
 *    - If target < current->id, recurse into left subtree.
 *    - If target > current->id, recurse into right subtree.
 *    - Average complexity: O(log n), reducing search space by half at each step.
 * 3. Deletion Algorithm:
 *    - Case 1 (0 children): Free node directly.
 *    - Case 2 (1 child): Replace node with its single child.
 *    - Case 3 (2 children): Replace node's value with its In-Order Successor
 *      (minimum value node in the right subtree), then delete the successor.
 * 4. In-Order Traversal (L-N-R):
 *    - Traverses left subtree, visits current node, traverses right subtree.
 *    - Produces a strictly sorted sequence of Book IDs in O(n) time.
 * ============================================================================
 */

// Inserts a new book node into the BST keyed by book_id
// Time Complexity: O(log n) average, O(n) worst-case
BSTNode* bst_insert(BSTNode *root, const Book *b) {
    if (!b) return root;

    // Base case: Reached a leaf position where new node should be attached
    if (root == NULL) {
        BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
        if (!node) {
            fprintf(stderr, "[!] Error: Memory allocation failed for BSTNode.\n");
            return NULL;
        }
        node->data = *b;
        node->left = NULL;
        node->right = NULL;
        return node;
    }

    // Traverse left or right based on Book ID comparison
    if (b->id < root->data.id) {
        root->left = bst_insert(root->left, b);
    } else if (b->id > root->data.id) {
        root->right = bst_insert(root->right, b);
    } else {
        // If ID already exists in BST, synchronize updated book details
        root->data = *b;
    }

    return root;
}

// Searches for a book by ID using Binary Search Tree properties
// Time Complexity: O(log n) average, O(n) worst-case
BSTNode* bst_search_id(BSTNode *root, int id) {
    if (root == NULL || root->data.id == id) {
        return root;
    }

    if (id < root->data.id) {
        return bst_search_id(root->left, id);
    } else {
        return bst_search_id(root->right, id);
    }
}

// Searches books by title (case-insensitive substring match) using tree traversal
void bst_search_title(const BSTNode *root, const char *keyword, int *found_count) {
    if (!root || !keyword || !found_count) return;

    // In-order traversal: Left subtree
    bst_search_title(root->left, keyword, found_count);

    // Current node examination
    if (case_insensitive_contains(root->data.title, keyword)) {
        if (*found_count == 0) {
            printf("\n+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+\n",
                   "------", "------------------------------", "--------------------", "---------------", "-----", "-----");
            printf("|  %-6s |  %-30s |  %-20s |  %-15s |  %-5s |  %-5s |\n",
                   "ID", "Title", "Author", "Category", "Total", "Avail");
            printf("+- %-6s -+- %-30s -+- %-20s -+- %-15s -+- %-5s -+- %-5s -+\n",
                   "------", "------------------------------", "--------------------", "---------------", "-----", "-----");
        }
        printf("|  %-6d |  %-30.30s |  %-20.20s |  %-15.15s |  %-5d |  %-5d |\n",
               root->data.id,
               root->data.title,
               root->data.author,
               root->data.category,
               root->data.total_copies,
               root->data.available_copies);
        (*found_count)++;
    }

    // In-order traversal: Right subtree
    bst_search_title(root->right, keyword, found_count);
}

// Finds the minimum node in a given subtree (leftmost descendant)
BSTNode* bst_find_min(BSTNode *root) {
    if (!root) return NULL;
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

// Deletes a node with the specified book ID from the BST
// Time Complexity: O(log n) average, O(n) worst-case
BSTNode* bst_delete(BSTNode *root, int id) {
    if (root == NULL) {
        return NULL;
    }

    if (id < root->data.id) {
        root->left = bst_delete(root->left, id);
    } else if (id > root->data.id) {
        root->right = bst_delete(root->right, id);
    } else {
        // Node found! Handle the 3 deletion cases:

        // Case 1 & 2: Node with 0 or 1 child
        if (root->left == NULL) {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        }

        // Case 3: Node with 2 children
        // Find In-Order Successor (smallest node in the right subtree)
        BSTNode *successor = bst_find_min(root->right);

        // Copy successor's data to current node
        root->data = successor->data;

        // Delete the successor from the right subtree
        root->right = bst_delete(root->right, successor->data.id);
    }

    return root;
}

// In-Order Traversal (Left, Node, Right) to display books sorted by ID
void bst_inorder_display(const BSTNode *root) {
    if (!root) return;

    bst_inorder_display(root->left);

    const char *status = (root->data.available_copies > 0) ? "AVAILABLE" : "OUT OF STOCK";
    printf("|  %-6d |  %-30.30s |  %-20.20s |  %-15.15s |  %-5d |  %-5d |  %-10s |\n",
           root->data.id,
           root->data.title,
           root->data.author,
           root->data.category,
           root->data.total_copies,
           root->data.available_copies,
           status);

    bst_inorder_display(root->right);
}

// Frees all nodes in the BST using Post-Order Traversal (Left, Right, Node)
void free_bst(BSTNode **root) {
    if (!root || !*root) return;

    free_bst(&((*root)->left));
    free_bst(&((*root)->right));

    free(*root);
    *root = NULL;
}

// Rebuilds the entire BST from scratch from a linked list
BSTNode* build_bst_from_list(const BookNode *head) {
    BSTNode *root = NULL;
    const BookNode *curr = head;
    while (curr != NULL) {
        root = bst_insert(root, &(curr->data));
        curr = curr->next;
    }
    return root;
}
