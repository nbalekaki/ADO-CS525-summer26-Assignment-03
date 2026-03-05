#ifndef DBERROR_H
#define DBERROR_H

#include <stdio.h>
#include <stdlib.h>

/* return code definitions */
typedef int RC;

#define RC_OK                   0
#define RC_FILE_NOT_FOUND       1
#define RC_FILE_READ_FAILED     2
#define RC_INVALID_ATTR_INDEX   3
#define RC_OPERATOR_NOT_INIT    4
#define RC_NULL_POINTER         5

/* Query Engine Error Codes */
#define RC_QE_OPERATOR_NOT_INIT 100
#define RC_QE_NO_MORE_TUPLES    101
#define RC_QE_INVALID_ATTR      102
#define RC_QE_FILE_ERROR        103

/* holder for error messages */
extern char *RC_message;

/* print a message to standard out describing the error */
extern void printError(RC error);
extern char *errorMessage(RC error);

#define THROW(rc, message) \
    do { \
        RC_message = message; \
        return rc; \
    } while (0)

/* check the return code and exit if it is an error */
#define CHECK(code) \
    do { \
        int rc_internal = (code); \
        if (rc_internal != RC_OK) { \
            char *message = errorMessage(rc_internal); \
            printf("[%s-L%i-%s] ERROR: Operation returned error: %s\n", \
                   __FILE__, __LINE__, __TIME__, message); \
            free(message); \
            exit(1); \
        } \
    } while(0)

#endif /* DBERROR_H */
