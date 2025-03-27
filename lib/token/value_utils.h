#ifndef OPENPAYGO_TOKEN_VALUE_UTILS_H_
#define OPENPAYGO_TOKEN_VALUE_UTILS_H_

#include <stdint.h>

uint16_t decode_base(uint16_t starting_code_base, uint16_t token_base);

uint16_t get_token_base(uint32_t token);

uint32_t put_base_in_token(uint32_t token, uint16_t token_base);

#endif /* OPENPAYGO_TOKEN_VALUE_UTILS_H_ */