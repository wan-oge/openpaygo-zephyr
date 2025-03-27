#include "unused_older_tokens.h"
#include "utils.h"
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(openpaygo_token, CONFIG_OPENPAYGO_TOKEN_LOG_LEVEL);

#define CHECK_BIT(variable, position) ((variable) & (1 << (position)))

void mark_count_as_used(uint16_t count, uint16_t max_count, used_counts_t *used_counts, uint16_t value){
    uint16_t new_used_count = 0;
    if(IS_SET_TOKEN(count) || value == OPENPAYGO_TOKEN_COUNTER_SYNC_VALUE || value == OPENPAYGO_TOKEN_PAYG_DISABLE_VALUE){
        *used_counts = USED_COUNTS_MAX;
        return;
    } else {
        if(count > max_count){
            uint16_t shift_distance = (count - max_count + 1) / 2;
            new_used_count = (*used_counts << shift_distance) | 1;
            // LOG_INF("New used count: %04X", new_used_count);
            LOG_DBG("Count: %d, Max Count: %d, Shift Distance: %d, OLD: %04X, NEW: %04X", count, max_count, shift_distance, *used_counts, new_used_count);
            *used_counts = new_used_count;
        } else {
            // In this case, both the count and the max count are even
            if ((max_count - count) < CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS * 2){
                uint16_t shift_distance = (max_count - count) / 2;
                new_used_count = *used_counts | (1 << shift_distance);
                LOG_DBG("Count: %d, Max Count: %d, Shift Distance: %d, OLD: %04X, NEW: %04X", count, max_count, shift_distance, *used_counts, new_used_count);
                *used_counts = new_used_count;
            }
        }
    }
}

bool is_in_used_counts(uint16_t count, uint16_t max_count, used_counts_t used_counts){
    if(IS_SET_TOKEN(count) || count == max_count){
        return true;
    }
    uint16_t relative_count = max_count - count;
    relative_count = relative_count / 2;
    return CHECK_BIT(used_counts, relative_count);
}