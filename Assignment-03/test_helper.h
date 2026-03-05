#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* var to store the current test's name */
extern char *testName;

/* counter for passed and failed tests */
extern int testsPassed;
extern int testsFailed;

/* short cut for test information */
#define TEST_INFO  __FILE__, testName, __LINE__, __TIME__

/* check whether two ints are equal */
#define ASSERT_EQUALS_INT(expected, real, message) \
    do { \
        if ((expected) != (real)) { \
            printf("[%s-%s-L%i-%s] FAILED: expected <%i> but was <%i>: %s\n", \
                   TEST_INFO, expected, real, message); \
            testsFailed++; \
            return; \
        } \
        printf("[%s-%s-L%i-%s] OK: expected <%i> and was <%i>: %s\n", \
               TEST_INFO, expected, real, message); \
        testsPassed++; \
    } while(0)

/* check whether two strings are equal */
#define ASSERT_EQUALS_STRING(expected, real, message) \
    do { \
        if (strcmp((expected), (real)) != 0) { \
            printf("[%s-%s-L%i-%s] FAILED: expected <%s> but was <%s>: %s\n", \
                   TEST_INFO, expected, real, message); \
            testsFailed++; \
            return; \
        } \
        printf("[%s-%s-L%i-%s] OK: expected <%s> and was <%s>: %s\n", \
               TEST_INFO, expected, real, message); \
        testsPassed++; \
    } while(0)

/* check whether condition is true */
#define ASSERT_TRUE(real, message) \
    do { \
        if (!(real)) { \
            printf("[%s-%s-L%i-%s] FAILED: expected true: %s\n", \
                   TEST_INFO, message); \
            testsFailed++; \
            return; \
        } \
        printf("[%s-%s-L%i-%s] OK: expected true: %s\n", TEST_INFO, message); \
        testsPassed++; \
    } while(0)

/* check whether a pointer is not NULL */
#define ASSERT_NOT_NULL(ptr, message) \
    do { \
        if ((ptr) == NULL) { \
            printf("[%s-%s-L%i-%s] FAILED: expected non-NULL: %s\n", \
                   TEST_INFO, message); \
            testsFailed++; \
            return; \
        } \
        printf("[%s-%s-L%i-%s] OK: non-NULL pointer: %s\n", TEST_INFO, message); \
        testsPassed++; \
    } while(0)

/* check whether a pointer is NULL */
#define ASSERT_NULL(ptr, message) \
    do { \
        if ((ptr) != NULL) { \
            printf("[%s-%s-L%i-%s] FAILED: expected NULL: %s\n", \
                   TEST_INFO, message); \
            testsFailed++; \
            return; \
        } \
        printf("[%s-%s-L%i-%s] OK: got NULL as expected: %s\n", TEST_INFO, message); \
        testsPassed++; \
    } while(0)

/* test worked */
#define TEST_DONE() \
    do { \
        printf("[%s-%s-L%i-%s] OK: finished test\n\n", TEST_INFO); \
    } while (0)

#endif /* TEST_HELPER_H */
