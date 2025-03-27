#ifndef OPENPAYGO_TOKEN_UNUSED_OLDER_TOKENS_H_
#define OPENPAYGO_TOKEN_UNUSED_OLDER_TOKENS_H_

#include <openpaygo/token.h>

void mark_count_as_used(uint16_t count, uint16_t max_count, used_counts_t *used_counts, uint16_t value);
bool is_in_used_counts(uint16_t count, uint16_t max_count, used_counts_t used_counts);

#endif /* OPENPAYGO_TOKEN_UNUSED_OLDER_TOKENS_H_ */