/*******************************************************************************
 * Advanced Database Organization - Assignment 03
 * Query Execution Engine – Basic Tests (Student Version)
 *
 * These are BASIC tests to help you get started.
 * The autograder runs more comprehensive hidden tests.
 * Passing these tests does NOT guarantee full marks.
 *
 * Grading (hidden tests): 80 points automated + 10 BONUS
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operator.h"
#include "dberror.h"
#include "test_helper.h"

char *testName;
int testsPassed = 0;
int testsFailed = 0;

/* Temporary binary files for tests */
#define FILE_A  "test_rel_A.bin"
#define FILE_B  "test_rel_B.bin"

static void cleanup(void) {
    remove(FILE_A);
    remove(FILE_B);
}

/******************************************************************************
 * Test: Table Scan – read all tuples from a file
 ******************************************************************************/
void testTableScan(void) {
    testName = "Table Scan Basic";
    printf(">> TEST: %s\n", testName);

    /* Create a small relation: 3 tuples, 2 attributes each */
    Tuple data[3];
    data[0].num_attr = 2; data[0].values[0] = 10; data[0].values[1] = 100;
    data[1].num_attr = 2; data[1].values[0] = 20; data[1].values[1] = 200;
    data[2].num_attr = 2; data[2].values[0] = 30; data[2].values[1] = 300;
    write_tuples_to_file(FILE_A, data, 3);

    Operator *scan = table_scan(FILE_A);
    ASSERT_NOT_NULL(scan, "table_scan should return non-NULL operator");

    Tuple *t;

    t = scan->next(scan);
    ASSERT_NOT_NULL(t, "first tuple should not be NULL");
    ASSERT_EQUALS_INT(10, t->values[0], "first tuple attr[0] = 10");
    ASSERT_EQUALS_INT(100, t->values[1], "first tuple attr[1] = 100");
    free(t);

    t = scan->next(scan);
    ASSERT_NOT_NULL(t, "second tuple should not be NULL");
    ASSERT_EQUALS_INT(20, t->values[0], "second tuple attr[0] = 20");
    free(t);

    t = scan->next(scan);
    ASSERT_NOT_NULL(t, "third tuple should not be NULL");
    ASSERT_EQUALS_INT(30, t->values[0], "third tuple attr[0] = 30");
    free(t);

    t = scan->next(scan);
    ASSERT_NULL(t, "fourth next() should return NULL (exhausted)");

    scan->close(scan);
    cleanup();
    TEST_DONE();
}

/******************************************************************************
 * Test: Selection – filter tuples by attribute value
 ******************************************************************************/
void testSelection(void) {
    testName = "Selection Basic";
    printf(">> TEST: %s\n", testName);

    /* Relation: 5 tuples; attr[0] = department id */
    Tuple data[5];
    for (int i = 0; i < 5; i++) {
        data[i].num_attr = 2;
        data[i].values[0] = i % 2;   /* alternating 0,1,0,1,0 */
        data[i].values[1] = i * 10;
    }
    write_tuples_to_file(FILE_A, data, 5);

    /* SELECT * FROM A WHERE attr[0] = 1 -> should return tuples 1 and 3 */
    Operator *scan = table_scan(FILE_A);
    Operator *sel  = selection(scan, 0, 1);
    ASSERT_NOT_NULL(sel, "selection operator should not be NULL");

    Tuple *t;

    t = sel->next(sel);
    ASSERT_NOT_NULL(t, "first selected tuple should not be NULL");
    ASSERT_EQUALS_INT(1, t->values[0], "selected tuple attr[0] = 1");
    ASSERT_EQUALS_INT(10, t->values[1], "selected tuple attr[1] = 10");
    free(t);

    t = sel->next(sel);
    ASSERT_NOT_NULL(t, "second selected tuple should not be NULL");
    ASSERT_EQUALS_INT(1, t->values[0], "second selected attr[0] = 1");
    ASSERT_EQUALS_INT(30, t->values[1], "second selected attr[1] = 30");
    free(t);

    t = sel->next(sel);
    ASSERT_NULL(t, "no more tuples after selection");

    sel->close(sel);
    cleanup();
    TEST_DONE();
}

/******************************************************************************
 * Test: Projection – keep only selected attributes
 ******************************************************************************/
void testProjection(void) {
    testName = "Projection Basic";
    printf(">> TEST: %s\n", testName);

    /* Relation: 3 tuples, 4 attributes: (A, B, C, D) */
    Tuple data[3];
    for (int i = 0; i < 3; i++) {
        data[i].num_attr = 4;
        data[i].values[0] = i + 1;      /* A */
        data[i].values[1] = (i + 1)*10; /* B */
        data[i].values[2] = (i + 1)*100;/* C */
        data[i].values[3] = (i + 1)*1000;/* D */
    }
    write_tuples_to_file(FILE_A, data, 3);

    /* PROJECT (A, C) = attributes {0, 2} */
    int attrs[] = {0, 2};
    Operator *scan = table_scan(FILE_A);
    Operator *proj = projection(scan, attrs, 2);
    ASSERT_NOT_NULL(proj, "projection operator should not be NULL");

    Tuple *t;

    t = proj->next(proj);
    ASSERT_NOT_NULL(t, "first projected tuple should not be NULL");
    ASSERT_EQUALS_INT(2, t->num_attr, "projected tuple has 2 attributes");
    ASSERT_EQUALS_INT(1, t->values[0], "projected attr[0] = A = 1");
    ASSERT_EQUALS_INT(100, t->values[1], "projected attr[1] = C = 100");
    free(t);

    t = proj->next(proj);
    ASSERT_NOT_NULL(t, "second projected tuple should not be NULL");
    ASSERT_EQUALS_INT(2, t->num_attr, "second tuple has 2 attributes");
    ASSERT_EQUALS_INT(2, t->values[0], "second projected attr[0] = 2");
    ASSERT_EQUALS_INT(200, t->values[1], "second projected attr[1] = 200");
    free(t);

    t = proj->next(proj);  /* third */
    ASSERT_NOT_NULL(t, "third projected tuple should not be NULL");
    free(t);

    t = proj->next(proj);
    ASSERT_NULL(t, "no more tuples after projection");

    proj->close(proj);
    cleanup();
    TEST_DONE();
}

/******************************************************************************
 * Test: Nested Loop Join – basic equi-join
 ******************************************************************************/
void testNestedLoopJoin(void) {
    testName = "Nested Loop Join Basic";
    printf(">> TEST: %s\n", testName);

    /*
     * Relation A: 3 tuples (id, valA)
     *   (1, 10), (2, 20), (3, 30)
     *
     * Relation B: 3 tuples (id, valB)
     *   (2, 200), (3, 300), (4, 400)
     *
     * JOIN A.id == B.id  ->  (2,20, 2,200) and (3,30, 3,300)
     */
    Tuple dataA[3], dataB[3];
    for (int i = 0; i < 3; i++) {
        dataA[i].num_attr = 2;
        dataA[i].values[0] = i + 1;
        dataA[i].values[1] = (i + 1) * 10;
    }
    dataB[0].num_attr = 2; dataB[0].values[0] = 2; dataB[0].values[1] = 200;
    dataB[1].num_attr = 2; dataB[1].values[0] = 3; dataB[1].values[1] = 300;
    dataB[2].num_attr = 2; dataB[2].values[0] = 4; dataB[2].values[1] = 400;

    write_tuples_to_file(FILE_A, dataA, 3);
    write_tuples_to_file(FILE_B, dataB, 3);

    Operator *scanA = table_scan(FILE_A);
    Operator *scanB = table_scan(FILE_B);
    Operator *join  = nested_loop_join(scanA, scanB, 0, 0); /* join on attr[0] */
    ASSERT_NOT_NULL(join, "join operator should not be NULL");

    Tuple *t;
    int count = 0;

    /* First matching tuple: A=(2,20) joined with B=(2,200) */
    t = join->next(join);
    ASSERT_NOT_NULL(t, "first join result should not be NULL");
    ASSERT_EQUALS_INT(4, t->num_attr, "joined tuple has 4 attributes");
    ASSERT_EQUALS_INT(2, t->values[0], "joined tuple left attr[0] = 2");
    ASSERT_EQUALS_INT(20, t->values[1], "joined tuple left attr[1] = 20");
    ASSERT_EQUALS_INT(2, t->values[2], "joined tuple right attr[0] = 2");
    ASSERT_EQUALS_INT(200, t->values[3], "joined tuple right attr[1] = 200");
    free(t);
    count++;

    /* Second matching tuple: A=(3,30) joined with B=(3,300) */
    t = join->next(join);
    ASSERT_NOT_NULL(t, "second join result should not be NULL");
    ASSERT_EQUALS_INT(3, t->values[0], "joined tuple left attr[0] = 3");
    ASSERT_EQUALS_INT(300, t->values[3], "joined tuple right attr[1] = 300");
    free(t);
    count++;

    t = join->next(join);
    ASSERT_NULL(t, "no more join results");

    ASSERT_EQUALS_INT(2, count, "join should produce exactly 2 results");

    join->close(join);
    cleanup();
    TEST_DONE();
}

/******************************************************************************
 * Test: Operator chaining – scan + selection + projection
 ******************************************************************************/
void testOperatorChain(void) {
    testName = "Operator Chaining (Scan + Select + Project)";
    printf(">> TEST: %s\n", testName);

    /*
     * Relation: 6 tuples (dept, salary, name_code)
     * SELECT salary FROM employees WHERE dept = 2
     */
    Tuple data[6];
    int depts[]    = {1, 2, 2, 1, 2, 3};
    int salaries[] = {50, 80, 90, 60, 70, 55};
    for (int i = 0; i < 6; i++) {
        data[i].num_attr = 3;
        data[i].values[0] = depts[i];
        data[i].values[1] = salaries[i];
        data[i].values[2] = i + 100;
    }
    write_tuples_to_file(FILE_A, data, 6);

    /* Pipeline: scan -> select(dept==2) -> project(salary) */
    Operator *scan = table_scan(FILE_A);
    Operator *sel  = selection(scan, 0, 2);
    int attrs[] = {1};
    Operator *proj = projection(sel, attrs, 1);

    Tuple *t;
    int count = 0;
    int salary_sum = 0;

    while ((t = proj->next(proj)) != NULL) {
        ASSERT_EQUALS_INT(1, t->num_attr, "projected tuple has 1 attribute");
        salary_sum += t->values[0];
        count++;
        free(t);
    }

    ASSERT_EQUALS_INT(3, count, "chain: dept=2 has 3 tuples");
    ASSERT_EQUALS_INT(240, salary_sum, "chain: salary sum = 80+90+70 = 240");

    proj->close(proj);
    cleanup();
    TEST_DONE();
}

/******************************************************************************
 * Test: Full pipeline – scan + select + join + project (4 operators)
 ******************************************************************************/
void testFullPipeline(void) {
    testName = "Full Pipeline (4 Operators)";
    printf(">> TEST: %s\n", testName);

    /*
     * SQL equivalent:
     *   SELECT A.col0, B.col1
     *   FROM A JOIN B ON A.col1 = B.col0
     *   WHERE A.col0 = 10;
     *
     * Relation A: (col0, col1) = (10, 2), (20, 3), (10, 4)
     * Relation B: (col0, col1) = (2, 200), (3, 300), (4, 400), (5, 500)
     *
     * WHERE A.col0=10: rows (10,2) and (10,4)
     * JOIN A.col1=B.col0:
     *   (10,2) JOIN (2,200) -> (10, 2, 2, 200) -> PROJECT(0,3) -> (10, 200)
     *   (10,4) JOIN (4,400) -> (10, 4, 4, 400) -> PROJECT(0,3) -> (10, 400)
     */
    Tuple dataA[3], dataB[4];
    dataA[0].num_attr=2; dataA[0].values[0]=10; dataA[0].values[1]=2;
    dataA[1].num_attr=2; dataA[1].values[0]=20; dataA[1].values[1]=3;
    dataA[2].num_attr=2; dataA[2].values[0]=10; dataA[2].values[1]=4;

    dataB[0].num_attr=2; dataB[0].values[0]=2; dataB[0].values[1]=200;
    dataB[1].num_attr=2; dataB[1].values[0]=3; dataB[1].values[1]=300;
    dataB[2].num_attr=2; dataB[2].values[0]=4; dataB[2].values[1]=400;
    dataB[3].num_attr=2; dataB[3].values[0]=5; dataB[3].values[1]=500;

    write_tuples_to_file(FILE_A, dataA, 3);
    write_tuples_to_file(FILE_B, dataB, 4);

    Operator *scanA = table_scan(FILE_A);
    Operator *selA  = selection(scanA, 0, 10);        /* WHERE A.col0=10 */
    Operator *scanB = table_scan(FILE_B);
    Operator *join  = nested_loop_join(selA, scanB, 1, 0); /* A.col1=B.col0 */
    int attrs[] = {0, 3};
    Operator *proj  = projection(join, attrs, 2);     /* PROJECT(A.col0, B.col1) */

    Tuple *t;
    int count = 0;

    t = proj->next(proj);
    ASSERT_NOT_NULL(t, "first pipeline result should not be NULL");
    ASSERT_EQUALS_INT(2, t->num_attr, "pipeline result has 2 attributes");
    ASSERT_EQUALS_INT(10, t->values[0], "pipeline result attr[0] = 10");
    ASSERT_EQUALS_INT(200, t->values[1], "pipeline result attr[1] = 200");
    free(t);
    count++;

    t = proj->next(proj);
    ASSERT_NOT_NULL(t, "second pipeline result should not be NULL");
    ASSERT_EQUALS_INT(10, t->values[0], "second result attr[0] = 10");
    ASSERT_EQUALS_INT(400, t->values[1], "second result attr[1] = 400");
    free(t);
    count++;

    t = proj->next(proj);
    ASSERT_NULL(t, "no more pipeline results");

    ASSERT_EQUALS_INT(2, count, "full pipeline should produce 2 results");

    proj->close(proj);
    cleanup();
    TEST_DONE();
}

/******************************************************************************
 * MAIN
 ******************************************************************************/
int main(void) {
    printf("============================================================\n");
    printf("  QUERY EXECUTION ENGINE – BASIC TESTS (Student Version)\n");
    printf("  Note: Hidden tests are more comprehensive!\n");
    printf("============================================================\n\n");

    cleanup();

    testTableScan();
    testSelection();
    testProjection();
    testNestedLoopJoin();
    testOperatorChain();
    testFullPipeline();

    cleanup();

    printf("\n============================================================\n");
    printf("Tests Passed: %d\n", testsPassed);
    printf("Tests Failed: %d\n", testsFailed);
    printf("============================================================\n");

    return (testsFailed == 0) ? 0 : 1;
}
