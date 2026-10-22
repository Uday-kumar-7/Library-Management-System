#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/*
 * Utility functions for user input reading, date calculations,
 * string sanitization, and terminal formatting.
 */

// Clears leftover newline/characters from stdin
void clear_input_buffer(void);

// Safe integer reader within range [min_val, max_val]
int read_int_bounded(const char *prompt, int min_val, int max_val);

// Safe positive integer reader (> 0)
int read_positive_int(const char *prompt);

// Safe non-empty string reader with maximum buffer size protection
void read_string(const char *prompt, char *buffer, int max_len);

// Read string that can be optional/empty (press enter to keep current value)
void read_string_optional(const char *prompt, char *buffer, int max_len);

// Gets current system date formatted as YYYY-MM-DD
void get_current_date(char *date_buf, int max_len);

// Adds specified days to a YYYY-MM-DD date
void calculate_due_date(const char *issue_date, int days, char *due_buf, int max_len);

// Case-insensitive substring search (returns 1 if found, 0 otherwise)
int case_insensitive_contains(const char *haystack, const char *needle);

// Print visual separators and headers
void print_divider(char ch, int length);
void print_header(const char *title);
void pause_screen(void);

#endif // UTILS_H
