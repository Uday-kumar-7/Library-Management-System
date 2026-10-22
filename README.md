# Library Management System (in C & Data Structures)

A robust, modular, high-performance console-based **Library Management System** engineered in **C99**, leveraging fundamental data structures for optimal time complexity, memory efficiency, and real-world reliability.

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Data Structures & Complexity Analysis](#data-structures--complexity-analysis)
  - [1. Singly Linked List (Catalog Master Storage)](#1-singly-linked-list-catalog-master-storage)
  - [2. Binary Search Tree (BST Indexing & Search)](#2-binary-search-tree-bst-indexing--search)
  - [3. Queue (FIFO Waitlist for Unavailable Books)](#3-queue-fifo-waitlist-for-unavailable-books)
  - [4. Stack (LIFO Undo Mechanism for Deletions)](#4-stack-lifo-undo-mechanism-for-deletions)
- [Project Architecture](#project-architecture)
- [Compiler Installation Guide (Windows)](#compiler-installation-guide-windows)
  - [Method 1: WinGet (Recommended - Fast & Portable)](#method-1-winget-recommended---fast--portable)
  - [Method 2: MSYS2 / UCRT64](#method-2-msys2--ucrt64)
  - [Method 3: Visual Studio Community / Build Tools](#method-3-visual-studio-community--build-tools)
- [Building and Running](#building-and-running)
  - [One-Click Build Script](#one-click-build-script)
  - [One-Click Run Script](#one-click-run-script)
  - [Using GNU Make](#using-gnu-make)
  - [Manual Compilation](#manual-compilation)
- [Feature Walkthrough & Menu Options](#feature-walkthrough--menu-options)
- [File Persistence Specifications](#file-persistence-specifications)
- [Input Validation & Error Handling](#input-validation--error-handling)

---

## Overview

The Library Management System provides a complete solution for cataloging books, tracking active circulation and loan durations, queuing reservation requests, and preserving data across restarts. It demonstrates real-world application of core computer science data structures:

- **Linked List** for dynamic inventory management without fixed memory limits.
- **Binary Search Tree (BST)** for fast $O(\log n)$ book lookups and natural alphabetical/ID sorting.
- **Queue** (FIFO) for handling student reservations when inventory is exhausted.
- **Stack** (LIFO) for transactional undo operations when a book is inadvertently deleted.

---

## Key Features

### 1. Book Management
- **Add Book**: Enforces unique book IDs, checks duplicates via BST, and stores metadata (Title, Author, Category, Copies).
- **Display Catalog**: Outputs a clean ASCII table with inventory levels and status (`AVAILABLE` vs `OUT OF STOCK`).
- **Search by ID**: High-speed $O(\log n)$ lookup via Binary Search Tree.
- **Search by Title**: Case-insensitive substring searching across the catalog.
- **Update Details**: Safely modify title, author, category, or total copies (prevents lowering copies below active loans).
- **Delete Book**: Unlinks from Linked List and BST; automatically pushes deleted snapshot to the Undo Stack.

### 2. Issue & Return Circulation
- **Issue Books**: Tracks student ID, student name, issue date, and computes due date (e.g. 14 days). Prevents duplicate concurrent loans of the same book to the same student.
- **Out-of-Stock Handling**: When available copies reach 0, automatically prompts the user to place the student in the **Waitlist Queue**.
- **Return Books**: Marks loan status as `RETURNED`, timestamps the return, and increments available shelf copies.
- **Auto-Fulfill Queue**: Upon book return, instantly checks if students are waiting in the queue and offers to auto-allocate the returned copy directly to the next student in line (FIFO).
- **Circulation History**: View both currently borrowed books (`ACTIVE`) and complete audit logs (`RETURNED`).

### 3. Undo Protection
- **Undo Delete**: Popping from the LIFO stack restores the deleted book back into both the Linked List and the Binary Search Tree seamlessly.
- **Stack Inspector**: View recently deleted records waiting in the undo buffer.

### 4. File Persistence
- **Automatic Auto-Save**: In-memory state is synchronized to disk on modifications and upon clean exit.
- **Seamless Startup Loading**: In-memory data structures are re-populated from files on start.
- **Seed Catalog**: Automatically creates canonical starter books on the first launch if no database exists.

---

## Data Structures & Complexity Analysis

| Data Structure | Component / Role | Primary Operations | Time Complexity | Space Complexity |
| :--- | :--- | :--- | :--- | :--- |
| **Singly Linked List** | Catalog Master Record | Insertion (Sorted by ID)<br>Traversal / Display<br>Deletion | $O(n)$<br>$O(n)$<br>$O(n)$ | $O(n)$ heap |
| **Binary Search Tree** | Fast Search Index | Search by Book ID<br>Insertion<br>In-Order Traversal (Sorted) | $O(\log n)$ avg, $O(n)$ worst<br>$O(\log n)$ avg<br>$O(n)$ | $O(n)$ heap |
| **Queue (FIFO)** | Pending Book Waitlist | Enqueue (Request)<br>Dequeue (Fulfill)<br>Priority Find by Book ID | $O(1)$<br>$O(1)$<br>$O(k)$ | $O(k)$ heap |
| **Stack (LIFO)** | Undo Deletion History | Push (On Delete)<br>Pop (On Undo)<br>Peek | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(m)$ heap |

### 1. Singly Linked List (Catalog Master Storage)
Each book is represented by a `BookNode` containing a `Book` structure and a pointer to `next`. Sorted insertion ensures the master catalog is continuously ordered without needing an external sorting pass.

### 2. Binary Search Tree (BST Indexing & Search)
The BST organizes books by `book_id`. When searching for a book among thousands of entries, comparison at each node reduces the search subspace by half:
- If `target_id < current->id`, search left subtree.
- If `target_id > current->id`, search right subtree.
- In-Order Traversal (`Left -> Node -> Right`) naturally prints the catalog in strictly ascending ID order.

### 3. Queue (FIFO Waitlist for Unavailable Books)
Maintains pointer references to `front` and `rear`:
- When an out-of-stock book is requested, a `RequestNode` is enqueued at `rear` in $O(1)$ time.
- When a book copy is returned, the student at `front` is dequeued first, upholding fairness.

### 4. Stack (LIFO Undo Mechanism for Deletions)
Maintains pointer reference to `top`:
- Deleting a book moves its snapshot to `UndoStack` top in $O(1)$ time.
- Choosing "Undo Last Deleted Book" pops the snapshot from `top` in $O(1)$ time and reinserts it into the live catalog.

---

## Project Architecture

```
Library-Management-System/
├── include/
│   ├── book.h          # Book data structure & Linked List interface
│   ├── bst.h           # Binary Search Tree declarations & prototypes
│   ├── queue.h         # FIFO Request Queue declarations & prototypes
│   ├── stack.h         # LIFO Undo Stack declarations & prototypes
│   ├── issue.h         # Issue & circulation tracking declarations
│   ├── file_io.h       # File persistence engine prototypes
│   └── utils.h         # Safe input readers, date utils & string helpers
├── src/
│   ├── book.c          # Linked list operations & formatting
│   ├── bst.c           # BST insertion, search, deletion & traversal
│   ├── queue.c         # Queue enqueue, dequeue & matching logic
│   ├── stack.c         # Stack push, pop & inspection logic
│   ├── issue.c         # Issue loan creation, due date & return logic
│   ├── file_io.c       # Delimited file parser (safe custom field extractor)
│   ├── utils.c         # Input validation, bounds checks & sanitization
│   └── main.c          # Interactive CLI menu & application state loop
├── data/
│   ├── books.txt       # Master book records file
│   ├── issues.txt      # Circulation & loan transaction records
│   └── requests.txt    # Pending waitlist reservation records
├── build.bat           # Windows automated compile script (auto-detects compilers)
├── run.bat             # Windows one-click execution script
├── Makefile            # Standard GNU Makefile (for make / mingw32-make)
└── README.md           # Documentation and compiler installation guide
```

---

## Compiler Installation Guide (Windows)

If your Windows computer does not have a C compiler installed, choose any of the methods below:

### Method 1: WinGet (Recommended - Fast & Portable)
Windows 10/11 includes the Windows Package Manager (`winget`). Open **PowerShell** or **Command Prompt** and execute:

```powershell
winget install --id MartinStorsjo.LLVM-MinGW.UCRT --accept-package-agreements --accept-source-agreements
```

Restart your terminal or run `refreshenv`. Both `gcc` and `clang` will be available.
*Note: The included `build.bat` automatically detects this installation directory even if not added to system PATH!*

### Method 2: MSYS2 / UCRT64
1. Download the MSYS2 installer from [msys2.org](https://www.msys2.org/).
2. Run the installer and launch the **MSYS2 UCRT64** terminal.
3. Install the GCC toolchain:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
   ```
4. Add `C:\msys64\ucrt64\bin` to your Windows `PATH` environment variable.

### Method 3: Visual Studio Community / Build Tools
1. Download **Visual Studio Community** or **Build Tools for Visual Studio** from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/).
2. In the installer, select **Desktop development with C++**.
3. Use the **Developer Command Prompt for VS** to compile with `cl.exe` or `clang`.

---

## Building and Running

### One-Click Build Script
In PowerShell or CMD inside the project root:
```cmd
build.bat
```
The script will locate your installed C compiler (`gcc` or `clang`), compile all source files with `-Wall -Wextra -std=c99 -O2`, and generate `library_system.exe`.

### One-Click Run Script
```cmd
run.bat
```
If `library_system.exe` has not been compiled yet, `run.bat` automatically invokes `build.bat` first.

### Using GNU Make
If you have `make` or `mingw32-make`:
```cmd
make
make run
```

### Manual Compilation
```cmd
gcc -Wall -Wextra -std=c99 -Iinclude src\*.c -o library_system.exe
.\library_system.exe
```

---

## Feature Walkthrough & Menu Options

Upon running `library_system.exe`, the interactive console displays:

```text
==============================================================================
           LIBRARY MANAGEMENT SYSTEM - DATA STRUCTURES POWERED
==============================================================================
  [ BOOK MANAGEMENT ]
    1. Add New Book
    2. Display All Books (Linked List Catalog)
    3. Search Book by ID (Binary Search Tree - O(log n))
    4. Search Books by Title (Case-Insensitive)
    5. Update Book Details
    6. Delete Book (Pushes to Undo Stack)
    7. Undo Last Deleted Book (Stack Pop - O(1))

  [ ISSUE & CIRCULATION ]
    8. Issue Book to Student (Auto-enqueues if out of stock)
    9. Return Book (Checks Waitlist Queue for auto-reissue)
   10. View Currently Borrowed Books (Active Issues)
   11. View Complete Circulation History

  [ DATA STRUCTURE INSPECTORS ]
   12. View Pending Request Queue (FIFO Waitlist)
   13. View Undo History Stack (LIFO Deletions)
   14. Display Books in BST In-Order (Sorted by ID)

   15. Save and Exit
------------------------------------------------------------------------------
```

### Common Scenarios:
1. **Borrowing an Out-of-Stock Book**:
   - If copies available is `0`, selecting **Option 8** asks: `Would you like to place the student into the Pending Request Queue? (1 = Yes, 0 = No)`.
   - Entering student details inserts them into the FIFO queue.
2. **Returning a Book with Waiting Students**:
   - When **Option 9** is selected to return a book, the system identifies that a student is waiting in the queue.
   - It prompts: `Would you like to fulfill the request for the next waiting student now?`. Selecting `1` auto-issues the book to the earliest applicant.
3. **Accidental Book Deletion**:
   - Deleting a book with **Option 6** pushes it onto the Undo Stack.
   - Selecting **Option 7** pops it from the stack and restores it to both the catalog list and the BST index.

---

## File Persistence Specifications

The system stores plain-text pipe-delimited (`|`) records inside the `data/` directory:

1. **`data/books.txt`**:
   ```
   # ID|Title|Author|Category|TotalCopies|AvailableCopies
   101|The C Programming Language|Brian Kernighan & Dennis Ritchie|Computer Science|5|5
   102|Introduction to Algorithms|Thomas Cormen & Ronald Rivest|Algorithms|4|4
   ```
2. **`data/issues.txt`**:
   ```
   # IssueID|BookID|BookTitle|StudentID|StudentName|IssueDate|DueDate|ReturnDate|Status(0=Active,1=Returned)
   1001|106|Structure and Interpretation|STU001|Alice Smith|2026-10-22|2026-11-05|2026-10-22|1
   1002|101|The C Programming Language|STU001|Alice Smith|2026-10-22|2026-11-05||0
   ```
3. **`data/requests.txt`**:
   ```
   # RequestID|BookID|StudentID|StudentName|RequestDate
   501|104|STU009|David Clark|2026-10-22
   ```

*Design Note: Unlike standard `strtok()` which collates adjacent delimiters, the custom `next_field()` parser preserves empty fields (such as empty `ReturnDate` in unreturned books) preventing off-by-one field desynchronization.*

---

## Input Validation & Error Handling

- **Buffer Overflow Protection**: Replaces unsafe `scanf("%s", ...)` with `fgets()` and bounded length string readers.
- **Garbage Input Handling**: Cleans stdin buffers and loops until a valid integer within specified bounds `[min, max]` is entered.
- **Business Rule Constraints**:
  - Duplicate Book IDs are rejected upon creation.
  - Total copies cannot be updated to a number lower than active borrowed copies.
  - Books currently in circulation cannot be deleted until all copies are returned.
  - The same student cannot borrow multiple copies of the same book simultaneously.
- **Graceful Heap Cleanup**: Deep traversal deallocation functions (`free_book_list`, `free_bst`, `free_undo_stack`, `free_queue`, `free_issue_list`) guarantee zero memory leaks on exit.
