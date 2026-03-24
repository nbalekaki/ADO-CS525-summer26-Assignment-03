# Programming Assignment 3: Query Execution Engine – Iterator (Volcano) Model

## Overview

In this assignment, you will build a simplified relational query execution engine
using the **Iterator (Volcano) model**. You will implement four core relational
operators that can be composed into an execution pipeline where tuples flow through
one at a time using a uniform `next()` interface.

## Files

| File | Description |
|------|-------------|
| `operator.h` | Operator interface: `Tuple`, `Operator` structs, function declarations |
| `operators.c` | **Your implementation goes here** |
| `dberror.h` | Error codes and macros |
| `dberror.c` | Error message utilities |
| `dt.h` | Boolean type definitions |
| `test_helper.h` | Test assertion macros |
| `test_operators_basic.c` | Basic tests (student version) |
| `Makefile` | Build and test targets |

## Building and Testing

```bash
# Build
make

# Run tests
make test

# Memory check (requires Valgrind on Linux/WSL)
make memcheck

# Clean
make clean
```

## The Iterator (Volcano) Model

Every operator exposes the same interface:

```c
typedef struct Operator {
    void   *state;
    Tuple* (*next) (struct Operator *op);  /* pull one tuple; NULL = exhausted */
    void   (*close)(struct Operator *op);  /* free all resources               */
} Operator;
```

- **Pull-based**: execution starts at the root and recursively pulls from children
- **Tuple-at-a-time**: one tuple produced per `next()` call
- **Pipelined**: no intermediate results are stored (except in the join, see below)

## Data Model

All relations are stored as flat binary files. Each tuple has this layout:

```c
#define MAX_ATTR 8

typedef struct {
    int values[MAX_ATTR];  /* all attributes are integers */
    int num_attr;          /* number of valid attributes  */
} Tuple;
```

Use `write_tuples_to_file()` in your tests to create relation files.

## Operators to Implement

### 1. Table Scan
```c
Operator* table_scan(const char *filename);
```
Opens a binary file and returns one `Tuple` per `next()` call. Returns `NULL` at EOF.

### 2. Selection
```c
Operator* selection(Operator *child, int attr, int value);
```
Returns only tuples where `tuple->values[attr] == value`. Non-matching tuples are discarded.

### 3. Projection
```c
Operator* projection(Operator *child, int *attrs, int num_attrs);
```
For each input tuple, outputs a new tuple containing only the attributes listed in `attrs[]`.
Sets output `num_attr = num_attrs`.

**Example:** Input = `(A, B, C, D)`, `attrs = {0, 2}` → Output = `(A, C)`, `num_attr = 2`

### 4. Nested Loop Join
```c
Operator* nested_loop_join(Operator *left, Operator *right,
                            int left_attr, int right_attr);
```
Equi-join: emits a concatenated tuple when `left.values[left_attr] == right.values[right_attr]`.
The right side must be re-scanned for every left tuple (materialize it into a buffer).

## Example Query Pipeline

SQL equivalent:
```sql
SELECT A.col0, B.col1
FROM A JOIN B ON A.col1 = B.col0
WHERE A.col0 = 10;
```

Iterator construction:
```c
Operator *scanA = table_scan("A.bin");
Operator *selA  = selection(scanA, 0, 10);
Operator *scanB = table_scan("B.bin");
Operator *join  = nested_loop_join(selA, scanB, 1, 0);
int attrs[]     = {0, 3};
Operator *proj  = projection(join, attrs, 2);

/* Drive execution */
Tuple *t;
while ((t = proj->next(proj)) != NULL) {
    printf("(%d, %d)\n", t->values[0], t->values[1]);
    free(t);
}
proj->close(proj);   /* recursively closes all children */
```

## Memory Management Rules

- Each `next()` call returns a **heap-allocated** `Tuple*` that the caller must `free()`.
- Each `close()` must **free** the operator's state, close child operators, and free the `Operator` struct itself.
- Run **Valgrind** before submitting: `make memcheck`

## Operator Tree Diagram

```
               PROJECTION (cols 0, 3)
                       |
           NESTED LOOP JOIN (A.col1 = B.col0)
              /                    \
    SELECTION (A.col0=10)       TABLE SCAN B
             |
       TABLE SCAN A
```

## Assumptions

- All attributes are integers (no strings, floats, or NULLs)
- Relations stored as flat binary files (Tuple structs in sequence)
- No schema system required
- No duplicate elimination
- Single-threaded (no concurrency)
- Caller allocates `attrs[]` for projection; implementation should copy it

## Bonus (Optional – Extra Credit)

Implement one of these alternative execution models:

- **Materialization**: `Operator* materialization(Operator *child)` – fully evaluates
  child into a buffer, then returns tuples one at a time from the buffer
- **Vectorized model**: process batches of tuples at a time

Document your bonus implementation in this README, comparing it to the Volcano model.

## Deliverables

- `operators.c` (your implementation)
- `README.md` (updated with design notes, execution flow, and operator tree diagram)
- Test output demonstrating correctness
