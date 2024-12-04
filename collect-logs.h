#ifndef COLLECT_LOGS_H
#define COLLECT_LOGS_H

#include "shared-logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *generate_log_collection_response(int *response_length);
char *format_log_entry(const log_entry_t *entry);

#endif // COLLECT_LOGS_H