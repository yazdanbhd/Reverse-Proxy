#ifndef COLLECT_LOGS_H
#define COLLECT_LOGS_H

#include "shared-logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function to generate a log collection response
char *generate_log_collection_response(int *response_length);

// function to format a single log entry
char *format_log_entry(const log_entry_t *entry);

#endif // COLLECT_LOGS_H