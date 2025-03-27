#include "value_utils.h"
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(openpaygo_token, CONFIG_OPENPAYGO_TOKEN_LOG_LEVEL);

uint16_t decode_base(uint16_t starting_code_base, uint16_t token_base){
    if(token_base < starting_code_base){
        return token_base + 1000 - starting_code_base;
    } else {
        return token_base - starting_code_base;
    }
}

uint16_t get_token_base(uint32_t token){
    return token % 1000;
}

uint32_t put_base_in_token(uint32_t token, uint16_t token_base){
    return token - (token % 1000) + token_base;
}