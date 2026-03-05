#ifndef OPERATOR_H
#define OPERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************************************
 *                    data structures                       *
 ************************************************************/

/* Maximum number of attributes per tuple */
#define MAX_ATTR 8

/*
 * Tuple: fixed-width in-memory representation.
 * All attributes are integers; no NULLs; no schema required.
 */
typedef struct {
    int values[MAX_ATTR];
    int num_attr;
} Tuple;

/*
 * Operator: the uniform interface for all relational operators.
 *
 * next()  – produce the next output tuple (caller must free it)
 *           returns NULL when the operator is exhausted
 * close() – release all resources held by this operator
 *           (also closes and frees child operators recursively)
 */
typedef struct Operator {
    void   *state;
    Tuple* (*next) (struct Operator *op);
    void   (*close)(struct Operator *op);
} Operator;

/************************************************************
 *                    operator constructors                 *
 ************************************************************/

/*
 * table_scan – sequential scan of a binary relation file.
 * The file stores Tuple structs written with write_tuples_to_file().
 */
Operator* table_scan(const char *filename);

/*
 * selection – filters tuples from child where tuple[attr] == value.
 */
Operator* selection(Operator *child, int attr, int value);

/*
 * projection – keeps only the listed attribute indices.
 * attrs[]   : array of attribute indices to keep (0-based)
 * num_attrs : length of attrs[]
 * Output tuple's num_attr == num_attrs.
 */
Operator* projection(Operator *child, int *attrs, int num_attrs);

/*
 * nested_loop_join – equi-join on left.values[left_attr] == right.values[right_attr].
 * Output tuple = concatenation of matching left and right tuples.
 * The right side is re-scanned for every left tuple.
 */
Operator* nested_loop_join(Operator *left, Operator *right,
                            int left_attr, int right_attr);

/************************************************************
 *                    bonus operators                       *
 ************************************************************/

/*
 * BONUS: materialization – eagerly evaluates child into an in-memory buffer,
 * then returns tuples one at a time from the buffer.
 * Child is closed after materialization is complete.
 */
Operator* materialization(Operator *child);

/************************************************************
 *                    utilities                             *
 ************************************************************/

/*
 * write_tuples_to_file – write an array of Tuples to a binary file.
 * Used by tests to create relation files.
 */
void write_tuples_to_file(const char *filename, Tuple *tuples, int count);

#endif /* OPERATOR_H */
