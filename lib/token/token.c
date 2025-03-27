#include "core.h"
#include "value_utils.h"
#include "utils.h"

#include <openpaygo/token.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(openpaygo_token, CONFIG_OPENPAYGO_TOKEN_LOG_LEVEL);


#ifdef CONFIG_OPENPAYGO_TOKEN_RESTRICTED_DIGIT_SET_MODE
#include "restricted_digit_mode.h"
#endif

#if CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 0
#include "unused_older_tokens.h"
#endif

bool is_count_valid(uint16_t count, uint16_t max_count, uint16_t value,
                    uint16_t used_counts) {
  if (value == OPENPAYGO_TOKEN_COUNTER_SYNC_VALUE) {
    if (count > max_count - CONFIG_OPENPAYGO_TOKEN_MAX_TOKEN_JUMP) {
      return true;
    }
  } else if (count > max_count) {
    return true;
  }
#if CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 0
  else if (count > 0 &&
           count > max_count -
                       (CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS * 2) &&
           IS_ADD_TOKEN(count) &&
           !is_in_used_counts(count, max_count, used_counts)) {
    return true;
  }
#endif
  return false;
}

int16_t openpaygo_token_decode(const struct openpaygo_token_config *conf,
                               struct openpaygo_token_state *state,
                               uint64_t input_token) {
#ifdef CONFIG_OPENPAYGO_TOKEN_RESTRICTED_DIGIT_SET_MODE
  input_token = convert_from_four_digit_token(input_token);
#endif
  uint16_t starting_code_base = get_token_base(conf->starting_code);
  uint16_t token_base = get_token_base((uint32_t)input_token);
  uint32_t current_token = put_base_in_token(conf->starting_code, token_base);

  uint32_t masked_token;
  int max_count_try;
  int value = decode_base(starting_code_base, token_base);
#if CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 0
  bool valid_older_token = false;
#endif
  if (value == OPENPAYGO_TOKEN_COUNTER_SYNC_VALUE) {
    max_count_try =
        state->count + CONFIG_OPENPAYGO_TOKEN_MAX_TOKEN_JUMP_COUNTER_SYNC;
  } else {
    max_count_try = state->count + CONFIG_OPENPAYGO_TOKEN_MAX_TOKEN_JUMP;
  }

  for (uint16_t count = 0; count <= max_count_try; count++) {
    masked_token = put_base_in_token(current_token, token_base);
    if (masked_token == input_token) {
      LOG_DBG("Count %d", count);
      if (is_count_valid(count, state->count, value, state->used_counts)) {
#if CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 0
        mark_count_as_used(count, state->count, &state->used_counts, value);
        LOG_DBG("Used_counts: %04X", state->used_counts);
#endif
        if (count > state->count) {
          state->count = count;
        }
        return value;
      }
#if CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 0
      else {
        valid_older_token = true;
      }
#endif
    }
    current_token =
        generate_openpaygo_token(current_token, (uint8_t *)conf->secret_key);
  }
#if CONFIG_OPENPAYGO_TOKEN_CONFIG_OPENPAYGO_TOKEN_MAX_UNUSED_OLDER_TOKENS > 0
  if (valid_older_token) {
    return OPENPAYGO_TOKEN_OLDER_TOKEN_VALID;
  }
#endif
  return OPENPAYGO_TOKEN_TOKEN_INVALID;
}