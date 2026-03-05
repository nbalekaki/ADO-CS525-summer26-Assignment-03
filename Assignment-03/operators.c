/*******************************************************************************
 * Advanced Database Organization - Assignment 03
 * Query Execution Engine – Iterator (Volcano) Model
 *
 * Student Name: [YOUR NAME]
 * Student ID:   [YOUR ID]
 *
 * INSTRUCTIONS:
 * - Implement all functions declared in operator.h
 * - Do not modify the Operator or Tuple struct definitions
 * - Each next() call must return exactly one tuple (heap-allocated)
 * - Each next() call must return NULL when the operator is exhausted
 * - Each close() must free all memory allocated by that operator
 *   and recursively close child operators
 * - Test your implementation using the provided test files
 * - Run Valgrind to check for memory leaks before submitting
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operator.h"

/******************************************************************************
 * UTILITY: Write tuples to a binary file (used by test harnesses)
 ******************************************************************************/

/**
 * Write an array of Tuples to a binary file.
 * Each tuple is stored as a raw struct (sizeof(Tuple) bytes).
 *
 * @param filename  Path to output file
 * @param tuples    Array of Tuple values
 * @param count     Number of tuples to write
 */
void write_tuples_to_file(const char *filename, Tuple *tuples, int count) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Open file for binary write ("wb")
     * 2. For each tuple, zero-initialize a local copy (memset to 0),
     *    copy only the num_attr values into it, then fwrite that copy.
     *    This avoids writing uninitialized bytes for unused values[] slots.
     * 3. Close the file
     */
}

/******************************************************************************
 * OPERATOR 1: TABLE SCAN
 *
 * Opens a binary file and returns one Tuple per next() call.
 * Returns NULL when all tuples have been read.
 ******************************************************************************/

/**
 * table_scan – create a table scan operator over a binary file.
 *
 * @param filename  Path to binary relation file
 * @return          Pointer to the constructed Operator
 */
Operator* table_scan(const char *filename) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate a state struct that holds the FILE* and any other needed fields
     * 2. Open filename in binary read mode ("rb")
     * 3. Set op->next  = a function that reads and returns the next Tuple
     * 4. Set op->close = a function that closes the file and frees the state
     * 5. Return the operator
     *
     * next() behavior:
     *   - Allocate a Tuple on the heap
     *   - Try fread() to read one Tuple from the file
     *   - If successful, return the Tuple pointer
     *   - If EOF or error, free the Tuple and return NULL
     *
     * close() behavior:
     *   - Close the FILE*
     *   - Free the state struct
     *   - Free the Operator struct itself
     */
    return NULL; /* Replace with your implementation */
}

/******************************************************************************
 * OPERATOR 2: SELECTION
 *
 * Pulls tuples from child one at a time and forwards only those where
 * tuple->values[attr] == value.  Non-matching tuples are discarded (freed).
 ******************************************************************************/

/**
 * selection – create a selection (filter) operator.
 *
 * @param child  Child operator to pull tuples from
 * @param attr   Attribute index to test (0-based)
 * @param value  Value to compare against
 * @return       Pointer to the constructed Operator
 */
Operator* selection(Operator *child, int attr, int value) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate a state struct holding child, attr, value
     * 2. Set op->next  = a function that loops calling child->next() until
     *                    it finds a matching tuple or child is exhausted
     * 3. Set op->close = a function that calls child->close(child) then frees
     *                    state and op
     * 4. Return the operator
     *
     * next() behavior:
     *   while ((t = child->next(child)) != NULL)
     *       if t->values[attr] == value  -> return t
     *       else free(t) and continue
     *   return NULL (exhausted)
     *
     * NOTE: Do NOT modify the tuple; just discard non-matching ones.
     */
    return NULL; /* Replace with your implementation */
}

/******************************************************************************
 * OPERATOR 3: PROJECTION
 *
 * For each tuple from child, outputs a new tuple containing only the
 * attribute indices listed in attrs[].  The output tuple's num_attr is
 * set to num_attrs.
 ******************************************************************************/

/**
 * projection – create a projection operator.
 *
 * @param child      Child operator to pull tuples from
 * @param attrs      Array of attribute indices to keep (0-based, caller owns)
 * @param num_attrs  Number of attributes to keep
 * @return           Pointer to the constructed Operator
 */
Operator* projection(Operator *child, int *attrs, int num_attrs) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Allocate a state struct holding child, a copy of attrs[], num_attrs
     *    (copy attrs[] so the caller can free their array)
     * 2. Set op->next  = a function that:
     *       a. Calls child->next(child)
     *       b. If NULL, returns NULL
     *       c. Allocates a new output Tuple
     *       d. For each i in [0, num_attrs): out->values[i] = in->values[attrs[i]]
     *       e. Sets out->num_attr = num_attrs
     *       f. Frees the input tuple
     *       g. Returns the output tuple
     * 3. Set op->close = a function that calls child->close(child), frees state, frees op
     * 4. Return the operator
     *
     * Example:
     *   Input  tuple = (A, B, C, D), num_attr=4
     *   attrs = {0, 2}, num_attrs = 2
     *   Output tuple = (A, C), num_attr=2
     */
    return NULL; /* Replace with your implementation */
}

/******************************************************************************
 * OPERATOR 4: NESTED LOOP JOIN
 *
 * Equi-join: for each left tuple, scan ALL right tuples; emit a concatenated
 * tuple when left.values[left_attr] == right.values[right_attr].
 *
 * The right side must be re-scanned for every left tuple.
 * A practical approach is to materialize the right side into a buffer once
 * and then iterate over the buffer for each left tuple.
 ******************************************************************************/

/**
 * nested_loop_join – create a nested loop join operator.
 *
 * @param left        Left child operator
 * @param right       Right child operator
 * @param left_attr   Attribute index in the left tuple used for join
 * @param right_attr  Attribute index in the right tuple used for join
 * @return            Pointer to the constructed Operator
 */
Operator* nested_loop_join(Operator *left, Operator *right,
                            int left_attr, int right_attr) {
    /* TODO: Implement this function
     *
     * Recommended approach (materialize right side):
     * 1. Allocate a state struct holding: left, right, left_attr, right_attr,
     *    a Tuple** buffer for the right side, right_count, right_capacity,
     *    a pointer to the current left tuple, current right index, initialized flag
     *
     * 2. Set op->next = a function that:
     *    a. On first call (initialized == 0):
     *         - Drain right->next() into right_buf[] (materialize all right tuples)
     *         - Fetch the first left tuple: left_tuple = left->next(left)
     *         - Set right_idx = 0, initialized = 1
     *    b. Loop:
     *         while left_tuple != NULL:
     *             while right_idx < right_count:
     *                 r = right_buf[right_idx++]
     *                 if left_tuple->values[left_attr] == r->values[right_attr]:
     *                     build concatenated output tuple (left fields then right fields)
     *                     return output tuple
     *             // right exhausted for this left tuple
     *             free(left_tuple)
     *             left_tuple = left->next(left)
     *             right_idx = 0
     *         return NULL
     *
     * 3. Output tuple: values = [left.values... , right.values...],
     *    num_attr = left.num_attr + right.num_attr (capped at MAX_ATTR)
     *
     * 4. Set op->close = a function that:
     *         - Frees left_tuple if non-NULL
     *         - Frees all tuples in right_buf[], then frees right_buf
     *         - Calls left->close(left) and right->close(right)
     *         - Frees state and op
     */
    return NULL; /* Replace with your implementation */
}

/******************************************************************************
 * BONUS: MATERIALIZATION MODEL (Optional – Extra Credit)
 *
 * Eagerly evaluates the child operator, stores all tuples in an in-memory
 * buffer, closes the child, then returns tuples one at a time from the buffer.
 ******************************************************************************/

/**
 * materialization – create a materialization operator.
 *
 * @param child  Child operator to fully evaluate
 * @return       Pointer to the constructed Operator
 */
Operator* materialization(Operator *child) {
    /* BONUS TODO: Implement this function
     *
     * Steps:
     * 1. Drain child->next() into a dynamically grown Tuple** buffer
     * 2. Call child->close(child) to release child resources
     * 3. Store the buffer and its size in a state struct
     * 4. Set op->next  = a function that returns buffer[idx++] (a copy), NULL when done
     * 5. Set op->close = a function that frees all buffered tuples, buffer, state, op
     * 6. Return the operator
     *
     * This implements the "store-then-serve" (blocking) execution model as opposed
     * to the pipelined Volcano model.
     */
    return NULL; /* Replace with your implementation */
}
