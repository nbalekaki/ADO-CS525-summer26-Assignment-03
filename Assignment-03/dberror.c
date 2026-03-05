/*******************************************************************************
 * Advanced Database Organization - Assignment 03
 * Error handling utilities
 ******************************************************************************/

#include "dberror.h"
#include <string.h>

char *RC_message = "";

void printError(RC error) {
    char *msg = errorMessage(error);
    printf("EC (%i), \"%s\"\n", error, msg);
    free(msg);
}

char *errorMessage(RC error) {
    char *msg;
    switch (error) {
        case RC_OK:                   msg = "OK";                          break;
        case RC_FILE_NOT_FOUND:       msg = "File not found";              break;
        case RC_FILE_READ_FAILED:     msg = "File read failed";            break;
        case RC_INVALID_ATTR_INDEX:   msg = "Invalid attribute index";     break;
        case RC_OPERATOR_NOT_INIT:    msg = "Operator not initialized";    break;
        case RC_NULL_POINTER:         msg = "NULL pointer";                break;
        case RC_QE_OPERATOR_NOT_INIT: msg = "Query engine operator not initialized"; break;
        case RC_QE_NO_MORE_TUPLES:    msg = "No more tuples";              break;
        case RC_QE_INVALID_ATTR:      msg = "Invalid attribute";           break;
        case RC_QE_FILE_ERROR:        msg = "Query engine file error";     break;
        default:                      msg = "Unknown error";               break;
    }
    char *result = (char *)malloc(strlen(msg) + 1);
    strcpy(result, msg);
    return result;
}
